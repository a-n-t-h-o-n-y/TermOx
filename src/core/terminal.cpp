#include <ox/core/terminal.hpp>

#include <algorithm>
#include <cassert>

#include <esc/io.hpp>
#include <esc/sequence.hpp>
#include <esc/terminal.hpp>
#include <esc/text.hpp>
#include <exception>

namespace ox {

ScreenBuffer::ScreenBuffer(Area size)
    : size_{size}, buffer_((std::size_t)(size.width * size.height))
{}

auto ScreenBuffer::operator[](Point p) -> Glyph&
{
    auto const at = (std::size_t)(p.y * size_.width + p.x);
    assert(at < buffer_.size());
    return buffer_[at];
}

auto ScreenBuffer::operator[](Point p) const -> Glyph const&
{
    auto const at = (std::size_t)(p.y * size_.width + p.x);
    assert(at < buffer_.size());
    return buffer_[at];
}

void ScreenBuffer::resize(Area a)
{
    size_ = a;
    buffer_.resize((std::size_t)(a.width * a.height));
}

void ScreenBuffer::fill(Glyph const& g)
{
    for (auto& glyph : buffer_) {
        glyph = g;
    }
}

// -------------------------------------------------------------------------------------

Terminal::Terminal(Options x)
    : foreground{x.foreground},
      background{x.background},
      session_{esc::Terminal::open([&] {
          auto options = esc::TerminalOptions::interactive();
          options.mouse = x.mouse_mode;
          options.signals = x.signals;
          options.paste = esc::BracketedPaste::On;
          return options;
      }())},
      terminal_input_thread_{[this](auto st) { this->run_read_loop(st); }}
{}

Terminal::Terminal(MouseMode mouse_mode,
                   Signals signals,
                   Color foreground_,
                   Color background_)
    : Terminal{Options{.mouse_mode = mouse_mode,
                       .signals = signals,
                       .foreground = foreground_,
                       .background = background_}}
{}

Terminal::~Terminal()
{
    terminal_input_thread_.request_stop();
    terminal_input_thread_.join();
}

void Terminal::commit_changes()
{
    auto const lock = std::scoped_lock{session_mutex_};
    escape_sequence_.clear();

    if (this->changes.size() != current_screen_.size()) {
        current_screen_.resize(this->changes.size());
        current_screen_.fill(Glyph{U'\0'});  // Trigger Repaint
    }

    auto style = Style{};

    for (auto y = 0; y < this->changes.size().height; ++y) {
        for (auto x = 0; x < this->changes.size().width; ++x) {
            auto const change = [&] {
                auto g = this->changes[{x, y}];
                if (g.style.background == Color{default_color}) {
                    g.style.background = this->background;
                }
                if (g.style.foreground == Color{default_color}) {
                    g.style.foreground = this->foreground;
                }
                return g;
            }();
            auto const& current = current_screen_[{x, y}];
            if (change != current) {
                escape_sequence_ += esc::encode(session_.profile(), esc::Cursor{x, y});
                if (change.style != style) {
                    escape_sequence_ += esc::encode(session_.profile(), change.style);
                    style = change.style;
                }
                escape_sequence_ += esc::encode_utf8(change.symbol).view();
                current_screen_[{x, y}] = change;
            }
        }
    }
    // Reset style for consistency with above optimization.
    escape_sequence_ += esc::encode(session_.profile(), Style{});

    this->changes.fill(Glyph{});

    auto frame = session_.frame();
    session_.set(esc::CursorMode::Hide);
    session_.write(escape_sequence_);

    if (cursor.has_value()) {
        session_.write(esc::Cursor{cursor->x, cursor->y});
        session_.set(esc::CursorMode::Show);
    }
    frame.close();
}

void Terminal::run_read_loop(std::stop_token st)
{
    try {
        Terminal::event_queue.enqueue(esc::Resize{this->size()});
        while (!st.stop_requested()) {
            auto input = [&] {
                auto const lock = std::scoped_lock{session_mutex_};
                return session_.read(std::chrono::milliseconds{16});
            }();
            if (auto event = std::get_if<esc::Event>(&input)) {
                if (auto key = std::get_if<KeyEvent>(event);
                    key &&
                    (key->matches(U'c', {.ctrl = true}) || key->matches(U'\x03'))) {
                    Terminal::event_queue.enqueue(event::Interrupt{});
                    return;
                }
                Terminal::event_queue.enqueue(std::visit(
                    [](auto&& e) -> Event { return std::move(e); }, std::move(*event)));
            }
            else if (std::holds_alternative<esc::EndOfInput>(input)) {
                Terminal::event_queue.enqueue(
                    event::Custom{[] { return QuitRequest{}; }});
                return;
            }
        }
    }
    catch (...) {
        Terminal::event_queue.enqueue(
            event::Custom{[error = std::current_exception()]() -> EventResponse {
                std::rethrow_exception(error);
            }});
    }
}

auto Terminal::size() -> Area
{
    auto const lock = std::scoped_lock{session_mutex_};
    return session_.area();
}

// -------------------------------------------------------------------------------------

auto Canvas::operator[](Point p) -> Glyph&
{ return buffer[{.x = at.x + p.x, .y = at.y + p.y}]; }

auto Canvas::operator[](Point p) const -> Glyph const&
{ return buffer[{.x = at.x + p.x, .y = at.y + p.y}]; }

}  // namespace ox