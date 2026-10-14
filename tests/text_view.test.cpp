#include <zzz/test.hpp>

#include <limits>

#include <ox/ox.hpp>

using namespace ox;

TEST(text_view_scrolling_drawing_and_hits_share_grapheme_geometry)
{
    auto layout = TextLayout{};
    layout.update("aé界🙂b", 4, TextWrap::None);
    auto const scroll = text_scroll(layout, layout.text().size(), {4, 1});
    ASSERT((scroll == Point{4, 0}));
    ASSERT((text_position(layout, layout.text().size(), {4, 1}, Align::Left, scroll) ==
            Point{3, 0}));
    ASSERT(text_offset(layout, {0, 0}, {4, 1}, Align::Left, scroll) == 7);
    ASSERT(text_offset(layout, {1, 0}, {4, 1}, Align::Left, scroll) == 7);
    auto frame = Frame{{4, 1}};
    paint_text(Canvas{frame}, layout, Align::Left, scroll, {},
               {.traits = Trait::Inverse}, {7, 11});
    ASSERT((frame[Point{0, 0}].text == U"🙂"));
    ASSERT((frame[Point{1, 0}].columns == 0));
    ASSERT((frame[Point{1, 0}].style.traits.contains(Trait::Inverse)));
    ASSERT((frame[Point{2, 0}].text == U"b"));
    ASSERT((!frame[Point{2, 0}].style.traits.contains(Trait::Inverse)));
}

TEST(text_view_alignment_reserves_a_visible_end_caret_and_clamps_extreme_hits)
{
    auto layout = TextLayout{};
    layout.update("界x", 8, TextWrap::None);
    auto scroll = text_scroll(layout, 0, {8, 1}, Align::Right);
    ASSERT((text_position(layout, 0, {8, 1}, Align::Right, scroll) == Point{5, 0}));
    scroll = text_scroll(layout, layout.text().size(), {8, 1}, Align::Right, scroll);
    ASSERT((text_position(layout, layout.text().size(), {8, 1}, Align::Right, scroll) ==
            Point{7, 0}));
    ASSERT(text_offset(layout, {5, 0}, {8, 1}, Align::Right, scroll) == 0);
    ASSERT(
        text_offset(layout,
                    {std::numeric_limits<int>::min(), std::numeric_limits<int>::min()},
                    {8, 1}, Align::Right, scroll) == 0);
    ASSERT(
        text_offset(layout,
                    {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()},
                    {8, 1}, Align::Right, scroll) == layout.text().size());
    ASSERT_THROWS((text_scroll(layout, 0, {-1, 1})), std::invalid_argument);
}

TEST(cached_text_rows_clip_tabs_wide_clusters_and_selected_crlf)
{
    auto layout = TextLayout{};
    layout.update("\t界é\r\n", 8, TextWrap::None);
    auto frame = Frame{{6, 1}};
    put_text_line(Canvas{frame, {-2, 0}, {8, 1}}, {0, 0}, layout, 0, {},
                  {.traits = Trait::Inverse}, {0, layout.text().size()});
    ASSERT((frame[Point{0, 0}].text == U" "));
    ASSERT((frame[Point{2, 0}].text == U"界"));
    ASSERT((frame[Point{3, 0}].columns == 0));
    ASSERT((frame[Point{4, 0}].text == U"é"));
    for (auto const& cell : frame.row(0)) {
        ASSERT(cell.style.traits.contains(Trait::Inverse));
    }
    layout.update("界x", 3, TextWrap::None);
    frame.resize({3, 1});
    put_text_line(Canvas{frame}, {-1, 0}, layout, 0, {}, {});
    ASSERT((frame[Point{0, 0}].text == U" "));
    ASSERT((frame[Point{1, 0}].text == U"x"));
}

TEST(text_input_composition_updates_layout_scroll_and_streamed_paste)
{
    auto edit = TextEdit{"", false};
    auto layout = TextLayout{};
    auto scroll = Point{};
    auto const accept =
        std::function<bool(char32_t)>{[](char32_t c) { return c != U'x'; }};
    auto response = edit_text(edit, layout, scroll, {3, 1}, TextWrap::None, Align::Left,
                              KeyEvent{.key = U'x'}, accept);
    ASSERT(response.handled && !response.repaint && edit.text().empty());
    response = edit_text(edit, layout, scroll, {3, 1}, TextWrap::None, Align::Left,
                         esc::PasteChunk{"éx\r\n\t界", true, true}, accept);
    ASSERT(response.handled && response.repaint && edit.text() == "é界");
    ASSERT(layout.text() == edit.text());
    ASSERT((text_position(layout, edit.caret(), {3, 1}, Align::Left, scroll) ==
            Point{2, 0}));
    response = edit_text(edit, layout, scroll, {3, 1}, TextWrap::None, Align::Left,
                         esc::PasteChunk{"\xf0\x9f", true, false}, accept);
    ASSERT(response.handled && !response.repaint);
    response = edit_text(edit, layout, scroll, {3, 1}, TextWrap::None, Align::Left,
                         esc::PasteChunk{"\x99\x82", false, true}, accept);
    ASSERT(response.repaint && edit.text() == "é界🙂");
    response = edit_text(
        edit, layout, scroll, {3, 1}, TextWrap::None, Align::Left,
        KeyEvent{.key = KeyCode::Backspace, .action = KeyAction::Release}, accept);
    ASSERT(!response.handled && !response.repaint && edit.text() == "é界🙂");
}

TEST(text_widgets_share_unwrapped_scrolling_and_keep_placeholder_on_one_row)
{
    auto box = TextBox{{.text = "abc界d\nshort",
                        .wrap = TextWrap::None,
                        .focus_policy = FocusPolicy::Strong}};
    arrange(box, {{0, 0}, {4, 2}});
    box.handle(KeyEvent{.key = KeyCode::End});
    auto frame = Frame{box.size()};
    auto cursor = box.paint(Canvas{frame});
    ASSERT((cursor == Cursor{Point{3, 0}}));
    ASSERT((frame[Point{0, 0}].text == U"界"));
    box.handle(MousePress{{1, 0}, MouseButton::Left});
    ASSERT(box.editor.caret() == 3);
    auto line = LineEdit{{.text = "界x", .align = Align::Right}};
    arrange(line, {{0, 0}, {8, 1}});
    line.handle(KeyEvent{.key = KeyCode::End});
    frame.resize(line.size());
    ASSERT((line.paint(Canvas{frame}) == Cursor{Point{7, 0}}));
    line.set_text("");
    line.ghost_text = "first\nsecond";
    arrange(line, {{0, 0}, {8, 2}});
    frame.resize(line.size());
    line.paint(Canvas{frame});
    for (auto const& cell : frame.row(1)) {
        ASSERT(cell.text == U" ");
    }
}
