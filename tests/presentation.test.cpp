#include <limits>
#include <ox/ox.hpp>
#include <zzz/test.hpp>
using namespace ox;

TEST(text_gutter_tracks_logical_lines_wraps_selection_and_mouse_coordinates)
{
    auto box = TextBox{{.text = "abcd界é\nz\n",
                        .wrap = TextWrap::Any,
                        .line_numbers = true,
                        .highlight_line = true}};
    auto app = Application{box};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{8, 5}});
    app.set_focus(box);
    app.paint();
    auto const& frame = app.loop().screen;
    ASSERT((box.viewport_size() == Area{6, 5}));
    ASSERT((frame[Point{0, 0}].text == U"1"));
    ASSERT((frame[Point{2, 0}].text == U"a"));
    ASSERT((frame[Point{0, 1}].text == U"↪"));
    ASSERT((frame[Point{0, 2}].text == U"2"));
    ASSERT(
        (frame[Point{7, 0}].style.background != frame[Point{7, 2}].style.background));
    app.dispatch(MousePress{.at = {2, 2}, .button = MouseButton::Left});
    app.dispatch(MouseRelease{.at = {2, 2}, .button = MouseButton::Left});
    ASSERT(box.editor().caret() == 11);
    app.paint();
    ASSERT((app.loop().cursor == Point{2, 2}));
    app.dispatch(MousePress{.at = {0, 0}, .button = MouseButton::Left});
    ASSERT((box.editor().selection() == std::pair<std::size_t, std::size_t>{0, 11}));
    ASSERT(box.editor().caret() == 11);
    box.set_scroll_offset({2, 0});
    app.dispatch(MousePress{.at = {0, 2}, .button = MouseButton::Left});
    ASSERT((box.editor().selection() == std::pair<std::size_t, std::size_t>{11, 13}));
}

TEST(text_gutter_reflows_at_digit_boundaries_and_survives_streaming_and_moves)
{
    auto box = TextBox{{.text = "one\n", .line_numbers = true}};
    auto app = Application{box};
    app.dispatch(esc::Resize{{8, 3}});
    for (int i = 0; i < 8; ++i)
        box.append_lines("next\n");
    app.step();
    ASSERT((box.viewport_size() == Area{5, 3}));
    auto moved = std::move(box);
    moved.append_lines("tail\n", 4);
    ASSERT((moved.viewport_size() == Area{5, 3}));
    moved.set_text("a\n");
    moved.refresh();
    ASSERT((moved.viewport_size() == Area{6, 3}));
    for (auto size : {Area{0, 0}, Area{1, 1}, Area{2, 2}}) {
        arrange(moved, {{}, size});
        auto frame = Frame{size};
        moved.paint(Canvas{frame});
        ASSERT(moved.viewport_size().width == 0);
    }
}

TEST(text_focus_and_inactive_selection_styles_preserve_explicit_choices)
{
    auto box = TextBox{{.text = "selected",
                        .focused_style = {.background = {},
                                          .foreground = RGB{0xabcdef},
                                          .enabled = {},
                                          .disabled = {}},
                        .blurred_style = {.background = {},
                                          .foreground = RGB{0x123456},
                                          .enabled = {},
                                          .disabled = {}},
                        .inactive_selection_style = {.background = RGB{0x334455},
                                                     .foreground = {},
                                                     .enabled = {},
                                                     .disabled = {}}}};
    auto root = Row{std::tie(box)};
    auto app = Application{root};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{12, 1}});
    app.set_focus(box);
    box.select_all();
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].style.foreground ==
            app.theme().palette.accent));
    app.clear_focus();
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].style.background == Color{RGB{0x334455}}));
    ASSERT(
        (app.loop().screen[Point{0, 0}].style.foreground == app.theme().palette.muted));
}

TEST(table_density_controls_extent_selection_hits_and_header_separator)
{
    auto table = DataTable{{.columns = {{"Name", SizePolicy::fixed(8)}},
                            .rows = {{"one"}, {"two"}, {"three"}},
                            .header_separator = false,
                            .row_lines = true,
                            .cell_padding = 2,
                            .row_height = 3}};
    arrange(table, {{}, {8, 8}});
    ASSERT((table.content_size() == Area{8, 11}));
    ASSERT((table.viewport_size() == Area{8, 7}));
    auto frame = Frame{table.size()};
    table.paint(Canvas{frame});
    ASSERT((frame[Point{2, 2}].text == U"o"));
    ASSERT(
        !table.handle(MousePress{.at = {2, 4}, .button = MouseButton::Left}).handled);
    ASSERT(table.handle(MousePress{.at = {2, 5}, .button = MouseButton::Left}).handled);
    ASSERT(table.selected_row() == 1);
    table.select(2);
    ASSERT(table.scroll_offset().y == 4);
    table.handle(KeyEvent{.key = KeyCode::Home});
    ASSERT(table.selected_row() == 0 && table.scroll_offset().y == 0);
    table.handle(KeyEvent{.key = KeyCode::PageDown});
    ASSERT(table.selected_row() == 1);
    table.handle(MouseWheel{.at = {}, .dy = 1});
    ASSERT(table.scroll_offset().y == 4);
}

