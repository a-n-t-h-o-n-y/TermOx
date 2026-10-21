#include <atomic>
#include <iostream>
#include <map>
#include "explorer_model.hpp"

using namespace ox;
namespace fs = std::filesystem;
using namespace std::chrono_literals;

namespace {
// A built-in one-child layout lets SplitPane borrow a named widget.
auto view(Widget& child) { return Column{std::tie(child)}; }
auto label(std::string text) {
    return Label{{.text = std::move(text), .align = Align::Start}};
}
void dump(Frame const& frame, bool json)
{
    if (json)
        std::cout << "{\"width\":" << frame.size().width
                  << ",\"height\":" << frame.size().height << ",\"rows\":[";
    for (int y = 0; y < frame.size().height; ++y) {
        if (json) {
            if (y) std::cout << ',';
            std::cout << '[';
        }
        for (int x = 0; x < frame.size().width; ++x) {
            auto const& c = frame[Point{x, y}];
            if (json) {
                if (x) std::cout << ',';
                auto color = [](Color const& color) -> std::string {
                    if (auto* rgb = std::get_if<RGB>(&color))
                        return std::to_string(rgb->red) + "," +
                               std::to_string(rgb->green) + "," +
                               std::to_string(rgb->blue);
                    return "";
                };
                std::cout << "{\"text\":" << std::quoted(esc::utf32_to_utf8(c.text))
                          << ",\"columns\":" << c.columns
                          << ",\"fg\":" << std::quoted(color(c.style.foreground))
                          << ",\"bg\":" << std::quoted(color(c.style.background))
                          << '}';
            }
            else if (c.columns)
                std::cout << esc::utf32_to_utf8(c.text);
        }
        if (json)
            std::cout << ']';
        else
            std::cout << '\n';
    }
    if (json) std::cout << "]}\n";
}} // namespace

