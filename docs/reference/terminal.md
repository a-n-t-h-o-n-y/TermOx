# Terminal and input

[API reference](../reference.md) · [Guides](../README.md)

Terminal sessions, key and mouse events, and the event variant.

## On this page

- [ox::Terminal](#oxterminal)
- [Mouse Events](#mouse-events)
- [ox::KeyEvent](#oxkeyevent)
- [ox::Event](#oxevent)

## ox::Terminal

[`#include <ox/core/terminal.hpp>`](../../include/ox/core/terminal.hpp)

`class Terminal;`

Owns the terminal session and presents frames. Event-loop state belongs to
`EventLoop`.
Application opens a terminal in `run(Terminal::Options)` on the calling thread.

```cpp
auto t = Terminal{{
    .mouse_mode = MouseMode::Move,
    .foreground = RGB{0x773283},
    .background = RGB{0xFF8A4A},
}};
```

### Constructors

```cpp
struct Options {
    MouseMode mouse_mode = MouseMode::Basic;
    Signals signals = Signals::Off;
    Color foreground = default_color;
    Color background = default_color;
};

Terminal(Options options);

Terminal(MouseMode mouse_mode = MouseMode::Basic,
         Signals signals = Signals::Off,
         Color foreground_ = default_color,
         Color background_ = default_color);
```

Initialize the terminal screen to the 'alternate screen buffer' for interactive mode
with the given settings.

`MouseMode` determines which mouse events are generated:

```cpp
/**
 * Input modes for the Mouse.
 * @details Off: No Mouse Events are generated.
 * @details Basic: Mouse Press & Release Events for all Buttons + Scroll Wheel.
 * @details Drag: Basic, plus Move Events while a Button is pressed.
 * @details Move: Basic, plus Move Events sent with or without a Button pressed.
 */
enum class MouseMode { Off, Basic, Drag, Move };
```

`Signals` determines whether the OS generates signals from control keys. The default
is `Off`; TermOx handles Ctrl-C as an interrupt event and restores the terminal on exit.
With `On`, applications are responsible for OS signal handling:

```cpp
/**
 * `On` will generate signals from ctrl-[key] presses; `Off` will not.
 */
enum class Signals : bool { On, Off };
```

`foreground_` and `background_` determine the default colors for the Terminal.

```cpp
Terminal(Terminal const&) = delete;
Terminal(Terminal&&) = delete;
auto operator=(Terminal const&) -> Terminal& = delete;
auto operator=(Terminal&&) -> Terminal& = delete;
```

Terminal objects own an Escape session. Reading and rendering run on the calling thread.

### Destructor

Restores the session-owned terminal state.

### Reading and rendering

`read(timeout)` returns an optional TermOx event. `commit_changes(screen, cursor)`
compares a supplied screen with the previous frame and presents its changes.
`size()` returns terminal dimensions. Frame storage and repaint requests belong to
`EventLoop`, described in [the event loop guide](../event-loop.md).

## Mouse Events

TermOx uses Escape's `MousePress`, `MouseRelease`, `MouseMove`, and `MouseWheel`
types directly. Each event carries `at`, relative to the receiving widget, and
`modifiers`. Press events have a `MouseButton`; releases have an optional button,
and moves have an optional `held_button`. A missing release button means the
terminal did not identify the released button.

Wheel events use signed `dx` and `dy` steps: negative means left or up.

```cpp
auto mouse_press(MousePress m) -> EventResponse override
{
    if (m.button != MouseButton::Left) { return {}; }
    // Apply the click at m.at.
    return {.handled = true, .repaint = true};
}
```

## ox::KeyEvent

TermOx uses Escape's `KeyEvent` directly for `handle(KeyEvent const&)`.
`Key` is a variant of `char32_t`, `KeyCode`, and `esc::KittyKey`. An event has
an optional key identity, `Modifiers`, a `KeyAction`, and associated UTF-32 text.
An IME event can carry text without a key identity.

```cpp
auto handle(KeyEvent const& event) -> EventResponse override
{
    if (event.action == KeyAction::Release) { return {}; }
    if (event.matches(KeyCode::Enter, {}, event.action)) {
        // Accept the current selection.
        return {.handled = true, .repaint = true};
    }
    if (event.matches(U'q', {}, event.action)) { return {.handled = true, .quit = 0}; }
    return {};
}
```

Press, repeat, and release actions reach the same overload; inspect `event.action`.
`input_text(event)` returns printable text for editing, excluding releases and
control shortcuts. `handle(esc::PasteChunk const&)` receives bracketed paste chunks.
Text widgets buffer paste bytes until the final chunk so a UTF-8 scalar split
between chunks is decoded correctly. Aborted pastes are discarded.

## ox::Event

A `std::variant` of input event types. This is used by the core of the library and
direct access should not be needed by the typical user of this library.

```cpp
using Event = std::variant<esc::MousePress,
                           esc::MouseRelease,
                           esc::MouseWheel,
                           esc::MouseMove,
                           esc::KeyEvent,
                           esc::Focus,
                           esc::PasteChunk,
                           esc::UnknownInput,
                           esc::TerminalReply,
                           esc::Resize,
                           event::Custom,
                           event::Interrupt>;
```
