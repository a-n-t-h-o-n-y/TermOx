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

TEST(label_orientation_measures_graphemes_and_defaults_to_matching_axis_policies)
{
    auto horizontal = Label{{.text = "Aé界🙂"}};
    auto vertical = Label{{.text = "Aé界🙂", .vertical = true}};
    ASSERT((horizontal.measure({20, 20}) == Area{6, 1}));
    ASSERT((vertical.measure({20, 20}) == Area{2, 4}));
    ASSERT(horizontal.height_policy == SizePolicy::fixed(1));
    ASSERT(vertical.width_policy.flexibility == 0);
    ASSERT(vertical.height_policy == SizePolicy::flex());
    auto overrides = std::array{esc::ScalarWidthOverride{U'A', 2}};
    auto narrow = Label{{.text = "Ab", .vertical = true}};
    ASSERT((narrow.measure({20, 20}) == Area{1, 2}));
    ASSERT((narrow.measure({20, 20}, {.overrides = overrides}) == Area{2, 2}));
    ASSERT((vertical.measure({0, 0}) == Area{2, 4}));
    auto row = Row{Label{{.text = "界x", .vertical = true}}, Label{"rest"}};
    arrange(row, {{}, {12, 5}});
    ASSERT((std::get<0>(row.children).size() == Area{2, 5}));
    auto app = Application{row};
    app.dispatch(esc::Resize{{12, 5}});
    auto& label = std::get<0>(row.children);
    label.set_text("abc");
    app.step();
    ASSERT(label.size().width == 1);
    ASSERT(label.set_vertical(false));
    app.step();
    ASSERT(label.size().height == 1 && !label.vertical);
}

TEST(vertical_labels_align_along_rows_center_whole_graphemes_and_clip)
{
    auto label = Label{{.text = "é界🙂", .vertical = true, .align = Align::Start}};
    arrange(label, {{}, {3, 5}});
    auto frame = Frame{{3, 5}};
    label.paint(Canvas{frame});
    ASSERT((frame[Point{1, 0}].text == U"é"));
    ASSERT((frame[Point{0, 1}].text == U"界"));
    ASSERT((frame[Point{0, 2}].text == U"🙂"));
    ASSERT((frame[Point{1, 1}].columns == 0));
    label.align = Align::Center;
    label.paint(Canvas{frame});
    ASSERT((frame[Point{1, 1}].text == U"é"));
    ASSERT((frame[Point{0, 3}].text == U"🙂"));
    ASSERT((frame[Point{1, 0}].text == U" "));
    label.align = Align::End;
    label.paint(Canvas{frame});
    ASSERT((frame[Point{1, 2}].text == U"é"));
    ASSERT((frame[Point{0, 4}].text == U"🙂"));
    auto tiny = Frame{{1, 3}};
    label.paint(Canvas{tiny});
    ASSERT((tiny[Point{0, 2}].text == U"é"));
    ASSERT((tiny[Point{0, 1}].text == U" ")); // Wide glyph ends the fitting prefix.
    auto short_frame = Frame{{3, 2}};
    label.paint(Canvas{short_frame});
    ASSERT((short_frame[Point{1, 0}].text == U"é"));
    ASSERT((short_frame[Point{0, 1}].text == U"界"));
    label.set_text("");
    ASSERT((label.measure({3, 2}) == Area{0, 0}));
    label.paint(Canvas{short_frame});
    for (auto const& cell : short_frame.row(0)) ASSERT(cell.text == U" ");
}

TEST(label_orientation_is_explicit_in_tiny_views_and_refresh_preserves_custom_policies)
{
    auto label = Label{{.text = "ab", .vertical = true, .align = Align::End}};
    arrange(label, {{}, {1, 1}});
    auto cell = Frame{{1, 1}};
    label.paint(Canvas{cell});
    ASSERT(label.vertical);
    ASSERT((cell[Point{0, 0}].text == U"a"));
    ASSERT((label.measure({1, 1}) == Area{1, 2}));
    label.width_policy = SizePolicy::fixed(4);
    label.height_policy = SizePolicy::bounded(2, 8);
    label.vertical = false;
    label.refresh();
    ASSERT(label.width_policy == SizePolicy::fixed(4));
    ASSERT(label.height_policy == SizePolicy::bounded(2, 8));
    ASSERT((label.measure({1, 1}) == Area{2, 1}));
    ASSERT(!label.set_vertical(false));
    ASSERT(align_offset(Align::Start, 5, 2) == 0);
    ASSERT(align_offset(Align::Center, 5, 2) == 1);
    ASSERT(align_offset(Align::End, 5, 2) == 3);
    ASSERT(Align::Left == Align::Start && Align::Right == Align::End);
    for (auto size : {Area{0, 0}, Area{0, 3}, Area{3, 0}}) {
        auto empty = Frame{size};
        label.vertical = true;
        label.paint(Canvas{empty});
    }
}

TEST(label_measurement_and_painting_reject_invalid_text_in_both_directions)
{
    for (auto vertical : {false, true}) {
        auto label = Label{{.vertical = vertical}};
        for (auto text : {"a\nb", "a\tb", "a\x1b"}) {
            ASSERT_THROWS(label.set_text(text), std::invalid_argument);
            label.text = text;
            ASSERT_THROWS(label.measure({8, 8}), std::invalid_argument);
            auto frame = Frame{{8, 8}};
            ASSERT_THROWS(label.paint(Canvas{frame}), std::invalid_argument);
        }
        label.text.clear();
        ASSERT_THROWS(label.set_text("bad\xff"), esc::UnicodeError);
    }
}

TEST(shared_label_options_render_vertical_control_captions_and_decoration_alignment)
{
    auto button = Button{{.label = {.text = "ab", .vertical = true,
                                    .align = Align::End}}};
    ASSERT((button.measure({6, 6}) == Area{1, 2}));
    arrange(button, {{}, {3, 4}});
    auto frame = Frame{{3, 4}};
    button.paint(Canvas{frame});
    ASSERT((frame[Point{1, 2}].text == U"a"));
    ASSERT((frame[Point{1, 3}].text == U"b"));
    auto check = CheckBox{{.label = {.text = "ab", .vertical = true}}};
    ASSERT(check.measure({10, 10}).height == 2);
    auto line = Frame{{1, 4}};
    paint_divider(Canvas{line}, true, Lines::ascii(), {},
                  {.text = "ab", .align = Align::End});
    ASSERT((line[Point{0, 2}].text == U"a"));
    ASSERT((line[Point{0, 3}].text == U"b"));
}
