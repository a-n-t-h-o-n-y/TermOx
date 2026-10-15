#include <cmath>
#include <ox/ox.hpp>
#include <zzz/test.hpp>
using namespace ox;
namespace {
auto glyphs(Frame const& f, bool vertical = false) -> std::u32string
{
    auto result = std::u32string{};
    for (int i = 0; i < (vertical ? f.size().height : f.size().width); ++i)
        result += f[vertical ? Point{0, i} : Point{i, 0}].text;
    return result;
}
}  // namespace
TEST(rule_motion_uses_whole_cell_color_boundaries_and_uniform_strokes)
{
    auto frame = Frame{{6, 1}};
    auto active = Style{.foreground = colors::red};
    auto track = Style{.foreground = colors::blue};
    paint_rule(Canvas{frame}, 0, 3, active, track);
    ASSERT(glyphs(frame) == U"╶────╴");
    ASSERT((frame[Point{2, 0}].style == active));
    ASSERT((frame[Point{3, 0}].style == track));
    paint_rule(Canvas{frame}, .49, 3, active, track);
    ASSERT((frame[Point{0, 0}].style == active));
    paint_rule(Canvas{frame}, .5, 3, active, track);
    ASSERT((frame[Point{0, 0}].style == track));
    ASSERT((frame[Point{1, 0}].style == active));
    ASSERT(glyphs(frame) == U"╶────╴");
    paint_rule(Canvas{frame}, 1, 3, active, track);
    ASSERT(glyphs(frame) == U"╶────╴");
    paint_rule(Canvas{frame}, 0, 0, active, track);
    ASSERT(glyphs(frame) == U"╶────╴");
}
TEST(rule_vertical_weights_ascii_and_tiny_views)
{
    auto frame = Frame{{1, 5}};
    paint_rule(Canvas{frame}, 1, 3, {}, {}, true);
    ASSERT(glyphs(frame, true) == U"╷│││╵");
    paint_rule(Canvas{frame}, 1, 3, {}, {}, true, true);
    ASSERT(glyphs(frame, true) == U"|||||");
    auto single = Frame{{1, 1}};
    paint_rule(Canvas{single}, 0, 1, {}, {});
    ASSERT(glyphs(single) == U"─");
    paint_rule(Canvas{single}, 0, 0, {}, {});
    ASSERT(glyphs(single) == U"─");
    auto empty = Frame{};
    paint_rule(Canvas{empty}, 0, 0, {}, {});
    ASSERT_THROWS(paint_rule(Canvas{frame}, NAN, 1, {}, {}), std::invalid_argument);
    ASSERT_THROWS(paint_rule(Canvas{frame}, 0, -1, {}, {}), std::invalid_argument);
}
TEST(rule_clipping_keeps_geometry_and_does_not_touch_other_rows)
{
    auto frame = Frame{{5, 2}};
    fill(Canvas{frame}, U'x');
    auto view = Canvas{frame}.subcanvas({-1, 0}, {8, 1});
    paint_rule(view, 0, 4, {}, {});
    ASSERT(glyphs(frame) == U"─────");
    for (int x = 0; x < 5; ++x)
        ASSERT((frame[Point{x, 1}].text == U"x"));
}
TEST(progress_tracks_keep_sub_quantum_fill_visible_and_use_vertical_strokes)
{
    auto frame = Frame{{4, 1}};
    paint_meter(Canvas{frame}, .001, {}, {});
    ASSERT(glyphs(frame) == U"╶──╴");
    auto vertical = Frame{{1, 4}};
    paint_meter(Canvas{vertical}, 0, {}, {}, true);
    ASSERT(glyphs(vertical, true) == U"╷││╵");
    paint_meter(Canvas{vertical}, 0, {}, {}, true, true);
    ASSERT(glyphs(vertical, true) == U"||||");
}
TEST(themed_table_header_connects_to_column_rules_in_unicode_and_ascii)
{
    auto table = DataTable{
        {.columns = {{"A", SizePolicy::fixed(5)}, {"B", SizePolicy::fixed(5)}},
         .rows = {{"a", "b"}}}};
    auto app = Application{table};
    auto theme = Theme::dark();
    app.set_theme(theme);
    app.dispatch(esc::Resize{{11, 4}});
    app.paint();
    ASSERT((app.loop().screen[Point{5, 1}].text == U"┼"));
    ASSERT((app.loop().screen[Point{5, 0}].text == U"╷"));
    app.dispatch(esc::Resize{{11, 2}});
    app.paint();
    ASSERT((app.loop().screen[Point{5, 1}].text == U"┴"));
    theme.ascii = true;
    app.set_theme(theme);
    app.paint();
    ASSERT((app.loop().screen[Point{5, 1}].text == U"+"));
}
TEST(themed_menu_separator_fills_its_view_with_a_capped_rule)
{
    auto menu = Menu{{{.command = command({.text = "First"})},
                      {},
                      {.command = command({.text = "Last"})}}};
    auto app = Application{menu};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{20, 3}});
    app.paint();
    ASSERT((app.loop().screen[Point{1, 1}].text == U"╶"));
    ASSERT((app.loop().screen[Point{18, 1}].text == U"╴"));
}
TEST(themed_slider_and_divider_use_capped_connected_strokes)
{
    auto slider = Slider{{.value = 50}};
    auto app = Application{slider};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{5, 1}});
    app.paint();
    ASSERT(glyphs(app.loop().screen) == U"╶─●─╴");
    slider.vertical = true;
    app.dispatch(esc::Resize{{1, 5}});
    app.paint();
    ASSERT(glyphs(app.loop().screen, true) == U"╷│●│╵");
    auto divider = Divider{};
    auto other = Application{divider};
    other.set_theme(Theme::dark());
    other.dispatch(esc::Resize{{5, 1}});
    other.paint();
    ASSERT(glyphs(other.loop().screen) == U"╶───╴");
    divider.lines = Lines::dashed();
    other.request_repaint();
    other.paint();
    ASSERT(glyphs(other.loop().screen) == U"╌╌╌╌╌");
}
TEST(themed_scrollbar_uses_a_whole_cell_thumb_position)
{
    auto bar = ScrollBar{{.content_length = 100,
                          .viewport_length = 10,
                          .position = 5,
                          .vertical = false}};
    auto app = Application{bar};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{10, 1}});
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].text == U"╶"));
    ASSERT((app.loop().screen[Point{1, 0}].text == U"─"));
    ASSERT((app.loop().screen[Point{0, 0}].style.foreground ==
            app.theme().palette.border));
    ASSERT((app.loop().screen[Point{1, 0}].style.foreground ==
            app.theme().palette.accent));
    bar.set_position(0);
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].text == U"╶"));
}
TEST(themed_split_divider_has_vertical_endcaps)
{
    auto split = SplitPane{Label{"A"}, Label{"B"}};
    auto app = Application{split};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{11, 4}});
    app.paint();
    auto x = split.first.size().width;
    ASSERT((app.loop().screen[Point{x, 0}].text == U"╷"));
    ASSERT((app.loop().screen[Point{x, 3}].text == U"╵"));
}
TEST(themed_tree_connectors_follow_siblings_selection_scrolling_and_ascii)
{
    auto tree = TreeView{
        {.nodes = {{"Root", true, true, {{"A", true, true, {{"Leaf"}}}, {"B"}}}}}};
    auto app = Application{tree};
    auto theme = Theme::dark();
    app.set_theme(theme);
    app.dispatch(esc::Resize{{20, 6}});
    app.paint();
    ASSERT((app.loop().screen[Point{2, 1}].text == U"├"));
    ASSERT((app.loop().screen[Point{2, 2}].text == U"│"));
    ASSERT((app.loop().screen[Point{4, 2}].text == U"└"));
    ASSERT((app.loop().screen[Point{2, 3}].text == U"└"));
    tree.select({0, 0, 0});
    app.paint();
    ASSERT(
        (app.loop().screen[Point{2, 2}].style.background == theme.palette.selection));
    app.dispatch(esc::Resize{{8, 6}});
    tree.set_scroll_offset({1, 0});
    app.paint();
    ASSERT((app.loop().screen[Point{3, 2}].text == U"└"));
    ASSERT((app.loop().screen[Point{1, 2}].text == U" "));
    theme.ascii = true;
    app.set_theme(theme);
    tree.set_scroll_offset({0, 0});
    app.paint();
    ASSERT((app.loop().screen[Point{2, 0}].text == U"v"));
    ASSERT((app.loop().screen[Point{4, 2}].text == U"`"));
    ASSERT(tree.current_path() == std::vector<std::size_t>({0, 0, 0}));
}
TEST(short_fractional_rules_keep_a_visible_active_segment)
{
    auto frame = Frame{{5, 1}};
    auto active = Style{.foreground = colors::red};
    for (auto length : {1., 1.1, 1.25, 1.5, 1.75, 2.})
        for (auto start : {0., .25, .5, .75, 1.}) {
            paint_rule(Canvas{frame}, start, length, active, {});
            bool visible = false;
            for (int x = 0; x < 5; ++x)
                visible |= frame[Point{x, 0}].style == active;
            ASSERT(visible);
        }
}
