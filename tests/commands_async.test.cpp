#include <atomic>
#include <condition_variable>
#include <fstream>
#include <mutex>
#include <ox/ox.hpp>
#include <thread>
#include <zzz/test.hpp>
using namespace ox;
namespace {
template <typename Predicate>
void until(Predicate ready)
{
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{5};
    while (!ready()) {
        if (std::chrono::steady_clock::now() > deadline) {
            throw std::runtime_error{"Async test timeout"};
        }
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
}
void complete(FilePicker& picker)
{
    auto lifetime = track(picker);
    until([&] {
        auto* self = lifetime.try_get();
        if (!self) { return true; }
        self->poll();
        self = lifetime.try_get();
        return !self || !self->is_loading();
    });
}
struct ScanState {
    std::mutex mutex;
    std::condition_variable cv;
    bool entered = false, released = false, done = false;
    std::stop_token stop;
};
struct BlockedScan {
    std::shared_ptr<ScanState> state = std::make_shared<ScanState>();
    auto loader()
    {
        return
            [state = state](std::filesystem::path const& path, std::stop_token stop) {
                if (path.filename() == "slow") {
                    auto lock = std::unique_lock{state->mutex};
                    state->entered = true;
                    state->stop = stop;
                    state->cv.notify_all();
                    state->cv.wait(lock, [&] { return state->released; });
                    state->done = true;
                    state->cv.notify_all();
                }
                return DirectoryListing{.directory = path, .entries = {}, .error = {}};
            };
    }
    void entered()
    {
        auto lock = std::unique_lock{state->mutex};
        if (!state->cv.wait_for(lock, std::chrono::seconds{5},
                                [&] { return state->entered; })) {
            throw std::runtime_error{"Worker did not start"};
        }
    }
    void release()
    {
        auto lock = std::scoped_lock{state->mutex};
        state->released = true;
        state->cv.notify_all();
    }
    void done()
    {
        auto lock = std::unique_lock{state->mutex};
        if (!state->cv.wait_for(lock, std::chrono::seconds{5},
                                [&] { return state->done; })) {
            throw std::runtime_error{"Worker did not finish"};
        }
    }
    ~BlockedScan() { release(); }
};
}  // namespace
TEST(shared_commands_drive_menu_palette_help_keys_and_enabled_state)
{
    int calls = 0;
    auto save = command({.text = "Save",
                         .keywords = "write",
                         .action =
                             [&] {
                                 ++calls;
                                 return EventResponse{.repaint = true};
                             },
                         .keys = {{KeyCode::F5, {}}},
                         .checked = true});
    auto menu = Menu{{{.command = save}}};
    auto palette = CommandPalette{{save}};
    auto help = KeyHelp{{save}};
    save->text = "Write";
    save->keys = {{KeyCode::F6, {}}};
    save->enabled = false;
    save->checked = false;
    menu.refresh();
    palette.refresh();
    help.refresh();
    ASSERT(menu.items()[0].text.find("Write") != std::string::npos);
    ASSERT(menu.items()[0].text.find("f6") != std::string::npos);
    ASSERT(!menu.items()[0].enabled && !palette.choices().items()[0].enabled);
    ASSERT(!help.handle(KeyEvent{.key = KeyCode::F6}).handled);
    ASSERT(!menu.handle_command(KeyEvent{.key = KeyCode::F6}).handled);
    ASSERT(!palette.execute(0));
    save->enabled = true;
    menu.refresh();
    palette.refresh();
    ASSERT(!help.handle(KeyEvent{.key = KeyCode::F5}).handled);
    ASSERT(help.handle(KeyEvent{.key = KeyCode::F6}).repaint);
    ASSERT(menu.handle_command(KeyEvent{.key = KeyCode::F6}).repaint);
    ASSERT(palette.execute(0));
    ASSERT(calls == 3);
    ASSERT(!dispatch_commands(help.bindings,
                              KeyEvent{.key = KeyCode::F6, .action = KeyAction::Repeat})
                .handled);
    ASSERT(!dispatch_commands(help.bindings, KeyEvent{.key = KeyCode::F6,
                                                      .action = KeyAction::Release})
                .handled);
}
TEST(menu_shortcuts_work_closed_and_open_and_propagate_quit_effects)
{
    int calls = 0;
    auto run = command({.text = "Run",
                        .action =
                            [&] {
                                ++calls;
                                return EventResponse{.repaint = true};
                            },
                        .keys = {{U'r', {.ctrl = true}}}});
    auto quit = command({.text = "Quit",
                         .action = [] { return EventResponse{.quit = 7}; },
                         .keys = {{KeyCode::F8, {}}}});
    auto bar = MenuBar{{{"File",
                         {{.command = command({.text = "More"}),
                           .children = {{.command = run}, {.command = quit}}}}}}};
    auto app = Application{bar};
    app.dispatch(esc::Resize{{40, 10}});
    auto paths = std::vector<std::vector<std::size_t>>{};
    bar.on_trigger.connect([&](auto const& p) { paths.push_back(p); });
    app.dispatch(KeyEvent{.key = U'r', .modifiers = {.ctrl = true}});
    ASSERT(calls == 1);
    ASSERT((paths.back() == std::vector<std::size_t>{0, 0, 0}));
    bar.open(0);
    app.step();
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    app.dispatch(KeyEvent{.key = U'r', .modifiers = {.ctrl = true}});
    ASSERT(calls == 2);
    ASSERT(!bar.menu(0).visible || !app.is_interactive(bar.menu(0)));
    app.dispatch(KeyEvent{.key = KeyCode::F8});
    ASSERT(app.loop().exit_code() == 7);
}
TEST(command_effects_survive_palette_or_nested_menu_destruction)
{
    auto quit =
        command({.text = "Quit", .action = [] { return EventResponse{.quit = 4}; }});
    auto palette = std::make_unique<CommandPalette>(std::vector{quit});
    auto app = Application{*palette};
    palette->on_execute.connect([&](auto) { palette.reset(); });
    palette->execute(0);
    ASSERT(!palette && app.loop().exit_code() == 4);
    auto menu = std::make_unique<Menu>(std::vector<MenuItem>{{.command = quit}});
    auto menus = Application{*menu};
    menu->on_trigger.connect([&](auto const&) { menu.reset(); });
    menu->activate(0);
    ASSERT(!menu && menus.loop().exit_code() == 4);
    auto loop = std::make_unique<EventLoop>();
    auto respond = loop->effect_handler();
    loop.reset();
    respond({.quit = 1});
}
TEST(command_dispatch_actions_may_replace_their_bindings)
{
    auto values = std::vector<std::shared_ptr<Command>>{};
    values.push_back(command({.text = "Clear",
                              .action =
                                  [&] {
                                      values.clear();
                                      return EventResponse{.quit = 3};
                                  },
                              .keys = {{KeyCode::F1, {}}}}));
    auto result = dispatch_commands(values, KeyEvent{.key = KeyCode::F1});
    ASSERT(values.empty() && result.handled && result.quit == 3);
}
TEST(file_loading_cancels_without_waiting_and_ignores_late_results)
{
    auto blocked = BlockedScan{};
    auto picker = FilePicker{{.directory = "/slow", .loader = blocked.loader()}};
    blocked.entered();
    ASSERT(picker.is_loading());
    ASSERT(!picker.accept());
    ASSERT(picker.cancel_loading());
    ASSERT(picker.state() == FilePickerState::Cancelled);
    ASSERT(blocked.state->stop.stop_requested());
    ASSERT(picker.set_directory("/new"));
    complete(picker);
    ASSERT(picker.directory() == "/new");
    blocked.release();
    blocked.done();
    picker.poll();
    ASSERT(picker.directory() == "/new");
}
TEST(file_loader_keeps_application_input_responsive_and_delivers_on_ui_thread)
{
    auto blocked = BlockedScan{};
    auto root = Column{FilePicker{{.directory = "/slow", .loader = blocked.loader()}},
                       Button{}};
    auto& picker = std::get<0>(root.children);
    auto& button = std::get<1>(root.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{40, 10}});
    blocked.entered();
    int actions = 0;
    button.on_activate.connect([&] { ++actions; });
    app.set_focus(button);
    app.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(actions == 1);
    auto ui = std::this_thread::get_id();
    bool ready = false;
    picker.on_state.connect([&](auto state) {
        ASSERT(std::this_thread::get_id() == ui);
        if (state == FilePickerState::Ready) { ready = true; }
    });
    blocked.release();
    until([&] {
        app.step(EventLoop::Clock::now() + std::chrono::milliseconds{20});
        return ready;
    });
    ASSERT(picker.directory() == "/slow");
}
TEST(file_loading_follows_moves_and_destruction_requests_stop)
{
    auto blocked = BlockedScan{};
    auto source = std::make_unique<FilePicker>(
        FilePicker::Options{.directory = "/slow", .loader = blocked.loader()});
    blocked.entered();
    auto destination = std::make_unique<FilePicker>(std::move(*source));
    source.reset();
    ASSERT(!blocked.state->stop.stop_requested());
    blocked.release();
    complete(*destination);
    ASSERT(destination->directory() == "/slow");
    auto second = BlockedScan{};
    auto owned = std::make_unique<FilePicker>(
        FilePicker::Options{.directory = "/slow", .loader = second.loader()});
    second.entered();
    owned.reset();
    ASSERT(second.state->stop.stop_requested());
    second.release();
    second.done();
}
TEST(file_move_assignment_cancels_the_previous_load)
{
    auto blocked = BlockedScan{};
    auto old = FilePicker{{.directory = "/slow", .loader = blocked.loader()}};
    blocked.entered();
    auto replacement = FilePicker{{.directory = "/new", .loader = blocked.loader()}};
    complete(replacement);
    old = std::move(replacement);
    ASSERT(blocked.state->stop.stop_requested());
    blocked.release();
    blocked.done();
    old.poll();
    ASSERT(old.directory() == "/new");
}
TEST(file_loading_state_callbacks_can_replace_requests_or_destroy_the_picker)
{
    auto read = [](auto const& path, std::stop_token) {
        return DirectoryListing{.directory = path, .entries = {}, .error = {}};
    };
    auto picker = FilePicker{{.directory = "/old", .loader = read}};
    picker.on_state.connect([&](auto state) {
        if (state == FilePickerState::Ready && picker.directory() == "/old") {
            picker.set_directory("/new");
        }
    });
    complete(picker);
    ASSERT(picker.directory() == "/new");
    auto owned = std::make_unique<FilePicker>(
        FilePicker::Options{.directory = "/new", .loader = read});
    owned->on_state.connect([&](auto state) {
        if (state == FilePickerState::Ready) { owned.reset(); }
    });
    auto lifetime = track(*owned);
    until([&] {
        if (auto* self = lifetime.try_get()) { self->poll(); }
        return !lifetime.valid();
    });
    ASSERT(!owned);
}
TEST(file_worker_errors_are_reported_on_ui_and_preserve_the_loaded_model)
{
    auto read = [](auto const& path, std::stop_token) {
        if (path.filename() == "bad") { throw std::runtime_error{"Scan failed"}; }
        return DirectoryListing{.directory = path, .entries = {}, .error = {}};
    };
    auto picker = FilePicker{{.directory = "/good", .loader = read}};
    complete(picker);
    auto ui = std::this_thread::get_id();
    int errors = 0;
    picker.on_error.connect([&](auto const& message) {
        ASSERT(std::this_thread::get_id() == ui);
        ASSERT(message == "Scan failed");
        ++errors;
    });
    picker.set_directory("/bad");
    complete(picker);
    ASSERT(picker.state() == FilePickerState::Error && errors == 1);
    ASSERT(picker.directory() == "/good");
}

