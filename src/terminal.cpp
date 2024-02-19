#include <termox/terminal.hpp>

#include <esc/detail/signals.hpp>
#include <esc/detail/transcode.hpp>
#include <esc/io.hpp>
#include <esc/sequence.hpp>
#include <esc/terminal.hpp>

namespace ox {

ScreenBuffer::ScreenBuffer(Area area)
    : area_{area}, buffer_((std::size_t)(area.width * area.height))
{}

auto ScreenBuffer::operator[](Point p) -> Glyph&
{
    return buffer_[(std::size_t)(p.y * area_.width + p.x)];
}

auto ScreenBuffer::operator[](Point p) const -> Glyph const&
{
    return buffer_[(std::size_t)(p.y * area_.width + p.x)];
}

auto ScreenBuffer::reset(Area a) -> void
{
    area_ = a;
    buffer_.resize((std::size_t)(a.width * a.height));
    this->clear();
}

auto ScreenBuffer::clear() -> void
{
    for (auto& g : buffer_) {
        g.symbol = U'\0';
    }
}

// -----------------------------------------------------------------------------

TimerThread::TimerThread(std::chrono::milliseconds duration,
                         CallbackType callback)
    : thread_{[cb = std::move(callback), d = std::move(duration)](auto st) {
          TimerThread::run(st, cb, d);
      }}
{}

auto TimerThread::run(std::stop_token st,
                      CallbackType const& callback,
                      std::chrono::milliseconds duration) -> void
{
    constexpr auto timeout_duration =
        std::chrono::duration_cast<ClockType::duration>(
            std::chrono::milliseconds{16});
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

        auto const time_to_wait =
            std::min(next_callback_time - now, timeout_duration);
        std::this_thread::sleep_for(time_to_wait);
    }
}

// -----------------------------------------------------------------------------

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

    if (Terminal::changes.area() != current_screen_.area()) {
        current_screen_.reset(Terminal::changes.area());
    }

    for (auto x = 0; x < changes.area().width; ++x) {
        for (auto y = 0; y < changes.area().height; ++y) {
            // TODO optimize by only writing brush changes if the brush is
            // different from the previous brush. Hold a fn local brush for the
            // comparison.
            auto const& change  = changes[{x, y}];
            auto const& current = current_screen_[{x, y}];
            if (change.symbol != U'\0' && change != current) {
                escape_sequence_ += escape(esc::Cursor{x, y});
                escape_sequence_ += escape(change.brush);
                escape_sequence_ += esc::detail::u32_to_u8(change.symbol);
                current_screen_[{x, y}] = change;
            }
        }
    }

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
    Terminal::event_queue.append(esc::Resize{Terminal::area()});

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

auto Terminal::area() -> Area { return esc::terminal_area(); }

// -----------------------------------------------------------------------------

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
    id_               = std::move(other.id_);
    duration_         = std::move(other.duration_);
    is_running_       = std::move(other.is_running_);
    other.is_running_ = false;
}

auto Timer::operator=(Timer&& other) -> Timer&
{
    if (this->is_running_) {
        this->stop();
    }
    id_               = std::move(other.id_);
    duration_         = std::move(other.duration_);
    is_running_       = std::move(other.is_running_);
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
        duration_,
        [id = id_] { Terminal::event_queue.append(event::Timer{id}); }};
    is_running_ = true;
}

auto Timer::stop() -> void
{
    Terminal::timers[id_].request_stop();
    is_running_ = false;
}

// -----------------------------------------------------------------------------

Painter::CursorWriter::CursorWriter(Point at, Area bounds, ScreenBuffer& buffer)
    : at_{.x = std::max(0, at.x), .y = std::max(0, at.y)},
      bounds_{.width  = std::max(0, bounds.width),
              .height = std::max(0, bounds.height)},
      buffer_{buffer}
{}

auto Painter::CursorWriter::operator<<(Glyph const& g) && -> CursorWriter
{
    if (at_.x < bounds_.width && at_.y < bounds_.height) {
        buffer_[at_] = g;
        ++at_.x;
    }
    return std::move(*this);
}

