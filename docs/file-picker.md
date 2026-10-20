# Choose a file or directory

`FilePicker` browses directories and validates a selection asynchronously.
Handle file I/O in the application.

```cpp
#include <ox/ox.hpp>

int main()
{
    auto picker = ox::FilePicker{};
    auto status = ox::Label{"Choose a file"};
    auto root = ox::Column{std::tie(picker, status)};
    ox::Connection{
        .signal = picker.on_choose,
        .slot = [](std::filesystem::path const& path, ox::Label& status) {
            status.set_text(path.string());
        },
    }(status);
    return ox::Application{root}.run();
}
```

For a modal chooser, put the picker inside a [Dialog](selection-and-application.md#root-overlays-popups-and-dialogs).
`on_choose` supplies a normalized absolute path; `on_cancel` asks the host to
cancel. Neither signal closes the surrounding dialog automatically.

## Choose a mode

Set `.mode` to a `FilePickerMode` value:

| Mode | Accepted selection |
| --- | --- |
| `OpenFile` | An existing regular file |
| `SaveFile` | An existing regular file or a new name in an existing directory |
| `Directory` | The selected directory, or the current directory if none is selected |

Save mode never creates or truncates a file. The application handles saving and
any overwrite confirmation. Symlink targets are resolved.

Enter in the location field navigates. Enter/double-click on a directory opens it;
activating a file or entering a filename chooses it. Up/Alt-Up goes to the parent;
F5 reloads. Directories sort first. Set `show_hidden` to include dot-prefixed names,
or supply a `filter` for files; directories remain available. Call `reload()` after
changing configuration.

## Handle completion and cancellation

`set_directory`, `reload`, and `accept` report whether work started, not whether it
succeeded. Observe `on_state` and `on_error` for completion. `is_loading()` covers
both directory loading and selection validation. Errors leave the previous model
visible, and the picker displays the error message.

`cancel_loading()` or Escape cancels pending work. A new request supersedes the old
one; cancelled and superseded results are never applied. Destruction and move
assignment cancel the old request, while moves retain active work. Cancellation
is cooperative between filesystem calls: a blocked call may continue until it
returns, so worker captures must remain valid until the worker exits.

## Worker callbacks

Widget mutations and signals run on the application thread. An attached timer
delivers results; detached or headless use must call `poll()` explicitly.

`filter` and `loader` run on workers. They must be thread-safe and must not access
widgets. A custom loader can supply another filesystem source or deterministic
test data; it must honor stop requests between operations and return absolute
paths. See [Background work](background-work.md) for thread and lifetime rules,
and the [API reference](https://a-n-t-h-o-n-y.github.io/TermOx/) for loader types.