TEST(table_visible_cell_styles_hover_stripes_and_selection_have_explicit_precedence)
{
    int styled = 0;
    auto table = DataTable{{.columns = {{"Name", SizePolicy::fixed(10)}},
                            .rows = {{"one"}, {"two"}, {"three"}},
                            .inactive_selected_style = {.background = {},
                                                        .foreground = RGB{0x778899},
                                                        .enabled = {},
                                                        .disabled = {}},
                            .hover_style = {.background = RGB{0x445566},
                                            .foreground = {},
                                            .enabled = {},
                                            .disabled = {}},
                            .striped = true,
                            .hover_rows = true,
                            .cell_style = [&](auto, auto) {
                                ++styled;
                                return StylePatch{.background = {},
                                                  .foreground = RGB{0x112233},
                                                  .enabled = {},
                                                  .disabled = {}};
                            }}};
    auto app = Application{table};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{10, 4}});
    app.set_focus(table);
    app.paint();
    ASSERT(styled == 2);
    ASSERT((app.loop().screen[Point{1, 2}].style.foreground == Color{RGB{0x112233}}));
    auto first = app.loop().screen[Point{1, 2}].style.background;
    auto second = app.loop().screen[Point{1, 3}].style.background;
    ASSERT(first != second);
    table.handle(MouseMove{.at = {1, 3}, .held_button = {}});
    ASSERT(table.hovered_row() == 1);
    app.request_repaint();
    app.paint();
    ASSERT((app.loop().screen[Point{1, 3}].style.background == Color{RGB{0x445566}}));
    table.select(1);
    app.paint();
    ASSERT((app.loop().screen[Point{1, 3}].style.background ==
            app.theme().palette.selection));
    app.clear_focus();
    app.paint();
    ASSERT((app.loop().screen[Point{1, 3}].style.foreground == Color{RGB{0x778899}}));
    table.mouse_leave();
    ASSERT(!table.hovered_row());
}

TEST(table_sort_indicator_tracks_model_changes_and_ascii_policy)
{
    auto table = DataTable{
        {.columns = {{"Name", SizePolicy::fixed(10)}}, .rows = {{"b"}, {"a"}}}};
    auto app = Application{table};
    app.dispatch(esc::Resize{{10, 4}});
    table.sort_rows(0);
    app.paint();
    ASSERT((table.sort_order() == std::pair<std::size_t, bool>{0, true}));
    ASSERT((app.loop().screen[Point{8, 0}].text == U"↑"));
    table.set_cell(0, 0, "z");
    ASSERT(!table.sort_order());
    table.sort_rows(0, false);
    app.set_theme(Theme::terminal());
    app.paint();
    ASSERT((app.loop().screen[Point{8, 0}].text == U"v"));
    table.erase_row(0);
    ASSERT(table.sort_order().has_value());
    table.add_row({"c"});
    ASSERT(!table.sort_order());
}

TEST(table_spacing_validation_rejects_invalid_and_overflowing_models)
{
    ASSERT_THROWS(DataTable(DataTable::Options{.row_height = 0}),
                  std::invalid_argument);
    ASSERT_THROWS(DataTable(DataTable::Options{.cell_padding = -1}),
                  std::invalid_argument);
    auto table = DataTable{{.columns = {{"a"}}, .rows = {{"a"}}}};
    table.row_height = std::numeric_limits<int>::max() / 2 + 1;
    table.show_header = false;
    table.refresh();
    auto before = table.content_size();
    ASSERT_THROWS(table.set_rows({{"a"}, {"b"}, {"c"}}), std::length_error);
    ASSERT(table.rows().size() == 1 && table.content_size() == before);
    ASSERT_THROWS(table.insert_row(1, {"b"}), std::length_error);
    ASSERT(table.rows().size() == 1);
}

