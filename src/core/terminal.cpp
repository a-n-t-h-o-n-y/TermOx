#include <ox/core/terminal.hpp>

#include <algorithm>
#include <cassert>

#include <esc/detail/signals.hpp>
#include <esc/detail/transcode.hpp>
#include <esc/io.hpp>
#include <esc/sequence.hpp>
#include <esc/terminal.hpp>

namespace ox {

ScreenBuffer::ScreenBuffer(Area size)
    : size_{size}, buffer_((std::size_t)(size.width * size.height))
{}

auto ScreenBuffer::operator[](Point p) -> Glyph&
{
    return buffer_[(std::size_t)(p.y * size_.width + p.x)];
}

auto ScreenBuffer::operator[](Point p) const -> Glyph const&
{
    return buffer_[(std::size_t)(p.y * size_.width + p.x)];
}

auto ScreenBuffer::resize(Area a) -> void
{
    size_ = a;
    buffer_.resize((std::size_t)(a.width * a.height));
}

auto ScreenBuffer::fill(Glyph const& g) -> void
{
    for (auto& glyph : buffer_) {
        glyph = g;
    }
}

// -------------------------------------------------------------------------------------

TimerThread::TimerThread(std::chrono::milliseconds duration, CallbackType callback)
    : thread_{[cb = std::move(callback), d = std::move(duration)](auto st) {
          TimerThread::run(st, cb, d);
      }}
{}

auto TimerThread::run(std::stop_token st,
                      CallbackType const& callback,
                      std::chrono::milliseconds duration) -> void
{
    constexpr auto timeout_duration =
        std::chrono::duration_cast<ClockType::duration>(std::chrono::milliseconds{16});
    auto next_callback_time = ClockType::now() + duration;
    while (true) {
        if (st.stop_requested()) { return; }

        auto const now = ClockType::now();
        if (now >= next_callback_time) {
            callback();
            next_callback_time += duration;
        }

        auto const time_to_wait = std::min(next_callback_time - now, timeout_duration);
        std::this_thread::sleep_for(time_to_wait);
    }
}

// -------------------------------------------------------------------------------------

Terminal::Terminal(MouseMode mouse_mode, KeyMode key_mode, Signals signals)
    : terminal_input_thread_{[this](auto st) { this->run_read_loop(st); }}
{
    esc::initialize_interactive_terminal(mouse_mode, key_mode, signals);
}

Terminal::~Terminal()
{
    terminal_input_thread_.request_stop();
    esc::uninitialize_terminal();
}

void Terminal::commit_changes()
{
    escape_sequence_.clear();

    if (Terminal::changes.size() != current_screen_.size()) {
        current_screen_.resize(Terminal::changes.size());
        current_screen_.fill(Glyph{U'\0'});  // Trigger Repaint
    }

    auto brush = Brush{};

    for (auto y = 0; y < changes.size().height; ++y) {
        for (auto x = 0; x < changes.size().width; ++x) {
            auto const& change = changes[{x, y}];
            auto const& current = current_screen_[{x, y}];
            if (change != current) {
                escape_sequence_ += escape(esc::Cursor{x, y});
                if (change.brush != brush) {
                    escape_sequence_ += escape(change.brush);
                    brush = change.brush;
                }
                escape_sequence_ += esc::detail::u32_to_u8(change.symbol);
                current_screen_[{x, y}] = change;
            }
        }
    }
    // Reset brush for consistency with above optimization.
    escape_sequence_ += escape(Brush{});

    changes.fill(Glyph{});

    set(esc::CursorMode::Hide);
    esc::write(escape_sequence_);

    if (cursor.has_value()) {
        set(esc::CursorMode::Show);
        esc::write(escape(esc::Cursor{*cursor}));
    }
    else {
        esc::set(esc::CursorMode::Hide);
    }

    esc::flush();
}

auto Terminal::run_read_loop(std::stop_token st) -> void
{
    Terminal::event_queue.enqueue(esc::Resize{Terminal::size()});

    while (!st.stop_requested()) {
        if (esc::sigint_flag == 1) {
            Terminal::event_queue.enqueue(event::Interrupt{});
            return;
        }
        else if (auto const event = esc::read(16); event.has_value()) {
            Terminal::event_queue.enqueue(
                std::visit([](auto const& e) -> Event { return e; }, *event));
            // ^^ Translate from esc::Event to ox::Event ^^
        }
    }
}

auto Terminal::size() -> Area { return esc::terminal_area(); }

// -------------------------------------------------------------------------------------

Timer::Timer(std::chrono::milliseconds duration, bool launch)
    : id_{next_id_++}, duration_{duration}
{
    Terminal::timers.emplace(id_, TimerThread{});
    if (launch) { this->start(); }
}

Timer::Timer(Timer&& other)
{
    id_ = std::move(other.id_);
    duration_ = std::move(other.duration_);
    is_running_ = std::move(other.is_running_);
    other.is_running_ = false;
}

auto Timer::operator=(Timer&& other) -> Timer&
{
    if (this->is_running_) { this->stop(); }
    id_ = std::move(other.id_);
    duration_ = std::move(other.duration_);
    is_running_ = std::move(other.is_running_);
    other.is_running_ = false;
    return *this;
}

Timer::~Timer()
{
    if (is_running_) { this->stop(); }
}

auto Timer::start() -> void
{
    // TODO fix Timer and TimerThread so the thread is reused.
    Terminal::timers[id_] = TimerThread{
        duration_, [id = id_] { Terminal::event_queue.enqueue(event::Timer{id}); }};
    is_running_ = true;
}

auto Timer::stop() -> void
{
    Terminal::timers[id_].request_stop();
    is_running_ = false;
}

// -------------------------------------------------------------------------------------

auto Canvas::operator[](Point p) -> Glyph&
{
    auto const global_point = Point{
        .x = std::clamp(at.x + p.x, 0, this->buffer.size().width - 1),
        .y = std::clamp(at.y + p.y, 0, this->buffer.size().height - 1),
    };
    return buffer[global_point];
}

auto Canvas::operator[](Point p) const -> Glyph const&
{
    auto const global_point = Point{
        .x = std::clamp(at.x + p.x, 0, buffer.size().width - 1),
        .y = std::clamp(at.y + p.y, 0, buffer.size().height - 1),
    };
    return buffer[global_point];
}

}  // namespace ox