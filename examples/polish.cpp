#include <algorithm>
#include <array>
#include <format>
#include <iostream>
#include <ox/ox.hpp>
using namespace ox;

int main(int argc, char** argv)
try {
    auto check = argc == 2 && std::string_view{argv[1]} == "--check";
    if (argc > 1 && !check) {
        std::cerr << "Usage: polish [--check]\n";
        return 2;
    }
    auto editor = TextBox{{.text = "A little room to think\n\nQuiet details make a "
                                   "working tool feel considered.\n"
                                   "Keep the structure clear, the colors restrained, "
                                   "and the important things easy to find.\n\n"
                                   "Unicode belongs here too: 界 é 👩‍💻\n\n"
                                   "Try selecting a sentence, clicking a line number, "
                                   "or narrowing this pane.\n"}};
    auto table = DataTable{{.columns = {{"Project", SizePolicy::min(16)},
                                        {"Tasks", SizePolicy::fixed(9), Align::Right},
                                        {"Status", SizePolicy::min(14)}},
                            .rows = {{"Atlas", "12", "Ready"},
                                     {"Bloom", "3", "Review"},
                                     {"Cedar", "28", "Ready"},
                                     {"Delta", "7", "Blocked"},
                                     {"Ember", "16", "Review"},
                                     {"Fern 界", "9", "Ready"},
                                     {"Grove", "21", "Ready"},
                                     {"Harbor", "5", "Blocked"}},
                            .empty_text = "Nothing here yet"}};
    auto text_bar = ScrollBar{}, table_bar = ScrollBar{};
    link(editor, text_bar);
    link(table, table_bar);
    auto notes = Padding{Row{std::tie(editor, text_bar)}, std::array{1, 1, 1, 1}} |
                 Border{.box = shape::Box::round(),
                        .label = {.text = " Notes ", .align = Align::Start}};
    auto projects = Padding{Row{std::tie(table, table_bar)}, std::array{1, 1, 1, 1}} |
                    Border{.box = shape::Box::round(),
                           .label = {.text = " Projects ", .align = Align::Start}};
    auto panes = SplitPane{std::move(notes), std::move(projects), .48};
    auto preset =
        Select{{.items = {{"Minimal"}, {"Refined"}, {"Roomy"}}, .selected = 1}};
    auto theme = Select{{.items = {{"Dark"}, {"Light"}, {"Terminal"}}, .selected = 0}};
    preset.width_policy = theme.width_policy = SizePolicy::fixed(16);
    auto numbers = CheckBox{{.checked = true, .label = {.text = "Numbers"}}};
    auto current = CheckBox{{.checked = true, .label = {.text = "Current line"}}};
    auto stripes = CheckBox{{.checked = true, .label = {.text = "Stripes"}}};
    auto hover = CheckBox{{.checked = true, .label = {.text = "Hover"}}};
    auto rules = CheckBox{{.label = {.text = "Rules"}}};
    auto title = Label{{.text = "Quiet details",
                        .align = Align::Start,
                        .style = {.traits = Trait::Bold},
                        .role = Surface::Accent}};
    auto hint = Label{{.text = "Tab: focus  |  Click headers: sort  |  Alt-S: stack "
                               "panes  |  Ctrl-C: exit",
                       .align = Align::Start,
                       .role = Surface::Muted}};
    auto status =
        Label{{.text = "A notebook and a small project list. Make them your own.",
               .align = Align::Start,
               .role = Surface::Muted}};
    auto toolbar = Row{std::tie(title, preset, theme)};
    auto settings = Row{std::tie(numbers, current, stripes, hover, rules)};
    toolbar.height_policy = settings.height_policy = SizePolicy::fixed(1);
    auto root = Column{std::tie(toolbar, settings, hint, panes, status)};
    auto app = Application{root};
    auto apply = [&] {
        auto selected = preset.selected_index().value_or(1);
        editor.line_numbers = numbers.checked();
        editor.highlight_line = current.checked();
        table.striped = stripes.checked();
        table.hover_rows = hover.checked();
        table.column_lines = rules.checked();
        table.row_lines = rules.checked();
        table.header_separator = selected != 0;
        table.row_height = selected == 2 ? 3 : 1;
        table.cell_padding = selected == 2 ? 2 : 1;
        table.show_sort_indicator = true;
        editor.refresh();
        table.refresh();
    };
    for (auto* checkbox : {&numbers, &current, &stripes, &hover, &rules})
        checkbox->on_change.connect([&](bool) { apply(); });
    preset.on_change.connect([&](auto index) {
        auto detailed = index.value_or(0) != 0;
        numbers.set_checked(detailed);
        current.set_checked(detailed);
        stripes.set_checked(detailed);
        hover.set_checked(detailed);
        rules.set_checked(false);
        apply();
    });
    auto change_theme = [&] {
        auto index = theme.selected_index().value_or(0);
        app.set_theme(index == 0   ? Theme::dark()
                      : index == 1 ? Theme::light()
                                   : Theme::terminal());
    };
    theme.on_change.connect([&](auto) { change_theme(); });
    table.cell_style = [&](std::size_t row, std::size_t column) {
        auto patch = StylePatch{};
        if (column == 1) patch.foreground = app.theme().palette.muted;
        if (column == 2) {
            auto const& value = table.rows()[row][column];
            patch.foreground = value == "Ready"     ? app.theme().palette.positive
                               : value == "Blocked" ? app.theme().palette.negative
                                                    : app.theme().palette.secondary;
        }
        return patch;
    };
    table.on_header.connect([&](std::size_t column) {
        auto previous = table.sort_order();
        auto ascending = !previous || previous->first != column || !previous->second;
        table.sort_rows(
            column, ascending,
            column == 1
                ? std::function<bool(std::string_view, std::string_view)>{[](auto a,
                                                                             auto b) {
                      return std::stoi(std::string{a}) < std::stoi(std::string{b});
                  }}
                : std::function<bool(std::string_view, std::string_view)>{});
        status.set_text("Sorted by " + table.columns()[column].heading +
                        (ascending ? " ascending" : " descending"));
    });
    editor.on_change.connect([&](auto const&) {
        status.set_text(std::format("Draft updated · {} characters",
                                    editor.editor().boundaries().size() - 1));
    });
    root.shortcuts = [&](KeyEvent const& key) -> EventResponse {
        if (!key.matches(U's', {.alt = true})) return {};
        panes.vertical = !panes.vertical;
        panes.refresh();
        return {.handled = true, .repaint = true};
    };
    change_theme();
    apply();
    if (!check) return app.run({.mouse_mode = MouseMode::Move});
    auto renderer = Renderer{
        esc::resolve_profile({"xterm-256color", "truecolor", false}, true).profile};
    auto focus_visible = [&](Widget& widget) {
        auto snapshot = widget_tree(root);
        auto order = focus_order(snapshot);
        auto visible = std::ranges::any_of(order, [&](auto index) {
            return snapshot[index].widget.try_get() == &widget;
        });
        if (visible)
            app.set_focus(widget);
        else
            app.clear_focus();
    };
    for (std::size_t skin = 0; skin < 3; ++skin) {
        theme.select(skin);
        for (std::size_t look = 0; look < 3; ++look) {
            preset.select(look);
            for (auto size :
                 {Area{120, 38}, Area{80, 24}, Area{30, 10}, Area{1, 1}, Area{0, 0}}) {
                app.dispatch(esc::Resize{size});
                focus_visible(editor);
                app.step();
                app.paint();
                renderer.render(app.loop().screen, app.loop().cursor);
                if (!renderer.render(app.loop().screen, app.loop().cursor).empty())
                    throw std::runtime_error{"Unstable frame"};
                focus_visible(table);
                table.select(5);
                table.sort_rows(1);
                table.handle(MouseMove{.at = {2, 4}, .held_button = {}});
                app.step();
                app.paint();
            }
        }
    }
    std::cout << "Polish: three presets, three themes, five sizes, focus, sorting, "
                 "hover and stable frames passed.\n";
    return 0;
}
catch (std::exception const& error) {
    std::cerr << error.what() << '\n';
    return 1;
}
