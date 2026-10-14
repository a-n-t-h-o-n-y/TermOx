#include <zzz/test.hpp>

#include <algorithm>
#include <array>
#include <limits>

#include <ox/ox.hpp>

using namespace ox;

TEST(line_edit_edits_unicode_scalars_and_repeated_keys)
{
    auto edit = LineEdit{{.text = "aé日"}};
    arrange(edit, {edit.at(), {8, 1}});
    edit.handle(KeyEvent{.key = KeyCode::End});
    edit.handle(KeyEvent{.key = KeyCode::Backspace, .action = KeyAction::Repeat});
    ASSERT((edit.text() == "aé"));
    edit.handle(KeyEvent{.key = KeyCode::ArrowLeft});
    edit.handle(KeyEvent{.text = U"界🙂"});
    ASSERT((edit.text() == "a界🙂é"));
    edit.handle(KeyEvent{.key = KeyCode::Delete});
    ASSERT((edit.text() == "a界🙂"));
    edit.handle(KeyEvent{.key = U'x', .modifiers = {.ctrl = true}});
    ASSERT((edit.text() == "a界🙂"));
    auto buffer = Frame{{8, 1}};
    edit.paint(Canvas{buffer, {0, 0}, {8, 1}});
    ASSERT((buffer[Point{1, 0}].text == U"界"));
    ASSERT((buffer[Point{2, 0}].columns == 0));
    ASSERT((buffer[Point{3, 0}].text == U"🙂"));
}

TEST(line_edit_buffers_split_paste_and_applies_scalar_validation)
{
    auto edit = LineEdit{{.validator = [](char32_t c) { return c != U'x'; }}};
    arrange(edit, {edit.at(), {8, 1}});
    edit.handle(esc::PasteChunk{"\xc3", true, false});
    ASSERT((edit.text().empty()));
    edit.handle(esc::PasteChunk{"\xa9x\n", false, true});
    ASSERT((edit.text() == "é"));
    edit.handle(esc::PasteChunk{"discard", true, true, true});
    ASSERT((edit.text() == "é"));
}

TEST(text_box_handles_associated_text_and_multiline_paste)
{
    auto box = TextBox{{.text = "é日", .focus_policy = FocusPolicy::Strong}};
    arrange(box, {box.at(), {10, 4}});
    box.handle(KeyEvent{.key = KeyCode::End});
    box.handle(KeyEvent{.key = KeyCode::Backspace});
    ASSERT((box.text() == "é"));
    box.handle(KeyEvent{.text = U"界🙂"});
    ASSERT((box.text() == "é界🙂"));
    box.handle(esc::PasteChunk{"\n\xc3", true, false});
    ASSERT((box.text() == "é界🙂"));
    box.handle(esc::PasteChunk{"\xa9", false, true});
    ASSERT((box.text() == "é界🙂\né"));
    box.handle(esc::PasteChunk{"discard", true, true, true});
    ASSERT((box.text() == "é界🙂\né"));
}

TEST(button_handles_repeats_and_unidentified_mouse_release)
{
    auto button = Button{};
    int presses = 0;
    int releases = 0;
    button.on_press.connect([&] { ++presses; });
    button.on_release.connect([&] { ++releases; });
    button.handle(KeyEvent{.key = KeyCode::Enter, .action = KeyAction::Repeat});
    button.handle(MousePress{{0, 0}, MouseButton::Left});
    button.handle(MouseRelease{{0, 0}, std::nullopt});
    ASSERT((presses == 2));
    ASSERT((releases == 1));
}

TEST(line_edit_enter_preserves_changes_from_the_callback)
{
    auto edit = LineEdit{{.text = "submitted"}};
    edit.on_enter.connect([&](std::string const&) { edit.set_text(""); });
    edit.handle(KeyEvent{.key = KeyCode::Enter});
    ASSERT(edit.text().empty());
}

TEST(data_table_uses_signed_wheel_steps_and_clamps_scroll)
{
    auto table = DataTable{};
    table.add_column("Items");
    for (int i = 0; i < 8; ++i) {
        table.add_row({std::to_string(i)});
    }
    table.handle(MouseWheel{{0, 0}, 0, 3});
    ASSERT(table.offset == 3);
    table.handle(MouseWheel{{0, 0}, 0, -2});
    ASSERT(table.offset == 1);
    table.handle(MouseWheel{{0, 0}, 0, 20});
    ASSERT(table.offset == 7);
    table.handle(MouseWheel{{0, 0}, 0, -20});
    ASSERT(table.offset == 0);
    table.handle(MouseWheel{{0, 0}, 2, 0});
    ASSERT(table.offset == 0);
}