int main(int argc, char** argv)
try {
    auto start = fs::current_path();
    bool snapshot = false, json = false, check = false;
    int width = 120, height = 32;
    auto initial_filter = std::string{};
    auto initial_selection = std::string{};
    auto theme_name = std::string{"dark"};
    for (int i = 1; i < argc; ++i) {
        auto arg = std::string{argv[i]};
        auto value = [&]() -> std::string {
            if (++i == argc) throw std::invalid_argument{"Missing value for " + arg};
            return argv[i];
        };
        if (arg == "--json") { snapshot = true; json = true; }
        else if (arg == "--dump") snapshot = true;
        else if (arg == "--check") check = true;
        else if (arg == "--width") width = std::stoi(value());
        else if (arg == "--height") height = std::stoi(value());
        else if (arg == "--filter") initial_filter = value();
        else if (arg == "--select") initial_selection = value();
        else if (arg == "--theme") theme_name = value();
        else if (arg == "--help") {
            std::cout << "Usage: explorer [directory] [--theme dark|light|terminal] "
                         "[--filter text] [--select filename]\n"
                         "       explorer [directory] --dump|--json|--check "
                         "[--width columns] [--height rows]\n";
            return 0;
        }
        else if (arg.starts_with('-')) throw std::invalid_argument{"Unknown option: " + arg};
        else start = arg;
    }
    if (width < 0 || width > 500 || height < 0 || height > 200)
        throw std::invalid_argument{"Snapshot dimensions must be 0–500 columns and 0–200 rows"};
    if (theme_name != "dark" && theme_name != "light" && theme_name != "terminal")
        throw std::invalid_argument{"Theme must be dark, light, or terminal"};

    auto path = LineEdit{{.placeholder = "Directory path — Enter to open"}};
    auto up = Button{{.label = {.text = "Up"}}};
    auto reload = Button{{.label = {.text = "Refresh"}}};
    auto heading = label("Explorer");
    heading.role = Surface::Accent;
    heading.width_policy = SizePolicy::fixed(10);
    up.width_policy = SizePolicy::fixed(6);
    reload.width_policy = SizePolicy::fixed(11);
    auto location = Row{std::tie(heading, up, path, reload)};
    location.gap = 1;
    location.height_policy = SizePolicy::fixed(1);

    auto filter = LineEdit{{.text = initial_filter, .placeholder = "Filter names"}};
    auto hidden = CheckBox{{.label = {.text = "Hidden files"}}};
    auto preview_toggle = CheckBox{{.checked = true, .label = {.text = "Preview"}}};
    hidden.width_policy = SizePolicy::fixed(18);
    preview_toggle.width_policy = SizePolicy::fixed(14);
    auto search = Row{std::tie(filter, hidden, preview_toggle)};
    search.gap = 1;
    search.height_policy = SizePolicy::fixed(1);

    auto folders = TreeView{};
    auto folder_bar = ScrollBar{};
    auto folder_rows = Row{std::tie(folders, folder_bar)};
    auto folder_frame = view(folder_rows) | Border{.label = {.text = "Folders"}};
    link(folders, folder_bar);
    auto table = DataTable{{
        .columns = {{.heading = "Name", .size = SizePolicy::min(16)},
                    {.heading = "Bytes", .size = SizePolicy::fixed(12), .align = Align::End},
                    {.heading = "Type", .size = SizePolicy::fixed(14)},
                    {.heading = "Modified (UTC)", .size = SizePolicy::fixed(18)}},
        .striped = true, .hover_rows = true, .empty_text = "Loading…"}};
    auto table_bar = ScrollBar{};
    auto table_rows = Row{std::tie(table, table_bar)};
    auto table_horizontal = ScrollBar{{.vertical = false}};
    auto table_area = Column{std::tie(table_rows, table_horizontal)};
    link(table, table_horizontal);
    auto table_frame = view(table_area) | Border{.label = {.text = "Files"}};
    link(table, table_bar);
    auto preview_text = TextBox{{.text = "Select a file to preview.", .read_only = true}};
    auto image = example::BmpImage{};
    auto preview_pages = Stack{std::tie(preview_text, image)};
    auto fit = Button{{.label = {.text = "Fit"}}};
    auto actual = Button{{.label = {.text = "100%"}}};
    auto smaller = Button{{.label = {.text = "−"}}};
    auto larger = Button{{.label = {.text = "+"}}};
    auto image_tools = Row{std::tie(fit, actual, smaller, larger)};
    image_tools.height_policy = SizePolicy::fixed(1);
    image_tools.visible = false;
    auto preview_body = Column{std::tie(image_tools, preview_pages)};
    auto preview_frame = view(preview_body) | Border{.label = {.text = "Preview"}};
    auto files_and_preview = SplitPane{view(table_frame), view(preview_frame), .62};
    auto body = SplitPane{view(folder_frame), view(files_and_preview), .20};
    body.width_policy = SizePolicy::flex();
    body.height_policy = SizePolicy::flex();
    auto status = label("Loading…");
    auto palette = CommandPalette{};
    auto command_dialog = Dialog{view(palette), "Commands"};
    command_dialog.requested_size = Area{52, 14};

    auto directory = explorer::Directory{};
    auto rows = std::vector<std::size_t>{};
    auto tree_paths = std::map<std::vector<std::size_t>, fs::path>{};
    auto selected = std::optional<fs::path>{};
    int sort_column = 0;
    bool ascending = true, rebuilding = false, loading = false, pending_preview = false;
    bool first_load = true;
    auto generation = std::atomic<std::uint64_t>{0};
    std::function<void(explorer::Worker::Task)> enqueue;
    std::function<void(fs::path)> navigate;
    std::function<void()> rebuild_rows, preview;
    std::function<void(int)> set_theme;
    std::function<void()> focus_path, focus_filter;

    auto parent_command = command({.text = "Parent directory", .action = [&] {
        navigate(directory.path.parent_path()); return EventResponse{};
    }, .keys = {{KeyCode::ArrowUp, {.alt = true}}}});
    auto refresh_command = command({.text = "Refresh", .action = [&] {
        navigate(directory.path); return EventResponse{};
    }, .keys = {{KeyCode::F5, {}}}});
    auto path_command = command({.text = "Go to path", .action = [&] {
        focus_path(); return EventResponse{};
    }, .keys = {{U'l', {.ctrl = true}}}});
    auto filter_command = command({.text = "Filter files", .action = [&] {
        focus_filter(); return EventResponse{};
    }, .keys = {{U'f', {.ctrl = true}}}});
    auto commands_command = command({.text = "Commands", .action = [&] {
        command_dialog.show(); return EventResponse{};
    }, .keys = {{U'p', {.ctrl = true}}}});
    auto preview_command = command({.text = "Toggle preview", .action = [&] {
        preview_toggle.toggle(); return EventResponse{};
    }, .keys = {{KeyCode::F4, {}}}});
    auto hidden_command = command({.text = "Show hidden files", .action = [&] {
        hidden.toggle(); return EventResponse{};
    }});
    auto dark = command({.text = "Dark theme", .action = [&] { set_theme(0); return EventResponse{}; }});
    auto light = command({.text = "Light theme", .action = [&] { set_theme(1); return EventResponse{}; }});
    auto terminal = command({.text = "Terminal theme", .action = [&] { set_theme(2); return EventResponse{}; }});
    auto quit = command({.text = "Quit", .action = [] { return EventResponse{.quit = 0}; }});
    auto menu = MenuBar{{{"File", {{parent_command}, {refresh_command}, {path_command}, {quit}}},
                         {"View", {{preview_command}, {hidden_command}, {dark}, {light}, {terminal}}}}};
    menu.height_policy = SizePolicy::fixed(1);
    auto help = KeyHelp{{path_command, filter_command, parent_command, refresh_command,
                         preview_command, commands_command}};
    help.height_policy = SizePolicy::fixed(1);
    palette.set_commands({parent_command, refresh_command, path_command, filter_command,
                          preview_command, hidden_command, dark, light, terminal, quit});
    auto content = Column{std::tie(location, menu, search, body, status, help, command_dialog)};
    auto root = Padding{std::move(content), std::array{1, 1, 1, 1}};
    auto app = Application{root};
    auto focus = [&](Widget& target) {
        for (auto const& node : widget_tree(root))
            if (node.widget == track(target) && node.input &&
                node.visible.size.width > 0 && node.visible.size.height > 0) {
                app.set_focus(target);
                return;
            }
    };
    focus_path = [&] { focus(path); path.select_all(); };
    focus_filter = [&] { focus(filter); filter.select_all(); };
    set_theme = [&](int index) {
        auto theme = index == 0 ? Theme::dark() : index == 1 ? Theme::light() : Theme::terminal();
        theme.motion.enabled = false;
        app.set_theme(theme);
    };
    set_theme(theme_name == "dark" ? 0 : theme_name == "light" ? 1 : 2);
    auto show_text = [&](std::string text) {
        preview_text.set_text(text);
        image_tools.visible = false;
        preview_pages.select(0);
        preview_body.refresh();
    };
    auto report_count = [&] {
        status.set_text(std::to_string(rows.size()) + " / " + std::to_string(directory.entries.size()) +
                        " entries" + (directory.limited ? " — directory limited to 10,000 entries" : ""));
    };
    preview = [&] {
        auto ticket = ++generation;
        pending_preview = false;
        preview_frame.border.label.text = selected ? explorer::display(selected->filename().string()) : "Preview";
        preview_frame.refresh();
        if (!selected || !preview_toggle.checked()) { show_text("Select a file to preview."); return; }
        auto file = *selected;
        auto entry = std::find_if(directory.entries.begin(), directory.entries.end(),
                                  [&](auto const& x) { return x.path == file; });
        if (entry == directory.entries.end() || entry->directory) {
            show_text("Folder — press Enter to open."); return;
        }
        show_text("Loading preview…");
        pending_preview = true;
        enqueue([&, file, ticket](std::stop_token stop) {
            auto result = explorer::Preview{};
            try { if (!stop.stop_requested() && generation == ticket) result = explorer::read_preview(file); }
            catch (std::exception const& e) { result.text = "Preview unavailable: " + explorer::display(e.what()); }
            if (stop.stop_requested() || generation != ticket) return;
            app.post(event::Custom{.action = [&, ticket, result = std::move(result)]() mutable {
                if (generation != ticket) return EventResponse{};
                pending_preview = false;
                if (result.image) {
                    result.image->name = explorer::display(result.image->name);
                    image.set_image(std::move(*result.image));
                    image_tools.visible = true;
                    preview_pages.select(1);
                    preview_body.refresh();
                    status.set_text(image.description());
                }
                else show_text(std::move(result.text));
                return EventResponse{};
            }});
        });
    };
    rebuild_rows = [&] {
        if (loading) return;
        rebuilding = true;
        rows = explorer::visible_rows(directory, filter.text(), hidden.checked(), sort_column, ascending);
        auto data = std::vector<std::vector<std::string>>{};
        std::optional<std::size_t> restore;
        for (auto i : rows) {
            auto const& entry = directory.entries[i];
            if (selected && entry.path == *selected) restore = data.size();
            data.push_back({entry.name, entry.bytes ? std::to_string(*entry.bytes) : "—",
                            entry.type, explorer::timestamp(entry.modified)});
        }
        table.empty_text = directory.entries.empty() ? "Empty directory" : "No matching files";
        table.set_rows(std::move(data));
        constexpr std::array headings{"Name", "Bytes", "Type", "Modified"};
        table_frame.border.label.text = std::string{"Files — "} + headings[sort_column] + (ascending ? " ↑" : " ↓");
        table_frame.refresh();
        if (!restore && !rows.empty()) restore = 0;
        if (restore) {
            selected = directory.entries[rows[*restore]].path;
            table.select(*restore);
        }
        else selected.reset();
        rebuilding = false;
        report_count();
        preview();
    };
    auto rebuild_tree = [&] {
        tree_paths.clear();
        auto chain = std::vector<fs::path>{};
        for (auto at = directory.path;; at = at.parent_path()) {
            chain.push_back(at);
            if (at == at.root_path() || at.parent_path() == at) break;
        }
        std::reverse(chain.begin(), chain.end());
        auto nodes = std::vector<TreeNode>{};
        auto* children = &nodes;
        auto indices = std::vector<std::size_t>{};
        for (auto const& item : chain) {
            indices.push_back(0);
            children->push_back({explorer::display(item.filename().empty() ? item.string() : item.filename().string()), true, true});
            tree_paths[indices] = item;
            children = &children->back().children;
        }
        auto dirs = explorer::visible_rows(directory, "", hidden.checked(), 0, true);
        for (auto i : dirs) if (directory.entries[i].directory) {
            auto child_path = indices; child_path.push_back(children->size());
            tree_paths[child_path] = directory.entries[i].path;
            children->push_back({directory.entries[i].name});
        }
        folders.set_nodes(std::move(nodes));
        folders.select(indices);
    };
    navigate = [&](fs::path requested) {
        if (requested.empty()) requested = start;
        if (requested.is_relative() && !directory.path.empty()) requested = directory.path / requested;
        auto ticket = ++generation;
        loading = true; pending_preview = false;
        table.enabled = false; folders.enabled = false; root.refresh();
        status.set_text("Loading " + explorer::display(requested.string()));
        enqueue([&, requested, ticket](std::stop_token stop) {
            auto result = explorer::Directory{};
            auto error = std::string{};
            auto cancelled = [&] { return stop.stop_requested() || generation != ticket; };
            try { if (!cancelled()) result = explorer::read_directory(requested, cancelled); }
            catch (std::exception const& e) { error = explorer::display(e.what()); }
            if (cancelled()) return;
            app.post(event::Custom{.action = [&, ticket, result = std::move(result), error = std::move(error)]() mutable {
                if (generation != ticket) return EventResponse{};
                loading = false;
                table.enabled = true; folders.enabled = true; root.refresh();
                if (!error.empty()) { status.set_text("Cannot open directory: " + error); return EventResponse{}; }
                directory = std::move(result);
                if (first_load) {
                    if (!initial_selection.empty()) selected = directory.path / initial_selection;
                    first_load = false;
                }
                path.set_text(explorer::display(directory.path.string()));
                rebuild_tree();
                rebuild_rows();
                focus(table);
                return EventResponse{};
            }});
        });
    };
    table.on_select.connect([&](std::optional<std::size_t> row) {
        if (rebuilding || loading) return;
        selected = row && *row < rows.size() ? std::optional{directory.entries[rows[*row]].path} : std::nullopt;
        report_count(); preview();
    });
    table.on_activate.connect([&](std::size_t row) {
        if (!loading && row < rows.size() && directory.entries[rows[row]].directory)
            navigate(directory.entries[rows[row]].path);
    });
    table.on_header.connect([&](std::size_t column) {
        if (column > 3 || loading) return;
        ascending = sort_column == (int)column ? !ascending : true;
        sort_column = (int)column; rebuild_rows();
    });
    folders.on_activate.connect([&](auto const& node) {
        if (!loading && tree_paths.contains(node)) navigate(tree_paths.at(node));
    });
    path.on_submit.connect([&](std::string const& value) { navigate(fs::path{value}); });
    filter.on_change.connect([&](auto const&) { rebuild_rows(); });
    hidden.on_change.connect([&](bool) { if (!loading) { rebuild_tree(); rebuild_rows(); } });
    preview_toggle.on_change.connect([&](bool visible) {
        // SplitPane always arranges both panes; a full ratio returns its space to the table.
        preview_frame.visible = visible;
        files_and_preview.set_ratio(visible ? .62 : 1.);
        root.refresh();
        if (!loading) preview();
    });
    up.on_activate.connect([&] { navigate(directory.path.parent_path()); });
    reload.on_activate.connect([&] { navigate(directory.path); });
    fit.on_activate.connect([&] { image.fit(); });
    actual.on_activate.connect([&] { image.zoom(1.); });
    smaller.on_activate.connect([&] { image.zoom_by(.5); });
    larger.on_activate.connect([&] { image.zoom_by(2.); });
    palette.on_execute.connect([&](std::size_t) { command_dialog.reject(); });

    // Last declared: all worker captures and Application outlive the joining thread.
    auto worker = explorer::Worker{};
    enqueue = [&](explorer::Worker::Task task) { worker.submit(std::move(task)); };
    navigate(start);
    if (!snapshot && !check) {
        return app.run({.mouse_mode = MouseMode::Move});
    }
    auto settle = [&] {
        auto deadline = std::chrono::steady_clock::now() + 10s;
        do {
            app.step();
            if (!loading && !pending_preview) return;
            std::this_thread::sleep_for(1ms);
        } while (std::chrono::steady_clock::now() < deadline);
        throw std::runtime_error{"Explorer worker timed out"};
    };
    app.post(esc::Resize{{width, height}});
    settle();
    if (directory.path.empty()) throw std::runtime_error{status.text};
    focus(table);
    if (!initial_selection.empty()) {
        auto match = std::find_if(rows.begin(), rows.end(), [&](auto i) {
            return directory.entries[i].path.filename().string() == initial_selection;
        });
        if (match == rows.end()) throw std::invalid_argument{"Selected file is not visible"};
        table.select((std::size_t)(match - rows.begin())); settle();
    }
    if (check) {
        auto require = [](bool condition, char const* message) {
            if (!condition) throw std::runtime_error{message};
        };
        auto original = directory.path;
        auto initial_rows = rows.size();
        auto original_selection = selected;
        for (int column = 0; column < 4; ++column) {
            table.on_header(column); settle();
            require(selected == original_selection, "Sorting lost file identity");
        }
        filter.set_text("a-file-that-does-not-exist-in-the-explorer-fixture"); settle();
        require(rows.empty() && !selected, "Filtering left a stale selection");
        filter.set_text(""); settle();
        require(rows.size() == initial_rows, "Clearing the filter lost rows");
        hidden.toggle(); settle(); hidden.toggle(); settle();
        navigate(original / "a-directory-that-does-not-exist-in-the-explorer-fixture"); settle();
        require(directory.path == original, "Failed navigation replaced the directory");
        navigate(original / "another-missing-directory"); navigate(original); settle();
        require(directory.path == original && !loading, "Stale navigation replaced the directory");
        for (auto const& entry : directory.entries) if (entry.directory) {
            auto child = entry.path;
            navigate(child); settle(); require(directory.path == fs::canonical(child), "Directory did not open");
            navigate(original); settle(); break;
        }
        preview_toggle.toggle(); settle(); preview_toggle.toggle(); settle();
        app.dispatch(KeyEvent{.key = U'p', .modifiers = {.ctrl = true}});
        app.dispatch(KeyEvent{.key = KeyCode::Escape});
        for (int theme = 0; theme < 3; ++theme) {
            set_theme(theme);
            for (auto size : {Area{120, 32}, Area{45, 15}, Area{12, 5}, Area{1, 1}, Area{0, 0}}) {
                app.post(esc::Resize{size}); app.step(); app.paint();
            }
        }
        std::cout << "Explorer: navigation, filtering, sorting, stale results, overlays, themes, and resizing passed\n";
    }
    else { app.paint(); dump(app.loop().screen, json); }
    return 0;
}
catch (std::exception const& e) {
    std::cerr << "Explorer: " << explorer::display(e.what()) << '\n';
    return 2;
}
