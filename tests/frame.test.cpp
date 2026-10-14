#include <zzz/test.hpp>

#include <array>
#include <limits>
#include <list>
#include <random>
#include <type_traits>

// These headers deliberately require no terminal session or event-loop header.
#include <ox/core/canvas.hpp>
#include <ox/put.hpp>

using namespace ox;

static_assert(std::same_as<decltype(std::declval<Frame&>()[Point{}]), Cell const&>);

namespace {
void check_frame(Frame const& frame)
{
    for (int y = 0; y < frame.size().height; ++y) {
        auto const row = frame.row(y);
        for (auto x = std::size_t{0}; x < row.size();) {
            auto const& cell = row[x];
            ASSERT(cell.columns > 0);
            ASSERT(cell.columns <= row.size() - x);
            ASSERT(esc::grapheme_width(cell.text, frame.width_policy()) ==
                   cell.columns);
            for (auto i = std::size_t{1}; i < cell.columns; ++i) {
                ASSERT(row[x + i].text.empty());
                ASSERT(row[x + i].columns == 0);
                ASSERT(row[x + i].style == cell.style);
            }
            x += cell.columns;
        }
    }
}

auto contents(Frame const& frame) -> std::vector<Cell>
{
    auto cells = std::vector<Cell>{};
    for (int y = 0; y < frame.size().height; ++y) {
        auto row = frame.row(y);
        cells.insert(cells.end(), row.begin(), row.end());
    }
    return cells;
}
}  // namespace

TEST(frame_dimensions_and_resize_have_defined_failure_and_reset_behavior)
{
    ASSERT_THROWS((Frame{{-1, 2}}), std::invalid_argument);
    ASSERT_THROWS((Frame{{2, -1}}), std::invalid_argument);
    ASSERT_THROWS(
        (Frame{{std::numeric_limits<int>::max(), std::numeric_limits<int>::max()}}),
        std::length_error);
    auto frame = Frame{{3, 2}};
    frame.write({1, 0}, U"界");
    auto const before = contents(frame);
    ASSERT_THROWS(frame.resize({-1, 2}), std::invalid_argument);
    ASSERT(contents(frame) == before);
    ASSERT((frame.size() == Area{3, 2}));
    frame.resize({3, 2});
    ASSERT((frame[Point{1, 0}] == Cell{}));
    frame.resize({1, 4});
    check_frame(frame);
    frame.resize({0, 3});
    ASSERT(frame.row(2).empty());
    frame.resize({3, 0});
    ASSERT_THROWS(frame.row(0), std::out_of_range);
}

TEST(frame_indexing_checks_each_axis_and_drawing_outside_is_a_noop)
{
    auto frame = Frame{{3, 2}};
    ASSERT_THROWS((frame[Point{-1, 1}]), std::out_of_range);
    ASSERT_THROWS((frame[Point{3, 0}]), std::out_of_range);
    ASSERT_THROWS((frame[Point{0, 2}]), std::out_of_range);
    auto before = contents(frame);
    ASSERT(!frame.write({-1, 0}, U"x"));
    ASSERT(!frame.write({3, 0}, U"x"));
    ASSERT(contents(frame) == before);
}

TEST(frame_stores_combining_and_emoji_graphemes_with_complete_footprints)
{
    auto frame = Frame{{8, 1}};
    ASSERT(frame.write({0, 0}, U"e\u0301", {.foreground = colors::cyan}));
    ASSERT(frame.write({1, 0}, U"界"));
    ASSERT(frame.write({3, 0}, U"👩\u200d💻"));
    ASSERT((frame[Point{0, 0}].text == U"e\u0301"));
    ASSERT((frame[Point{0, 0}].columns == 1));
    ASSERT((frame[Point{1, 0}].columns == 2));
    ASSERT((frame[Point{2, 0}].columns == 0));
    ASSERT((frame.lead_at({2, 0}) == Point{1, 0}));
    ASSERT((frame[Point{3, 0}].text == U"👩\u200d💻"));
    ASSERT((frame[Point{4, 0}].columns == 0));
    check_frame(frame);
}