TEST(text_painting_never_splits_a_combining_cluster_at_the_viewport_edge)
{
    auto editor = LineEdit{{.text = "ae\xcc\x81"}};
    arrange(editor, {editor.at(), {2, 1}});
    editor.handle(KeyEvent{.key = KeyCode::End});
    auto frame = Frame{{2, 1}};
    auto const cursor = editor.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"e\u0301"));
    ASSERT((cursor == Cursor{Point{1, 0}}));

    auto box = TextBox{{.text = "e\xcc\x81"
                                "e\xcc\x81"}};
    arrange(box, {box.at(), {1, 2}});
    frame.resize({1, 2});
    box.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"e\u0301"));
    ASSERT((frame[Point{0, 1}].text == U"e\u0301"));
}

TEST(text_widgets_delete_combining_and_emoji_graphemes_as_units)
{
    auto line = LineEdit{{.text = "é👩‍💻"}};
    arrange(line, {line.at(), {10, 1}});
    line.handle(KeyEvent{.key = KeyCode::End});
    line.handle(KeyEvent{.key = KeyCode::Backspace});
    ASSERT(line.text() == "é");
    line.handle(KeyEvent{.key = KeyCode::Backspace});
    ASSERT(line.text().empty());
    auto box = TextBox{{.text = "a🇺🇸é"}};
    arrange(box, {box.at(), {10, 3}});
    box.handle(KeyEvent{.key = KeyCode::End});
    box.handle(KeyEvent{.key = KeyCode::ArrowLeft});
    box.handle(KeyEvent{.key = KeyCode::Delete});
    ASSERT(box.text() == "a🇺🇸");
    box.handle(KeyEvent{.key = KeyCode::Backspace});
    ASSERT(box.text() == "a");
}

