# Update the UI from background work

Only `Application::post` is thread-safe. Workers should produce owned data and post
an `event::Custom` action that updates widgets on the application thread. Do not call
widget methods, emit their signals, or mutate their public fields from a worker.

## Post an owned result

This program reads a text file on a worker and displays it in a read-only editor.
Pass a path as the first argument; it otherwise tries `README.md` in the working
directory. Close exits the app and requests cancellation of the file-reading loop.

```cpp
#include <ox/ox.hpp>
#include <fstream>
#include <thread>

int main(int argc, char** argv)
{
    auto path = std::string{argc > 1 ? argv[1] : "README.md"};
    auto text = ox::TextBox{{.placeholder = "Reading…", .read_only = true}};
    auto close = ox::Button{{.label = {.text = "Close"}}};
    auto root = ox::Column{std::tie(text, close)};
    auto app = ox::Application{root};
    ox::Connection{.signal = close.on_activate, .slot = [&app] { app.quit(); }}();

    auto worker = std::jthread{[&app, target = ox::track(text), path](std::stop_token stop) {
        auto input = std::ifstream{path};
        auto result = std::string{};
        if (!input) result = "Cannot open " + path;
        for (std::string line; !stop.stop_requested() && std::getline(input, line);)
            result += line + '\n';
        if (stop.stop_requested()) return;
        app.post(ox::event::Custom{.action = [target, result = std::move(result)] {
            if (auto* text = target.try_get())
                text->set_text(result.empty() ? "(empty file)" : result);
            return ox::EventResponse{};
        }});
    }};
    auto code = app.run();
    worker.request_stop();
    return code;
}
```

The posted callback owns the result string. Its tracked view follows widget moves
and skips an editor that has been destroyed. `set_text` schedules its own layout and
paint, so the callback needs no separate repaint effect.

## Worker and application lifetime

The worker captures `app` by reference. It is declared after the application, so its
`jthread` destructor requests stop and joins before the application is destroyed.
Keep that ordering in a larger application too: every producer must stop posting
before the event loop it borrows goes away. Cancellation here is checked between
lines; it does not interrupt a blocked system read.

Posted work wakes an idle event loop. Posts made during a dispatch batch are delivered
on the next step. A callback runs on the application thread and should finish quickly;
put further blocking work back on the worker. See [event-loop internals](internals/event-loop.md)
for queue transfer and waiting behavior.

## Use FilePicker for directory browsing

`FilePicker` already owns asynchronous directory listing and path validation. Its
`filter` and `loader` run on the worker, so they must not capture widgets. Results,
`on_state`, `on_error`, and `on_choose` are delivered on the application thread.
See [File selection](file-picker.md) for those callbacks and
how pending work is cancelled.