TEST(overwriting_either_half_erases_the_complete_old_grapheme)
{
    auto frame = Frame{{5, 1}};
    frame.write({0, 0}, U"界");
    frame.write({2, 0}, U"🙂");
    frame.write({1, 0}, U"語");
    ASSERT((frame[Point{0, 0}].text == U" "));
    ASSERT((frame[Point{1, 0}].text == U"語"));
    ASSERT((frame[Point{2, 0}].columns == 0));
    ASSERT((frame[Point{3, 0}].text == U" "));
    frame.write({2, 0}, U"x");
    ASSERT((frame[Point{1, 0}].text == U" "));
    ASSERT((frame[Point{2, 0}].text == U"x"));
    frame.write({0, 0}, U"界");
    frame.write({0, 0}, U"a");
    ASSERT((frame[Point{1, 0}].text == U" "));
    check_frame(frame);
}

TEST(frame_rejects_controls_orphan_marks_and_multiple_graphemes_without_mutation)
{
    auto frame = Frame{{4, 1}};
    frame.write({0, 0}, U"界");
    auto before = contents(frame);
    ASSERT_THROWS(frame.write({0, 0}, U"ab"), std::invalid_argument);
    ASSERT_THROWS(frame.write({0, 0}, U"\n"), std::invalid_argument);
    ASSERT_THROWS(frame.write({0, 0}, U"\u0301"), std::invalid_argument);
    ASSERT_THROWS(frame.write({0, 0}, U""), std::invalid_argument);
    ASSERT_THROWS(frame.write({0, 0}, U"\x1b"), std::invalid_argument);
    ASSERT_THROWS(frame.write({-1, 0}, U"\n"), std::invalid_argument);
    ASSERT(contents(frame) == before);
    ASSERT(!frame.write({3, 0}, U"界"));
    ASSERT(contents(frame) == before);
}

TEST(frame_owns_width_overrides_and_copy_move_keep_independent_storage)
{
    auto overrides = std::array{esc::ScalarWidthOverride{U'a', 2}};
    auto frame = Frame{{6, 1}, esc::WidthPolicy{.overrides = overrides}};
    overrides[0].cells = 1;
    frame.write({0, 0}, U"a");
    ASSERT((frame[Point{0, 0}].columns == 2));
    auto copy = frame;
    frame.clear();
    copy.write({2, 0}, U"a");
    ASSERT((copy[Point{2, 0}].columns == 2));
    auto moved = std::move(copy);
    ASSERT((copy.size() == Area{0, 0}));
    moved.write({4, 0}, U"a");
    check_frame(moved);
    frame = moved;
    moved.clear();
    ASSERT((frame[Point{4, 0}].text == U"a"));
}

TEST(nested_canvases_cannot_escape_parent_clips)
{
    auto frame = Frame{{8, 4}};
    auto parent = Canvas{frame, {2, 1}, {3, 2}};
    auto child = parent.subcanvas({-2, -1}, {8, 4});
    ASSERT((child.visible() == Rect{{2, 1}, {3, 2}}));
    fill(child, U'#');
    for (int y = 0; y < 4; ++y) {
        for (int x = 0; x < 8; ++x) {
            ASSERT((frame[Point{x, y}].text ==
                    ((x >= 2 && x < 5 && y >= 1 && y < 3) ? U"#" : U" ")));
        }
    }
    auto grandchild = child.subcanvas({-3, -3}, {20, 20});
    fill(grandchild, U'*');
    ASSERT((frame[Point{2, 1}].text == U"*"));
    ASSERT((frame[Point{1, 1}].text == U" "));
    ASSERT_THROWS((parent[Point{-1, 0}]), std::out_of_range);
}

TEST(negative_origins_and_offsets_clip_text_without_repositioning_it)
{
    auto frame = Frame{{5, 2}};
    auto canvas = Canvas{frame, {-2, -1}, {7, 3}};
    ASSERT((canvas.visible() == Rect{{2, 1}, {5, 2}}));
    put(canvas, {0, 1}, U"abcdefg");
    ASSERT((frame[Point{0, 0}].text == U"c"));
    ASSERT((frame[Point{4, 0}].text == U"g"));
    put(Canvas{frame}, {-2, 1}, U"abcdef");
    ASSERT((frame[Point{0, 1}].text == U"c"));
    auto const before = contents(frame);
    fill(Canvas{frame, {100, 100}, {2, 2}}, U'x');
    ASSERT(contents(frame) == before);
}