auto Painter::CursorWriter::operator<<(Glyph const& g) & -> CursorWriter&
{
    if (at_.x < bounds_.width && at_.y < bounds_.height) {
        buffer_[at_] = g;
        ++at_.x;
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

auto Painter::CursorWriter::operator<<(
    std::u32string_view sv) && -> CursorWriter
{
    return std::move(*this) << esc::detail::utf32_to_glyphs(sv);
}

auto Painter::CursorWriter::operator<<(
    std::u32string_view sv) & -> CursorWriter&
{
    return *this << esc::detail::utf32_to_glyphs(sv);
}

auto Painter::CursorWriter::operator<<(Painter::Box const& b) -> CursorWriter
{
    auto const end = Point{
        .x = std::min(at_.x + b.area.width, bounds_.width),
        .y = std::min(at_.y + b.area.height, bounds_.height),
    };

    // Top
    buffer_[at_] = U'┌' | b.brush;
    for (auto x = at_.x + 1; x < end.x - 1; ++x) {
        buffer_[{x, at_.y}] = U'─' | b.brush;
    }
    buffer_[{end.x - 1, at_.y}] = U'┐' | b.brush;

    // Middle
    for (auto y = at_.y + 1; y < end.y - 1; ++y) {
        buffer_[{at_.x, y}]     = U'│' | b.brush;
        buffer_[{end.x - 1, y}] = U'│' | b.brush;
    }

    // Bottom
    buffer_[{at_.x, end.y - 1}] = U'└' | b.brush;
    for (auto x = at_.x + 1; x < end.x - 1; ++x) {
        buffer_[{x, end.y - 1}] = U'─' | b.brush;
    }
    buffer_[{end.x - 1, end.y - 1}] = U'┘' | b.brush;

    return *this;
}

auto Painter::CursorWriter::operator<<(Painter::RoundedBox const& rb)
    -> CursorWriter
{
    auto const end = Point{
        .x = std::min(at_.x + rb.area.width, bounds_.width),
        .y = std::min(at_.y + rb.area.height, bounds_.height),
    };

    // Top
    buffer_[at_] = U'╭' | rb.brush;
    for (auto x = at_.x + 1; x < end.x - 1; ++x) {
        buffer_[{x, at_.y}] = U'─' | rb.brush;
    }
    buffer_[{end.x - 1, at_.y}] = U'╮' | rb.brush;

    // Middle
    for (auto y = at_.y + 1; y < end.y - 1; ++y) {
        buffer_[{at_.x, y}]     = U'│' | rb.brush;
        buffer_[{end.x - 1, y}] = U'│' | rb.brush;
    }

    // Bottom
    buffer_[{at_.x, end.y - 1}] = U'╰' | rb.brush;
    for (auto x = at_.x + 1; x < end.x - 1; ++x) {
        buffer_[{x, end.y - 1}] = U'─' | rb.brush;
    }
    buffer_[{end.x - 1, end.y - 1}] = U'╯' | rb.brush;

    return *this;
}

auto Painter::CursorWriter::operator<<(Painter::HLine const& hline)
    -> CursorWriter
{
    auto const end = std::min(at_.x + hline.length, bounds_.width);
    for (auto x = at_.x; x < end; ++x) {
        buffer_[{x, at_.y}] = hline.glyph;
    }
    return *this;
}

auto Painter::CursorWriter::operator<<(Painter::VLine const& vline)
    -> CursorWriter
{
    auto const end = std::min(at_.y + vline.length, bounds_.height);
    for (auto y = at_.y; y < end; ++y) {
        buffer_[{at_.x, y}] = vline.glyph;
    }
    return *this;
}

// -----------------------------------------------------------------------------

Painter::Painter()
    : offset_{0, 0}, size_{Terminal::changes.area()}, screen_{Terminal::changes}
{
    this->fill(Glyph{' '});
}

auto Painter::operator[](Point p) -> CursorWriter
{
    return CursorWriter{
        {
            .x = p.x + offset_.x,
            .y = p.y + offset_.y,
        },
        size_,
        screen_,
    };
}

auto Painter::clear() -> void
{
    for (auto x = offset_.x; x < offset_.x + size_.width; ++x) {
        for (auto y = offset_.y; y < offset_.y + size_.height; ++y) {
            screen_[{x, y}].symbol = U'\0';
        }
    }
}

auto Painter::fill(Glyph const& g) -> void
{
    for (auto x = offset_.x; x < offset_.x + size_.width; ++x) {
        for (auto y = offset_.y; y < offset_.y + size_.height; ++y) {
            screen_[{x, y}] = g;
        }
    }
}

}  // namespace ox