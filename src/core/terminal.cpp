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
      }())}
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

void Terminal::commit_changes(ScreenBuffer const& screen, Cursor cursor)
{
    escape_sequence_.clear();

    if (screen.size() != current_screen_.size()) {
        current_screen_.resize(screen.size());
        current_screen_.fill(Glyph{U'\0'});  // Trigger Repaint
    }

    auto style = Style{};

    for (auto y = 0; y < screen.size().height; ++y) {
        for (auto x = 0; x < screen.size().width; ++x) {
            auto const change = [&] {
                auto g = screen[{x, y}];
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

    auto frame = session_.frame();
    session_.set(esc::CursorMode::Hide);
    session_.write(escape_sequence_);

    if (cursor.has_value()) {
        session_.write(esc::Cursor{cursor->x, cursor->y});
        session_.set(esc::CursorMode::Show);
    }
    frame.close();
}

auto Terminal::read(std::chrono::milliseconds timeout) -> std::optional<Event>
{
    auto input = session_.read(timeout);
    if (auto event = std::get_if<esc::Event>(&input)) {
        if (auto key = std::get_if<KeyEvent>(event);
            key && (key->matches(U'c', {.ctrl = true}) || key->matches(U'\x03'))) {
            return event::Interrupt{};
        }
        return std::visit([](auto&& e) -> Event { return std::move(e); },
                          std::move(*event));
    }
    if (std::holds_alternative<esc::EndOfInput>(input)) {
        return event::Custom{[] { return EventResponse{.handled = true, .quit = 0}; }};
    }
    return {};
}

auto Terminal::size() -> Area { return session_.area(); }

// -------------------------------------------------------------------------------------

auto Canvas::operator[](Point p) -> Glyph&
{ return buffer[{.x = at.x + p.x, .y = at.y + p.y}]; }

auto Canvas::operator[](Point p) const -> Glyph const&
{ return buffer[{.x = at.x + p.x, .y = at.y + p.y}]; }

}  // namespace ox