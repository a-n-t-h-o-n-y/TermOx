#include <ox/core/terminal.hpp>

#include <esc/terminal.hpp>

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
      }())},
      renderer_{session_.profile()}
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

Terminal::~Terminal() noexcept
{
    // Style is retained across frames, but must not leak into the restored screen.
    try {
        session_.write(Style{});
        session_.flush();
    }
    catch (...) {
        // Session destruction still attempts mode restoration after an I/O failure.
    }
}

void Terminal::commit_changes(Frame const& screen, Cursor cursor)
{
    try {
        auto const output = renderer_.render(screen, cursor, foreground, background);
        auto const visible = cursor && contains(Rect{{0, 0}, screen.size()}, *cursor);
        auto set_cursor = [&](bool show) {
            if (cursor_visible_ == show) { return; }
            session_.set(show ? esc::CursorMode::Show : esc::CursorMode::Hide);
            cursor_visible_ = show;
        };
        // Only change visibility when needed; Escape owns its restoration.
        // Skip synchronized frames and flushes entirely for unchanged output.
        if (output.empty()) {
            set_cursor(visible);
            return;
        }
        auto frame = session_.frame();
        set_cursor(false);
        session_.write(output);
        set_cursor(visible);
        frame.close();
    }
    catch (...) {
        renderer_.invalidate();
        throw;
    }
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
