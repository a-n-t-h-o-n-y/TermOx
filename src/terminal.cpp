#include <termox/terminal.hpp>

#include <algorithm>

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
        if (st.stop_requested()) {
            return;
        }

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

auto Terminal::commit_changes() -> void
{
    escape_sequence_.clear();

    if (Terminal::changes.size() != current_screen_.size()) {
        current_screen_.resize(Terminal::changes.size());
        current_screen_.fill(Glyph{U'\0'});  // Trigger Repaint
    }

    auto brush = Brush{};

    for (auto x = 0; x < changes.size().width; ++x) {
        for (auto y = 0; y < changes.size().height; ++y) {
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
    Terminal::event_queue.append(esc::Resize{Terminal::size()});

    while (!st.stop_requested()) {
        if (esc::sigint_flag == 1) {
            Terminal::event_queue.append(event::Interrupt{});
            return;
        }
        else if (auto const event = esc::read(16); event.has_value()) {
            Terminal::event_queue.append(
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
    if (launch) {
        this->start();
    }
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
    if (this->is_running_) {
        this->stop();
    }
    id_ = std::move(other.id_);
    duration_ = std::move(other.duration_);
    is_running_ = std::move(other.is_running_);
    other.is_running_ = false;
    return *this;
}

Timer::~Timer()
{
    if (is_running_) {
        this->stop();
    }
}

auto Timer::start() -> void
{
    Terminal::timers[id_] = TimerThread{
        duration_, [id = id_] { Terminal::event_queue.append(event::Timer{id}); }};
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
    p.x = std::clamp(p.x, 0, size.width - 1);
    p.y = std::clamp(p.y, 0, size.height - 1);
    auto const global_point = Point{
        .x = std::clamp(at.x + p.x, 0, Terminal::changes.size().width - 1),
        .y = std::clamp(at.y + p.y, 0, Terminal::changes.size().height - 1),
    };
    return Terminal::changes[global_point];
}

auto Canvas::operator[](Point p) const -> Glyph const&
{
    p.x = std::clamp(p.x, 0, size.width - 1);
    p.y = std::clamp(p.y, 0, size.height - 1);
    auto const global_point = Point{
        .x = std::clamp(at.x + p.x, 0, Terminal::changes.size().width - 1),
        .y = std::clamp(at.y + p.y, 0, Terminal::changes.size().height - 1),
    };
    return Terminal::changes[global_point];
}

// -------------------------------------------------------------------------------------

Painter::CursorWriter::CursorWriter(Canvas const& canvas, Point cursor)
    : canvas_{canvas}, cursor_{cursor}
{}

auto Painter::CursorWriter::operator<<(Glyph const& g) && -> CursorWriter
{
    if (cursor_.x < canvas_.size.width) {
        canvas_[cursor_] = g;
        ++cursor_.x;
    }
    return std::move(*this);
}

auto Painter::CursorWriter::operator<<(Glyph const& g) & -> CursorWriter&
{
    if (cursor_.x < canvas_.size.width) {
        canvas_[cursor_] = g;
        ++cursor_.x;
    }
    return *this;
}

auto Painter::CursorWriter::operator<<(std::string_view sv) && -> CursorWriter
{
    return std::move(*this) << esc::detail::utf8_to_glyphs(sv);
}

auto Painter::CursorWriter::operator<<(std::string_view sv) & -> CursorWriter&
{
    return *this << esc::detail::utf8_to_glyphs(sv);
}

auto Painter::CursorWriter::operator<<(std::u32string_view sv) && -> CursorWriter
{
    return std::move(*this) << esc::detail::utf32_to_glyphs(sv);
}

auto Painter::CursorWriter::operator<<(std::u32string_view sv) & -> CursorWriter&
{
    return *this << esc::detail::utf32_to_glyphs(sv);
}

auto Painter::CursorWriter::operator<<(Painter::Box const& b) -> CursorWriter
{
    auto const end = Point{
        .x = std::min(cursor_.x + b.size.width, canvas_.size.width),
        .y = std::min(cursor_.y + b.size.height, canvas_.size.height),
    };

    // Top
    canvas_[cursor_] = b.corners[0] | b.brush;
    for (auto x = cursor_.x + 1; x < end.x - 1; ++x) {
        canvas_[{x, cursor_.y}] = b.walls[0] | b.brush;
    }
    canvas_[{end.x - 1, cursor_.y}] = b.corners[1] | b.brush;

    // Middle
    for (auto y = cursor_.y + 1; y < end.y - 1; ++y) {
        canvas_[{cursor_.x, y}] = b.walls[1] | b.brush;
        canvas_[{end.x - 1, y}] = b.walls[1] | b.brush;
    }

    // Bottom
    canvas_[{cursor_.x, end.y - 1}] = b.corners[2] | b.brush;
    for (auto x = cursor_.x + 1; x < end.x - 1; ++x) {
        canvas_[{x, end.y - 1}] = b.walls[0] | b.brush;
    }
    canvas_[{end.x - 1, end.y - 1}] = b.corners[3] | b.brush;

    return *this;
}

auto Painter::CursorWriter::operator<<(Painter::HLine const& hline) -> CursorWriter
{
    auto const end = std::min(cursor_.x + hline.length, canvas_.size.width);
    for (auto x = cursor_.x; x < end; ++x) {
        canvas_[{x, cursor_.y}] = hline.glyph;
    }
    return *this;
}

auto Painter::CursorWriter::operator<<(Painter::VLine const& vline) -> CursorWriter
{
    auto const end = std::min(cursor_.y + vline.length, canvas_.size.height);
    for (auto y = cursor_.y; y < end; ++y) {
        canvas_[{cursor_.x, y}] = vline.glyph;
    }
    return *this;
}

Painter::Painter(Canvas const& c) : canvas_{c} {}

auto Painter::operator[](Point p) -> CursorWriter { return CursorWriter{canvas_, p}; }

auto Painter::clear() -> void
{
    for (auto x = 0; x < canvas_.size.width; ++x) {
        for (auto y = 0; y < canvas_.size.height; ++y) {
            canvas_[{x, y}] = Glyph{};
        }
    }
}

auto Painter::fill(Glyph const& g) -> void
{
    for (auto x = 0; x < canvas_.size.width; ++x) {
        for (auto y = 0; y < canvas_.size.height; ++y) {
            canvas_[{x, y}] = g;
        }
    }
}

}  // namespace ox