TEST(text_gutter_can_reuse_empty_moved_from_controls_and_explicit_lineedit_focus_color)
{
    auto box = TextBox{{.line_numbers = true}};
    arrange(box, {{}, {8, 2}});
    auto moved = std::move(box);
    arrange(box, {{}, {8, 2}});
    auto frame = Frame{{8, 2}};
    box.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"1"));
    auto line = LineEdit{{.text = "text",
                          .focused_style = {.background = {},
                                            .foreground = RGB{0xabcdef},
                                            .enabled = {},
                                            .disabled = {}}}};
    auto app = Application{line};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{8, 1}});
    app.set_focus(line);
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].style.foreground == Color{RGB{0xabcdef}}));
}

TEST(table_spacing_clipping_and_hover_mapping_cover_partial_rows)
{
    for (int height : {1, 2, 3})
        for (bool separators : {false, true})
            for (bool header : {false, true})
                for (bool header_rule : {false, true}) {
                    auto rows = std::vector<std::vector<std::string>>{};
                    for (int i = 0; i < 10; ++i)
                        rows.push_back({std::to_string(i)});
                    auto table = DataTable{{.columns = {{"Row", SizePolicy::fixed(9)}},
                                            .rows = rows,
                                            .hover_rows = true,
                                            .header_separator = header_rule,
                                            .row_lines = separators,
                                            .row_height = height,
                                            .show_header = header}};
                    arrange(table, {{}, {9, 8}});
                    auto header_height = header ? 1 + (int)header_rule : 0;
                    auto stride = height + (int)separators;
                    ASSERT(table.content_size().height ==
                           10 * stride - (int)separators);
                    for (int offset : {0, 3, 13}) {
                        table.set_scroll_offset({0, offset});
                        auto frame = Frame{{9, 8}};
                        table.paint(Canvas{frame});
                        for (int y = header_height; y < 8; ++y) {
                            auto physical = table.scroll_offset().y + y - header_height;
                            auto valid =
                                physical / stride < 10 && physical % stride < height;
                            table.handle(MouseMove{.at = {1, y}, .held_button = {}});
                            ASSERT(table.hovered_row() ==
                                   (valid ? std::optional<std::size_t>{(
                                                std::size_t)(physical / stride)}
                                          : std::nullopt));
                            auto response = table.handle(
                                MousePress{.at = {1, y}, .button = MouseButton::Left});
                            ASSERT(response.handled == valid);
                            if (valid)
                                ASSERT(table.selected_row() ==
                                       (std::size_t)(physical / stride));
                        }
                    }
                }
}

TEST(table_replacement_validates_new_header_extent_before_committing)
{
    auto table = DataTable{{.row_height = std::numeric_limits<int>::max()}};
    ASSERT_THROWS(table.set_data({{"heading"}}, {{"value"}}), std::length_error);
    ASSERT(table.columns().empty() && table.rows().empty());
}

TEST(table_heading_and_empty_roles_respect_explicit_state_patches)
{
    auto column = TableColumn{"Heading", SizePolicy::fixed(12)};
    column.style.foreground = RGB{0x123456};
    column.heading_style.foreground = RGB{0xabcdef};
    column.heading_style.disabled.insert(Trait::Bold);
    auto table = DataTable{{.columns = {column}, .rows = {{"body"}}}};
    auto app = Application{table};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{12, 4}});
    app.set_focus(table);
    app.paint();
    ASSERT((app.loop().screen[Point{1, 0}].style.foreground == Color{RGB{0xabcdef}}));
    ASSERT((!app.loop().screen[Point{1, 0}].style.traits.contains(Trait::Bold)));
    ASSERT((app.loop().screen[Point{1, 2}].style.foreground == Color{RGB{0x123456}}));
    table.focused_style.background = RGB{0x334455};
    table.set_rows({});
    table.refresh();
    app.paint();
    ASSERT((app.loop().screen[Point{1, 2}].style.background == Color{RGB{0x334455}}));
}

TEST(table_presentation_refresh_updates_intrinsic_parent_layout_without_rearrange_loops)
{
    auto table = DataTable{{.columns = {{"a"}},
                            .rows = {{"a"}, {"b"}, {"c"}},
                            .show_header = false,
                            .height_policy = SizePolicy::flex(0)}};
    auto root = Column{std::tie(table)};
    auto app = Application{root};
    app.dispatch(esc::Resize{{20, 20}});
    ASSERT(table.size().height == 3);
    table.row_height = 3;
    table.refresh();
    app.step();
    ASSERT(table.size().height == 9);
    table.row_lines = true;
    table.refresh();
    app.step();
    ASSERT(table.size().height == 11);
}
