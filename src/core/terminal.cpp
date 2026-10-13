#include <ox/core/terminal.hpp>

#include <algorithm>
#include <cassert>

#include <esc/io.hpp>
#include <esc/sequence.hpp>
#include <esc/terminal.hpp>
#include <esc/text.hpp>
#include <exception>

namespace ox {

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

void Terminal::commit_changes(Frame const& screen, Cursor cursor)
{
    escape_sequence_.clear();
    auto const repaint = !screen_valid_ || screen.size() != current_screen_.size() ||
                         foreground != last_foreground_ ||
                         background != last_background_;
    auto style = Style{};
    for (auto y = 0; y < screen.size().height; ++y) {
        for (auto x = 0; x < screen.size().width; ++x) {
            auto const& cell = screen[{x, y}];
            if (cell.columns == 0) { continue; }
            if (repaint || cell != current_screen_[{x, y}]) {
                auto appearance = cell.style;
                if (appearance.background == Color{default_color}) {
                    appearance.background = background;
                }
                if (appearance.foreground == Color{default_color}) {
                    appearance.foreground = foreground;
                }
                escape_sequence_ += esc::encode(session_.profile(), esc::Cursor{x, y});
                if (appearance != style) {
                    escape_sequence_ += esc::encode(session_.profile(), appearance);
                    style = appearance;
                }
                esc::append_utf8(escape_sequence_, cell.text);
            }
        }
    }
    escape_sequence_ += esc::encode(session_.profile(), Style{});
    auto frame = session_.frame();
    session_.set(esc::CursorMode::Hide);
    session_.write(escape_sequence_);
    if (cursor && contains(Rect{{0, 0}, screen.size()}, *cursor)) {
        auto const lead = screen.lead_at(*cursor);
        session_.write(esc::Cursor{lead.x, lead.y});
        session_.set(esc::CursorMode::Show);
    }
    frame.close();
    current_screen_ = screen;
    screen_valid_ = true;
    last_foreground_ = foreground;
    last_background_ = background;
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

}  // namespace ox