TEST(file_loading_escape_cancels_without_losing_focus_or_input)
{
    auto blocked = BlockedScan{};
    auto picker = FilePicker{{.directory = "/slow", .loader = blocked.loader()}};
    auto app = Application{picker};
    app.dispatch(esc::Resize{{30, 8}});
    blocked.entered();
    app.set_focus(picker.location);
    app.dispatch(KeyEvent{.key = KeyCode::Escape});
    ASSERT(picker.state() == FilePickerState::Cancelled);
    ASSERT(picker.location.is_focused());
    ASSERT(blocked.state->stop.stop_requested());
    blocked.release();
    blocked.done();
}
TEST(file_validation_can_be_cancelled_without_emitting_choose)
{
    auto root = std::filesystem::temp_directory_path();
    auto read = [root](auto const&, std::stop_token) {
        return DirectoryListing{.directory = root, .entries = {}, .error = {}};
    };
    auto picker = FilePicker{
        {.directory = root, .mode = FilePickerMode::Directory, .loader = read}};
    complete(picker);
    int choices = 0;
    picker.on_choose.connect([&](auto const&) { ++choices; });
    ASSERT(picker.accept());
    ASSERT(picker.state() == FilePickerState::Validating);
    ASSERT(picker.cancel_loading());
    picker.poll();
    ASSERT(choices == 0);
    ASSERT(picker.state() == FilePickerState::Cancelled);
}
TEST(file_filter_runs_on_worker_and_ready_notifications_can_supersede_results)
{
    auto root = std::filesystem::temp_directory_path();
    auto filename =
        root /
        ("termox-async-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    struct Remove {
        std::filesystem::path path;
        ~Remove()
        {
            std::error_code ec;
            std::filesystem::remove(path, ec);
        }
    } cleanup{filename};
    std::ofstream{filename} << "test";
    auto ui = std::this_thread::get_id();
    std::atomic<bool> worker{false};
    auto read = [filename, root](auto const&, std::stop_token) {
        return DirectoryListing{.directory = root,
                                .entries = {std::filesystem::directory_entry{filename}},
                                .error = {}};
    };
    auto picker = FilePicker{{.directory = root,
                              .filter =
                                  [&](auto const&) {
                                      worker = std::this_thread::get_id() != ui;
                                      return true;
                                  },
                              .loader = read}};
    complete(picker);
    ASSERT(worker && picker.paths().size() == 1);
    int choices = 0;
    picker.on_choose.connect([&](auto const&) { ++choices; });
    bool replace = true;
    picker.on_state.connect([&](auto state) {
        if (state == FilePickerState::Ready && std::exchange(replace, false)) {
            picker.reload();
        }
    });
    picker.filename.set_text(filename.filename().string());
    ASSERT(picker.accept());
    complete(picker);
    ASSERT(choices == 0);
}

TEST(file_completion_callbacks_can_destroy_the_picker_during_model_delivery)
{
    auto path = std::filesystem::temp_directory_path();
    auto read = [path](auto const&, std::stop_token) {
        return DirectoryListing{.directory = path,
                                .entries = {std::filesystem::directory_entry{path}},
                                .error = {}};
    };
    auto picker = std::make_unique<FilePicker>(
        FilePicker::Options{.directory = path, .loader = read});
    auto lifetime = track(*picker);
    for (auto& child : picker->get_children()) {
        if (auto* list = dynamic_cast<ListView*>(&child)) {
            list->on_scroll.connect([&](auto, auto, auto) { picker.reset(); });
        }
    }
    until([&] {
        if (auto* self = lifetime.try_get()) { self->poll(); }
        return !lifetime.valid();
    });
    ASSERT(!picker);
}