TEST(views_keep_geometry_and_safely_reclip_after_resize)
{
    auto frame = Frame{{5, 3}};
    auto view = Canvas{frame, {1, 1}, {4, 2}};
    frame.resize({2, 2});
    ASSERT((view.size() == Area{4, 2}));
    ASSERT((view.visible() == Rect{{0, 0}, {1, 1}}));
    fill(view, U'x');
    ASSERT((frame[Point{1, 1}].text == U"x"));
    frame.resize({0, 0});
    ASSERT(!view.contains({0, 0}));
    ASSERT(!view.write({0, 0}, U"x"));
    frame.resize({7, 5});
    ASSERT((view.visible() == Rect{{0, 0}, {4, 2}}));
}

TEST(clipped_views_never_overwrite_or_restyle_half_a_grapheme)
{
    auto frame = Frame{{5, 1}};
    frame.write({1, 0}, U"界", {.foreground = colors::cyan});
    auto half = Canvas{frame, {2, 0}, {2, 1}};
    auto before = contents(frame);
    ASSERT(!half.write({0, 0}, U"x"));
    ASSERT(!half.restyle({0, 0}, {.foreground = colors::red}));
    ASSERT(contents(frame) == before);
    fill(half, U'*');
    ASSERT((frame[Point{1, 0}].text == U"界"));
    ASSERT((frame[Point{2, 0}].columns == 0));
    ASSERT((frame[Point{3, 0}].text == U"*"));
    auto whole = Canvas{frame, {1, 0}, {2, 1}};
    fill(whole, fg(colors::red));
    ASSERT((frame[Point{1, 0}].style.foreground == Color{colors::red}));
    ASSERT((frame[Point{2, 0}].style.foreground == Color{colors::red}));
    check_frame(frame);
}

TEST(text_clipping_preserves_graphemes_and_unicode_column_positions)
{
    auto frame = Frame{{5, 1}};
    auto canvas = Canvas{frame};
    put(canvas, {-1, 0}, U"界a🙂b");
    ASSERT((frame[Point{0, 0}].text == U" "));
    ASSERT((frame[Point{1, 0}].text == U"a"));
    ASSERT((frame[Point{2, 0}].text == U"🙂"));
    ASSERT((frame[Point{3, 0}].columns == 0));
    ASSERT((frame[Point{4, 0}].text == U"b"));
    frame.clear();
    put(canvas, {3, 0}, U"ae\u0301界");
    ASSERT((frame[Point{4, 0}].text == U"e\u0301"));
    check_frame(frame);
}

TEST(drawing_validates_complete_text_and_supports_noncontiguous_glyph_ranges)
{
    auto frame = Frame{{5, 1}};
    auto canvas = Canvas{frame};
    auto before = contents(frame);
    ASSERT_THROWS(put(canvas, {0, 0}, std::string_view{"a\xff", 2}), esc::UnicodeError);
    ASSERT_THROWS(put(canvas, {0, 0}, U"a\n"), std::invalid_argument);
    auto mixed = esc::glyphs(U"e\u0301");
    mixed[1].style.foreground = colors::red;
    ASSERT_THROWS(put(canvas, {0, 0}, mixed), std::invalid_argument);
    ASSERT(contents(frame) == before);
    auto glyphs = std::list<Glyph>{{U'e'}, {U'\u0301'}, {U'界'}};
    put(canvas, {0, 0}, glyphs);
    ASSERT((frame[Point{0, 0}].text == U"e\u0301"));
    ASSERT((frame[Point{1, 0}].text == U"界"));
    check_frame(frame);
}

TEST(lines_boxes_and_fills_share_clipping_and_keep_original_geometry)
{
    auto frame = Frame{{4, 3}};
    auto canvas = Canvas{frame};
    put(canvas, {-2, 0}, shape::HLine{U'-'}, 4);
    ASSERT((frame[Point{0, 0}].text == U"-"));
    ASSERT((frame[Point{1, 0}].text == U"-"));
    ASSERT((frame[Point{2, 0}].text == U" "));
    put(canvas, {3, -2}, shape::VLine{U'|'}, 4);
    ASSERT((frame[Point{3, 1}].text == U"|"));
    frame.clear();
    put(canvas, {-1, -1}, shape::Box::ascii(), {6, 5});
    ASSERT((frame[Point{0, 0}].text == U" "));
    ASSERT((frame[Point{3, 2}].text == U" "));
    put(canvas, {0, 0}, shape::Box::ascii(), {1, 1});
    ASSERT((frame[Point{0, 0}].text == U"+"));
    frame.clear();
    put(canvas, {0, 0}, shape::Box::ascii(), {0, 3});
    ASSERT((frame[Point{0, 0}] == Cell{}));
    fill(canvas, U'界' | bg(colors::blue));
    ASSERT((frame[Point{0, 0}].columns == 2));
    ASSERT((frame[Point{2, 0}].columns == 2));
    check_frame(frame);
}

