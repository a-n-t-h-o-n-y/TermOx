#include <zzz/test.hpp>

#include <limits>
#include <memory>

#include <ox/ox.hpp>

using namespace ox;

namespace {
auto table_options() -> DataTable::Options
{
    return {.columns = {{.heading = "Name", .size = SizePolicy::fixed(8)},
                        {.heading = "Count",
                         .size = SizePolicy::fixed(7),
                         .align = Align::Right}},
            .rows = {{"界élong", "2"}, {"🇺🇸", "10"}, {"third", "3"}, {"fourth", "4"}}};
}
}  // namespace

TEST(table_validates_rectangular_single_line_data_before_mutation)
{
    auto table = DataTable{table_options()};
    ASSERT(table.rows().size() == 4 && table.columns().size() == 2);
    ASSERT_THROWS(table.add_row({"short"}), std::invalid_argument);
    ASSERT_THROWS(table.set_cell(0, 0, "a\n"), std::invalid_argument);
    ASSERT_THROWS(table.set_cell(0, 0, "\xff"), esc::UnicodeError);
    ASSERT_THROWS(table.set_cell(10, 0, "a"), std::out_of_range);
    ASSERT_THROWS(table.set_data({{.heading = "a", .size = SizePolicy::fixed(-1)}}, {}),
                  std::invalid_argument);
    ASSERT_THROWS(table.set_rows({{"bad", "\t"}}), std::invalid_argument);
    ASSERT(table.rows().size() == 4 && table.rows()[0][0] == "界élong");
    table.set_cell(0, 1, "5");
    ASSERT(table.rows()[0][1] == "5");
    table.set_data({}, {});
    ASSERT(table.rows().empty() && table.columns().empty());
}

TEST(table_paints_unicode_alignment_headers_and_horizontal_clipping)
{
    auto table = DataTable{table_options()};
    arrange(table, {{}, {16, 4}});
    auto frame = Frame{table.size()};
    table.paint(Canvas{frame});
    ASSERT((frame[Point{1, 0}].text == U"N"));
    ASSERT((frame[Point{1, 2}].text == U"界"));
    ASSERT((frame[Point{3, 2}].text == U"é"));
    ASSERT((frame[Point{6, 2}].text == U"…"));
    ASSERT((frame[Point{14, 2}].text == U"2"));
    ASSERT((frame[Point{1, 3}].text == U"🇺🇸"));
    table.select(3);
    frame.clear();
    table.paint(Canvas{frame});
    ASSERT((frame[Point{1, 0}].text == U"N"));
    ASSERT((frame[Point{1, 2}].text == U"t"));
    ASSERT((frame[Point{1, 3}].style.traits.contains(Trait::Inverse)));
    arrange(table, {{}, {5, 4}});
    table.set_scroll_offset({2, 0});
    frame.resize(table.size());
    table.paint(Canvas{frame});
    ASSERT((frame[Point{0, 2}].text == U" "));
    ASSERT((frame[Point{1, 2}].text == U"é"));
    ASSERT((frame[Point{4, 2}].text == U"…"));
}

TEST(table_navigation_selection_activation_and_header_clicks_are_explicit)
{
    auto table = DataTable{table_options()};
    arrange(table, {{}, {16, 4}});
    auto app = Application{table};
    int selections = 0;
    auto activated = std::optional<std::size_t>{};
    auto header = std::optional<std::size_t>{};
    table.on_select.connect([&](auto) { ++selections; });
    table.on_activate.connect([&](auto row) { activated = row; });
    table.on_header.connect([&](auto column) { header = column; });
    ASSERT(app.dispatch(KeyEvent{.key = KeyCode::ArrowDown}).handled);
    ASSERT(table.selected_row() == 0);
    app.dispatch(KeyEvent{.key = KeyCode::PageDown});
    ASSERT(table.selected_row() == 2 && table.scroll_offset().y == 1);
    app.dispatch(KeyEvent{.key = KeyCode::End});
    ASSERT(table.selected_row() == 3 && table.scroll_offset().y == 2);
    app.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(activated == 3 && selections == 3);
    ASSERT(!app.dispatch(KeyEvent{.key = KeyCode::Enter, .action = KeyAction::Release})
                .handled);
    table.handle(MousePress{{10, 0}, MouseButton::Left});
    ASSERT(header == 1 && table.selected_row() == 3);
    ASSERT(!table.handle(MousePress{{8, 0}, MouseButton::Left}).handled);
    table.handle(MousePress{{1, 2}, MouseButton::Left});
    ASSERT(table.selected_row() == 2);
    ASSERT(!table.select(100));
    table.selectable = false;
    table.refresh();
    ASSERT(!table.selected_row());
    table.handle(KeyEvent{.key = KeyCode::Home});
    ASSERT(table.scroll_offset().y == 0);
}

TEST(table_mutations_and_stable_sort_preserve_selected_row_identity)
{
    auto table = DataTable{table_options()};
    arrange(table, {{}, {16, 4}});
    table.select(1);
    table.insert_row(0, {"new", "10"});
    ASSERT(table.selected_row() == 2 && table.rows()[2][0] == "🇺🇸");
    table.erase_row(0);
    ASSERT(table.selected_row() == 1);
    table.sort_rows(1, true, [](auto a, auto b) {
        return std::stoi(std::string{a}) < std::stoi(std::string{b});
    });
    ASSERT(table.selected_row() == 3 && table.rows()[3][0] == "🇺🇸");
    table.sort_rows(1, false);
    ASSERT(table.rows()[0][1] == "4" && table.rows()[3][1] == "10");
    table.erase_row(3);
    ASSERT(!table.selected_row());
    table.set_rows({});
    ASSERT((table.scroll_offset() == Point{}));
}