TEST(line_edit_selection_and_mouse_hits_use_terminal_columns)
{
    auto line = LineEdit{{.text = "a界b"}};
    arrange(line, {line.at(), {8, 1}});
    line.handle(MousePress{{2, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 1);
    line.handle(MousePress{{3, 0}, MouseButton::Left, {.shift = true}});
    ASSERT((line.editor.selection() == std::pair<std::size_t, std::size_t>{1, 4}));
    auto frame = Frame{line.size()};
    line.paint(Canvas{frame});
    ASSERT((frame[Point{1, 0}].style.traits.contains(Trait::Inverse)));
    ASSERT((frame[Point{2, 0}].style.traits.contains(Trait::Inverse)));
    ASSERT((!frame[Point{3, 0}].style.traits.contains(Trait::Inverse)));
    line.handle(KeyEvent{.text = U"🙂"});
    ASSERT(line.text() == "a🙂b");
    line.handle(KeyEvent{.key = U'z', .modifiers = {.ctrl = true}});
    ASSERT(line.text() == "a界b");
}

TEST(line_edit_alignment_scrolling_and_hits_share_the_same_geometry)
{
    auto line = LineEdit{{.text = "界x", .align = Align::Right}};
    arrange(line, {line.at(), {8, 1}});
    auto frame = Frame{line.size()};
    line.paint(Canvas{frame});
    ASSERT((frame[Point{5, 0}].text == U"界"));
    line.handle(MousePress{{6, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 0);
    line.handle(MousePress{{7, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 3);
    line.set_text("aé界🙂b");
    arrange(line, {line.at(), {4, 1}});
    frame.resize(line.size());
    line.handle(KeyEvent{.key = KeyCode::End});
    auto const cursor = line.paint(Canvas{frame});
    ASSERT(cursor && cursor->x < 4);
    line.handle(MousePress{{0, 0}, MouseButton::Left});
    ASSERT(std::ranges::find(line.editor.boundaries(), line.editor.caret()) !=
           line.editor.boundaries().end());
}

TEST(text_box_wrapped_navigation_uses_preferred_columns_before_first_paint)
{
    auto box = TextBox{{.text = "abcd\nx\n界abc"}};
    arrange(box, {box.at(), {10, 3}});
    box.editor.set_caret(3);
    box.handle(KeyEvent{.key = KeyCode::ArrowDown});
    ASSERT(box.editor.caret() == 6);
    box.handle(KeyEvent{.key = KeyCode::ArrowDown});
    ASSERT(box.editor.caret() == 11);
    box.handle(KeyEvent{.key = KeyCode::ArrowUp});
    box.handle(KeyEvent{.key = KeyCode::ArrowUp});
    ASSERT(box.editor.caret() == 3);
    arrange(box, {box.at(), {2, 3}});
    box.set_text("é界ab");
    ASSERT(box.get_scroll_length() == 4);
    box.handle(KeyEvent{.key = KeyCode::End, .modifiers = {.ctrl = true}});
    auto frame = Frame{box.size()};
    auto const cursor = box.paint(Canvas{frame});
    ASSERT(cursor && cursor->y >= 0 && cursor->y < 3);
}

TEST(text_box_selection_tabs_newlines_and_orphan_marks_paint_without_controls)
{
    auto box = TextBox{{.text = "́a\t界\nend"}};
    arrange(box, {box.at(), {8, 3}});
    box.editor.select_all();
    auto frame = Frame{box.size()};
    box.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"◌́"));
    ASSERT((frame[Point{2, 0}].style.traits.contains(Trait::Inverse)));
    ASSERT((frame[Point{4, 0}].text == U"界"));
    ASSERT((frame[Point{6, 0}].style.traits.contains(Trait::Inverse)));
    box.handle(MousePress{{5, 0}, MouseButton::Left});
    ASSERT(box.editor.caret() == 4);
}

TEST(text_widgets_use_and_own_the_canvas_width_policy_for_layout_and_hits)
{
    auto overrides = std::array{esc::ScalarWidthOverride{U'a', 2}};
    auto line = LineEdit{{.text = "ax"}};
    arrange(line, {line.at(), {8, 1}});
    auto frame = Frame{line.size(), {.overrides = overrides}};
    line.paint(Canvas{frame});
    overrides[0].cells = 1;
    line.handle(MousePress{{1, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 0);
    line.handle(MousePress{{2, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 1);
    auto box = TextBox{{.text = "ax"}};
    arrange(box, {box.at(), {8, 2}});
    frame.resize(box.size());
    box.paint(Canvas{frame});
    box.handle(MousePress{{2, 0}, MouseButton::Left});
    ASSERT(box.editor.caret() == 1);
}

TEST(text_box_scroll_notifications_are_coalesced_and_direct_setters_request_paint)
{
    auto box = TextBox{{.text = "one\ntwo\nthree"}};
    arrange(box, {box.at(), {10, 2}});
    auto app = Application{box};
    app.post(esc::Resize{box.size()});
    app.step();
    int notifications = 0;
    box.on_scroll.connect([&](int, int) { ++notifications; });
    app.paint();
    auto const initial = notifications;
    app.request_repaint();
    app.paint();
    ASSERT(notifications == initial);
    ASSERT(box.set_scroll_offset(100));
    ASSERT(notifications == initial + 1);
    ASSERT(!box.set_scroll_offset(100));
    ASSERT(notifications == initial + 1);
    app.paint();
    ASSERT(!app.loop().needs_repaint());
    box.set_text("new");
    ASSERT(app.loop().needs_repaint());
    ASSERT(box.get_scroll_length() == 1);
}

TEST(application_paste_stays_with_the_original_editor_across_focus_changes)
{
    auto root = Row{LineEdit{}, LineEdit{}};
    arrange(root, {root.at(), {12, 1}});
    auto app = Application{root};
    auto& first = get_child<0>(root);
    auto& second = get_child<1>(root);
    app.dispatch(esc::PasteChunk{"e", true, false});
    app.set_focus(second);
    app.dispatch(esc::PasteChunk{"́", false, true});
    ASSERT(first.text() == "é" && second.text().empty());
    ASSERT(first.editor.undo() && first.text().empty());
    app.dispatch(esc::PasteChunk{"x", true, false});
    second.enabled = false;
    ASSERT(!app.dispatch(esc::PasteChunk{"end", false, true}).handled);
    ASSERT(second.text().empty());
}

TEST(text_widgets_extreme_hit_coordinates_clamp_without_overflow)
{
    auto line = LineEdit{{.text = "abc", .align = Align::Right}};
    arrange(line, {line.at(), {std::numeric_limits<int>::max(), 1}});
    line.handle(MousePress{{std::numeric_limits<int>::min(), 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 0);
    line.handle(MousePress{{std::numeric_limits<int>::max(), 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 3);
    auto box = TextBox{{.text = "abc", .align = Align::Right}};
    arrange(box, {box.at(), line.size()});
    box.handle(MousePress{{std::numeric_limits<int>::min(), 0}, MouseButton::Left});
    ASSERT(box.editor.caret() == 0);
}

TEST(application_text_replacement_cancels_an_unfinished_paste)
{
    auto line = LineEdit{};
    arrange(line, {line.at(), {10, 1}});
    auto app = Application{line};
    app.dispatch(esc::PasteChunk{"old", true, false});
    line.set_text("new");
    ASSERT(!app.dispatch(esc::PasteChunk{"bytes", false, true}).repaint);
    ASSERT(line.text() == "new");
}