TEST(extreme_coordinates_are_checked_and_clipped_without_signed_overflow)
{
    auto frame = Frame{{4, 2}};
    auto canvas = Canvas{frame};
    auto const maximum = std::numeric_limits<int>::max();
    auto const minimum = std::numeric_limits<int>::min();
    put(canvas, {maximum, maximum}, shape::Box::ascii(), {maximum, maximum});
    put(canvas, {minimum, 0}, shape::HLine{U'-'}, maximum);
    ASSERT((frame[Point{0, 0}] == Cell{}));
    auto extreme = Canvas{frame, {maximum, maximum}, {1, 1}};
    ASSERT_THROWS(extreme.subcanvas({1, 0}, {1, 1}), std::overflow_error);
    ASSERT_THROWS((Canvas{frame, {0, 0}, {-1, 0}}), std::invalid_argument);
    ASSERT_THROWS(put(canvas, {0, 0}, shape::HLine{}, -1), std::invalid_argument);
}

TEST(random_overlapping_writes_and_styles_keep_valid_rows)
{
    auto frame = Frame{{9, 3}};
    auto canvas = Canvas{frame};
    auto random = std::mt19937{1234};
    auto coordinate = std::uniform_int_distribution<int>{-2, 10};
    auto symbols =
        std::array<std::u32string_view, 4>{U"a", U"界", U"e\u0301", U"👩\u200d💻"};
    for (int i = 0; i < 300; ++i) {
        auto at = Point{coordinate(random), coordinate(random) % 5};
        canvas.write(at, symbols[random() % symbols.size()],
                     {.foreground = colors::blue});
        fill(canvas.subcanvas({1, 0}, {6, 3}), fg(colors::red));
        check_frame(frame);
    }
}

TEST(styling_an_extremely_offset_view_preserves_wide_boundary_cells)
{
    auto frame = Frame{{2, 1}};
    frame.write({0, 0}, U"界");
    auto const maximum = std::numeric_limits<int>::max();
    auto view = Canvas{frame, {1 - maximum, 0}, {maximum, 1}};
    fill(view, fg(colors::red));
    ASSERT((frame[Point{0, 0}].style.foreground == Color{default_color}));
    check_frame(frame);
}

TEST(blank_width_policy_and_invalid_shape_symbols_are_rejected)
{
    auto overrides = std::array{esc::ScalarWidthOverride{U' ', 2}};
    ASSERT_THROWS((Frame{{2, 1}, esc::WidthPolicy{.overrides = overrides}}),
                  std::invalid_argument);
    auto frame = Frame{{4, 2}};
    auto canvas = Canvas{frame};
    auto box = shape::Box::ascii();
    box.corners[3] = U'界';
    auto before = contents(frame);
    ASSERT_THROWS(put(canvas, box), std::invalid_argument);
    ASSERT_THROWS(put(canvas, {99, 99}, shape::HLine{U'界'}, 3), std::invalid_argument);
    ASSERT(contents(frame) == before);
}

TEST(cell_equality_includes_text_style_and_columns)
{
    auto const texts = std::array<std::u32string, 5>{U"", U" ", U"x", U"界", U"é"};
    for (auto const& text : texts) {
        auto cell = Cell{text, {.traits = Trait::Bold}, 1};
        ASSERT(cell == cell);
        for (auto const& other : texts) {
            ASSERT((cell == Cell{other, cell.style, 1}) == (text == other));
        }
        ASSERT((cell != Cell{text, {}, 1}));
        ASSERT((cell != Cell{text, cell.style, 0}));
    }
}

TEST(frame_clear_resets_long_clusters_continuations_and_blank_styles)
{
    auto frame = Frame{{8, 2}};
    frame.write({0, 0}, U"é́́́́́́́́́");
    frame.write({2, 0}, U"界");
    auto const style = Style{.foreground = colors::red, .traits = Trait::Bold};
    frame.clear(style);
    for (int y = 0; y < frame.size().height; ++y) {
        for (auto const& cell : frame.row(y)) {
            ASSERT((cell == Cell{U" ", style, 1}));
        }
    }
    frame.clear();
    for (int y = 0; y < frame.size().height; ++y) {
        for (auto const& cell : frame.row(y)) {
            ASSERT(cell == Cell{});
        }
    }
    check_frame(frame);
}