TEST(table_sort_comparator_exception_preserves_data_and_selection)
{
    auto table = DataTable{table_options()};
    table.select(1);
    ASSERT_THROWS(
        table.sort_rows(
            0, true, [](auto, auto) -> bool { throw std::runtime_error{"compare"}; }),
        std::runtime_error);
    ASSERT(table.selected_row() == 1 && table.rows()[1][0] == "🇺🇸");
}

TEST(table_and_scrollbars_synchronize_both_axes_data_and_viewport_changes)
{
    auto table = DataTable{table_options()};
    auto vertical = ScrollBar{};
    auto horizontal = ScrollBar{{.vertical = false}};
    arrange(table, {{}, {10, 4}});
    link(table, vertical);
    link(table, horizontal);
    ASSERT(vertical.content_length == 4 && vertical.viewport_length == 2);
    ASSERT(horizontal.content_length == 16 && horizontal.viewport_length == 10);
    vertical.set_position(100);
    horizontal.set_position(100);
    ASSERT((table.scroll_offset() == Point{6, 2}));
    arrange(table, {{}, {16, 10}});
    ASSERT((table.scroll_offset() == Point{}));
    ASSERT(vertical.position == 0 && vertical.viewport_length == 8);
    table.add_row({"last", "5"});
    ASSERT(vertical.content_length == 5);
    table.set_rows({});
    ASSERT(vertical.content_length == 0 && vertical.position == 0);
    int notifications = 0;
    table.on_scroll.connect([&](auto, auto, auto) { ++notifications; });
    table.refresh();
    ASSERT(notifications == 0);
}

TEST(table_callbacks_can_reenter_relocate_or_destroy_without_stale_activation)
{
    auto table = std::make_unique<DataTable>(table_options());
    arrange(*table, {{}, {16, 4}});
    int activations = 0;
    table->on_activate.connect([&](auto) { ++activations; });
    table->on_select.connect([&](auto) { table.reset(); });
    auto const response = table->handle(KeyEvent{.key = KeyCode::Enter});
    ASSERT(response.handled && !table && activations == 0);
    auto other = DataTable{table_options()};
    other.on_select.connect([&](auto row) {
        if (row == 0) { other.select(1); }
    });
    other.select(0);
    ASSERT(other.selected_row() == 1);
    auto source = DataTable{table_options()};
    auto destination = DataTable{};
    source.on_select.connect([&](auto) { destination = std::move(source); });
    source.select(2);
    ASSERT(destination.selected_row() == 2);
}

TEST(table_empty_tiny_hidden_header_and_width_policy_views_remain_valid)
{
    auto table = DataTable{table_options()};
    for (auto size : {Area{0, 0}, Area{1, 1}, Area{1, 2}, Area{2, 3}}) {
        arrange(table, {{}, size});
        auto frame = Frame{size};
        table.paint(Canvas{frame});
    }
    table.show_header = false;
    table.column_lines = false;
    table.refresh();
    arrange(table, {{}, {15, 2}});
    ASSERT(table.viewport_size().height == 2 && table.content_size().width == 15);
    auto overrides = std::array{esc::ScalarWidthOverride{U'…', 2}};
    auto frame = Frame{table.size(), {.overrides = overrides}};
    table.paint(Canvas{frame});
    ASSERT((frame[Point{5, 0}].text == U"…"));
    ASSERT((frame[Point{5, 0}].columns == 2));
    table.set_rows({});
    table.paint(Canvas{frame});
    table.set_data({}, {});
    table.paint(Canvas{frame});
}

TEST(textbox_scrollbar_link_tracks_viewport_limits_and_resize)
{
    auto box = TextBox{{.text = "a\nb\nc\nd"}};
    auto bar = ScrollBar{};
    arrange(box, {{}, {4, 2}});
    link(box, bar);
    bar.set_position(100);
    ASSERT(box.scroll_offset().y == 2 && bar.position == 2);
    arrange(box, {{}, {4, 8}});
    ASSERT(box.scroll_offset().y == 0 && bar.position == 0 && bar.viewport_length == 8);
    box.set_text("one");
    ASSERT(bar.content_length == 1);
}

TEST(table_scroll_links_follow_live_state_after_reentrant_callbacks)
{
    auto table = DataTable{table_options()};
    auto bar = ScrollBar{};
    arrange(table, {{}, {10, 3}});
    table.on_scroll.connect([&](Point at, Area content, Area) {
        if (at.y == 1 && content.height == 4) { table.set_rows({{"only", "1"}}); }
    });
    link(table, bar);
    table.set_scroll_offset({0, 1});
    ASSERT(table.rows().size() == 1 && bar.content_length == 1 && bar.position == 0);
    auto moved = std::move(table);
    moved.add_row({"next", "2"});
    bar.set_position(1);
    ASSERT(moved.scroll_offset().y == 1);
}
