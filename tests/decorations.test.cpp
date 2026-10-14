#include <zzz/test.hpp>

#include <type_traits>

#include <ox/ox.hpp>

using namespace ox;

TEST(border_configuration_is_copyable_and_tiny_interiors_are_explicit)
{
    static_assert(std::is_copy_constructible_v<Border>);
    auto border = Border{.label = {.text = "Title"}};
    auto copy = border;
    ASSERT(copy.label.text == "Title");
    ASSERT((border_content_rect({0, 0}) == Rect{{0, 0}, {0, 0}}));
    ASSERT((border_content_rect({1, 1}) == Rect{{1, 1}, {0, 0}}));
    ASSERT((border_content_rect({2, 2}) == Rect{{1, 1}, {0, 0}}));
    ASSERT((border_content_rect({8, 5}) == Rect{{1, 1}, {6, 3}}));
    ASSERT_THROWS(border_content_rect({-1, 0}), std::invalid_argument);
    auto widget = Widget{} | border;
    for (auto size : {Area{0, 0}, Area{1, 0}, Area{1, 1}, Area{2, 2}, Area{8, 5}}) {
        arrange(widget, {{}, size});
        ASSERT(widget.child.bounds() == border_content_rect(size));
        auto frame = Frame{size};
        widget.paint(Canvas{frame});
    }
}

TEST(border_title_shares_unicode_alignment_without_erasing_the_line_or_interior)
{
    auto frame = Frame{{10, 4}};
    fill(Canvas{frame}, U'x' | Style{});
    paint_border(Canvas{frame},
                 Border{.label = {.text = "界é",
                                  .align = Align::Center,
                                  .style = {.foreground = colors::red}}});
    ASSERT((frame[Point{3, 0}].text == U"界"));
    ASSERT((frame[Point{5, 0}].text == U"é"));
    ASSERT((frame[Point{1, 0}].text == U"─"));
    ASSERT((frame[Point{8, 0}].text == U"─"));
    ASSERT((frame[Point{1, 1}].text == U"x"));
    ASSERT((frame[Point{5, 0}].style.foreground == Color{colors::red}));
    auto clipped = Frame{{5, 2}};
    paint_border(Canvas{clipped, {-4, 0}, {10, 4}}, Border{.label = {.text = "界é"}});
    ASSERT((clipped[Point{0, 0}].text == U"─"));
    ASSERT((clipped[Point{1, 0}].text == U"é"));
}

TEST(label_drawing_validates_complete_text_and_never_splits_wide_clusters)
{
    auto frame = Frame{{3, 1}};
    fill(Canvas{frame}, U'-' | Style{});
    put_label(Canvas{frame}, "aé界", Align::Left, {});
    ASSERT((frame[Point{0, 0}].text == U"a"));
    ASSERT((frame[Point{1, 0}].text == U"é"));
    ASSERT((frame[Point{2, 0}].text == U"-"));
    ASSERT_THROWS(put_label(Canvas{frame}, "long\n", Align::Left, {}),
                  std::invalid_argument);
    ASSERT_THROWS(put_label(Canvas{frame}, "long\xff", Align::Left, {}),
                  esc::UnicodeError);
    auto overrides = std::array{esc::ScalarWidthOverride{U'a', 2}};
    auto custom = Frame{{5, 1}, {.overrides = overrides}};
    put_label(Canvas{custom}, "a界", Align::Right, {});
    ASSERT((custom[Point{1, 0}].text == U"a"));
    ASSERT((custom[Point{1, 0}].columns == 2));
    ASSERT((custom[Point{3, 0}].text == U"界"));
}

TEST(divider_orientation_is_explicit_including_one_cell_views)
{
    auto horizontal = Divider{{.lines = Lines::ascii()}};
    auto vertical = Divider{{.vertical = true, .lines = Lines::ascii()}};
    arrange(horizontal, {{}, {1, 1}});
    arrange(vertical, {{}, {1, 1}});
    auto frame = Frame{{1, 1}};
    horizontal.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"-"));
    vertical.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"|"));
    frame.resize({5, 5});
    horizontal.paint(Canvas{frame});
    ASSERT((frame[Point{0, 2}].text == U"-"));
    ASSERT((frame[Point{0, 0}].text == U" "));
    vertical.paint(Canvas{frame});
    ASSERT((frame[Point{2, 0}].text == U"|"));
    ASSERT((frame[Point{0, 2}].text == U" "));
}

TEST(divider_labels_overlay_horizontal_and_vertical_lines_with_complete_graphemes)
{
    auto frame = Frame{{8, 1}};
    paint_divider(Canvas{frame}, false, Lines::light(), {}, {.text = "go"});
    ASSERT((frame[Point{2, 0}].text == U"─"));
    ASSERT((frame[Point{3, 0}].text == U"g"));
    ASSERT((frame[Point{5, 0}].text == U"─"));
    frame.resize({3, 7});
    paint_divider(Canvas{frame}, true, Lines::light(), {}, {.text = "é界🙂"});
    ASSERT((frame[Point{1, 1}].text == U"│"));
    ASSERT((frame[Point{1, 2}].text == U"é"));
    ASSERT((frame[Point{0, 3}].text == U"界"));
    ASSERT((frame[Point{0, 4}].text == U"🙂"));
    ASSERT((frame[Point{1, 5}].text == U"│"));
    paint_divider(Canvas{frame}, true, Lines::light(), {},
                  {.text = "ab", .align = Align::Right});
    ASSERT((frame[Point{1, 5}].text == U"a"));
    ASSERT((frame[Point{1, 6}].text == U"b"));
}

TEST(label_paint_clears_previous_text_while_put_label_only_overlays_its_prefix)
{
    auto frame = Frame{{6, 1}};
    paint_label(Canvas{frame}, "abcdef", Align::Left, {});
    put_label(Canvas{frame}, "x", Align::Left, {});
    ASSERT((frame[Point{0, 0}].text == U"x"));
    ASSERT((frame[Point{1, 0}].text == U"b"));
    paint_label(Canvas{frame}, "", Align::Left, {});
    for (auto const& cell : frame.row(0)) {
        ASSERT(cell.text == U" ");
    }
}
