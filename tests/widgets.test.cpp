#include <zzz/test.hpp>

#include <algorithm>
#include <array>
#include <limits>

#include <ox/ox.hpp>

using namespace ox;

TEST(line_edit_edits_unicode_scalars_and_repeated_keys)
{
    auto edit = LineEdit{{.text = "aé日"}};
    edit.size = {8, 1};
    edit.key_press(KeyEvent{.key = KeyCode::End});
    edit.key_press(KeyEvent{.key = KeyCode::Backspace, .action = KeyAction::Repeat});
    ASSERT((edit.text() == "aé"));
    edit.key_press(KeyEvent{.key = KeyCode::ArrowLeft});
    edit.key_press(KeyEvent{.text = U"界🙂"});
    ASSERT((edit.text() == "a界🙂é"));
    edit.key_press(KeyEvent{.key = KeyCode::Delete});
    ASSERT((edit.text() == "a界🙂"));
    edit.key_press(KeyEvent{.key = U'x', .modifiers = {.ctrl = true}});
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
    edit.size = {8, 1};
    edit.paste(esc::PasteChunk{"\xc3", true, false});
    ASSERT((edit.text().empty()));
    edit.paste(esc::PasteChunk{"\xa9x\n", false, true});
    ASSERT((edit.text() == "é"));
    edit.paste(esc::PasteChunk{"discard", true, true, true});
    ASSERT((edit.text() == "é"));
}

TEST(text_box_handles_associated_text_and_multiline_paste)
{
    auto box = TextBox{{.text = "é日", .focus_policy = FocusPolicy::Strong}};
    box.size = {10, 4};
    box.key_press(KeyEvent{.key = KeyCode::End});
    box.key_press(KeyEvent{.key = KeyCode::Backspace});
    ASSERT((box.text() == "é"));
    box.key_press(KeyEvent{.text = U"界🙂"});
    ASSERT((box.text() == "é界🙂"));
    box.paste(esc::PasteChunk{"\n\xc3", true, false});
    ASSERT((box.text() == "é界🙂"));
    box.paste(esc::PasteChunk{"\xa9", false, true});
    ASSERT((box.text() == "é界🙂\né"));
    box.paste(esc::PasteChunk{"discard", true, true, true});
    ASSERT((box.text() == "é界🙂\né"));
}

TEST(button_handles_repeats_and_unidentified_mouse_release)
{
    auto button = BasicButton{};
    int presses = 0;
    int releases = 0;
    button.on_press.connect([&] { ++presses; });
    button.on_release.connect([&] { ++releases; });
    button.key_press(KeyEvent{.key = KeyCode::Enter, .action = KeyAction::Repeat});
    button.mouse_press(MousePress{{0, 0}, MouseButton::Left});
    button.mouse_release(MouseRelease{{0, 0}, std::nullopt});
    ASSERT((presses == 2));
    ASSERT((releases == 1));
}

TEST(line_edit_enter_preserves_changes_from_the_callback)
{
    auto edit = LineEdit{{.text = "submitted"}};
    edit.on_enter.connect([&](std::string const&) { edit.set_text(""); });
    edit.key_press(KeyEvent{.key = KeyCode::Enter});
    ASSERT(edit.text().empty());
}

TEST(data_table_uses_signed_wheel_steps_and_clamps_scroll)
{
    auto table = DataTable{};
    table.add_column("Items");
    for (int i = 0; i < 8; ++i) {
        table.add_row({std::to_string(i)});
    }
    table.mouse_wheel(MouseWheel{{0, 0}, 0, 3});
    ASSERT(table.offset == 3);
    table.mouse_wheel(MouseWheel{{0, 0}, 0, -2});
    ASSERT(table.offset == 1);
    table.mouse_wheel(MouseWheel{{0, 0}, 0, 20});
    ASSERT(table.offset == 7);
    table.mouse_wheel(MouseWheel{{0, 0}, 0, -20});
    ASSERT(table.offset == 0);
    table.mouse_wheel(MouseWheel{{0, 0}, 2, 0});
    ASSERT(table.offset == 0);
}

TEST(text_painting_never_splits_a_combining_cluster_at_the_viewport_edge)
{
    auto editor = LineEdit{{.text = "ae\xcc\x81"}};
    editor.size = {2, 1};
    editor.key_press(KeyEvent{.key = KeyCode::End});
    auto frame = Frame{{2, 1}};
    editor.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"e\u0301"));
    ASSERT((editor.cursor == Cursor{Point{1, 0}}));

    auto box = TextBox{{.text = "e\xcc\x81"
                                "e\xcc\x81"}};
    box.size = {1, 2};
    frame.resize({1, 2});
    box.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"e\u0301"));
    ASSERT((frame[Point{0, 1}].text == U"e\u0301"));
}

TEST(text_widgets_delete_combining_and_emoji_graphemes_as_units)
{
    auto line = LineEdit{{.text = "é👩‍💻"}};
    line.size = {10, 1};
    line.key_press(KeyEvent{.key = KeyCode::End});
    line.key_press(KeyEvent{.key = KeyCode::Backspace});
    ASSERT(line.text() == "é");
    line.key_press(KeyEvent{.key = KeyCode::Backspace});
    ASSERT(line.text().empty());
    auto box = TextBox{{.text = "a🇺🇸é"}};
    box.size = {10, 3};
    box.key_press(KeyEvent{.key = KeyCode::End});
    box.key_press(KeyEvent{.key = KeyCode::ArrowLeft});
    box.key_press(KeyEvent{.key = KeyCode::Delete});
    ASSERT(box.text() == "a🇺🇸");
    box.key_press(KeyEvent{.key = KeyCode::Backspace});
    ASSERT(box.text() == "a");
}

TEST(line_edit_selection_and_mouse_hits_use_terminal_columns)
{
    auto line = LineEdit{{.text = "a界b"}};
    line.size = {8, 1};
    line.mouse_press({{2, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 1);
    line.mouse_press({{3, 0}, MouseButton::Left, {.shift = true}});
    ASSERT((line.editor.selection() == std::pair<std::size_t, std::size_t>{1, 4}));
    auto frame = Frame{line.size};
    line.paint(Canvas{frame});
    ASSERT((frame[Point{1, 0}].style.traits.contains(Trait::Inverse)));
    ASSERT((frame[Point{2, 0}].style.traits.contains(Trait::Inverse)));
    ASSERT((!frame[Point{3, 0}].style.traits.contains(Trait::Inverse)));
    line.key_press(KeyEvent{.text = U"🙂"});
    ASSERT(line.text() == "a🙂b");
    line.key_press(KeyEvent{.key = U'z', .modifiers = {.ctrl = true}});
    ASSERT(line.text() == "a界b");
}

TEST(line_edit_alignment_scrolling_and_hits_share_the_same_geometry)
{
    auto line = LineEdit{{.text = "界x", .align = Align::Right}};
    line.size = {8, 1};
    auto frame = Frame{line.size};
    line.paint(Canvas{frame});
    ASSERT((frame[Point{5, 0}].text == U"界"));
    line.mouse_press({{6, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 0);
    line.mouse_press({{7, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 3);
    line.set_text("aé界🙂b");
    line.size = {4, 1};
    frame.resize(line.size);
    line.key_press(KeyEvent{.key = KeyCode::End});
    line.paint(Canvas{frame});
    ASSERT(line.cursor && line.cursor->x < 4);
    line.mouse_press({{0, 0}, MouseButton::Left});
    ASSERT(std::ranges::find(line.editor.boundaries(), line.editor.caret()) !=
           line.editor.boundaries().end());
}

TEST(text_box_wrapped_navigation_uses_preferred_columns_before_first_paint)
{
    auto box = TextBox{{.text = "abcd\nx\n界abc"}};
    box.size = {10, 3};
    box.editor.set_caret(3);
    box.key_press(KeyEvent{.key = KeyCode::ArrowDown});
    ASSERT(box.editor.caret() == 6);
    box.key_press(KeyEvent{.key = KeyCode::ArrowDown});
    ASSERT(box.editor.caret() == 11);
    box.key_press(KeyEvent{.key = KeyCode::ArrowUp});
    box.key_press(KeyEvent{.key = KeyCode::ArrowUp});
    ASSERT(box.editor.caret() == 3);
    box.size = {2, 3};
    box.set_text("é界ab");
    ASSERT(box.get_scroll_length() == 4);
    box.key_press(KeyEvent{.key = KeyCode::End, .modifiers = {.ctrl = true}});
    auto frame = Frame{box.size};
    box.paint(Canvas{frame});
    ASSERT(box.cursor && box.cursor->y >= 0 && box.cursor->y < 3);
}

TEST(text_box_selection_tabs_newlines_and_orphan_marks_paint_without_controls)
{
    auto box = TextBox{{.text = "́a\t界\nend"}};
    box.size = {8, 3};
    box.editor.select_all();
    auto frame = Frame{box.size};
    box.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"◌́"));
    ASSERT((frame[Point{2, 0}].style.traits.contains(Trait::Inverse)));
    ASSERT((frame[Point{4, 0}].text == U"界"));
    ASSERT((frame[Point{6, 0}].style.traits.contains(Trait::Inverse)));
    box.mouse_press({{5, 0}, MouseButton::Left});
    ASSERT(box.editor.caret() == 4);
}

TEST(text_widgets_use_and_own_the_canvas_width_policy_for_layout_and_hits)
{
    auto overrides = std::array{esc::ScalarWidthOverride{U'a', 2}};
    auto line = LineEdit{{.text = "ax"}};
    line.size = {8, 1};
    auto frame = Frame{line.size, {.overrides = overrides}};
    line.paint(Canvas{frame});
    overrides[0].cells = 1;
    line.mouse_press({{1, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 0);
    line.mouse_press({{2, 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 1);
    auto box = TextBox{{.text = "ax"}};
    box.size = {8, 2};
    frame.resize(box.size);
    box.paint(Canvas{frame});
    box.mouse_press({{2, 0}, MouseButton::Left});
    ASSERT(box.editor.caret() == 1);
}

TEST(text_box_scroll_notifications_are_coalesced_and_direct_setters_request_paint)
{
    auto box = TextBox{{.text = "one\ntwo\nthree"}};
    box.size = {10, 2};
    auto app = Application{box};
    app.post(esc::Resize{box.size});
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
    root.size = {12, 1};
    root.resize({});
    auto app = Application{root};
    auto& first = get_child<0>(root);
    auto& second = get_child<1>(root);
    app.handle_paste({"e", true, false});
    app.focus().set(second);
    app.handle_paste({"́", false, true});
    ASSERT(first.text() == "é" && second.text().empty());
    ASSERT(first.editor.undo() && first.text().empty());
    app.handle_paste({"x", true, false});
    second.enabled = false;
    ASSERT(!app.handle_paste({"end", false, true}).handled);
    ASSERT(second.text().empty());
}

TEST(text_widgets_extreme_hit_coordinates_clamp_without_overflow)
{
    auto line = LineEdit{{.text = "abc", .align = Align::Right}};
    line.size = {std::numeric_limits<int>::max(), 1};
    line.mouse_press({{std::numeric_limits<int>::min(), 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 0);
    line.mouse_press({{std::numeric_limits<int>::max(), 0}, MouseButton::Left});
    ASSERT(line.editor.caret() == 3);
    auto box = TextBox{{.text = "abc", .align = Align::Right}};
    box.size = line.size;
    box.mouse_press({{std::numeric_limits<int>::min(), 0}, MouseButton::Left});
    ASSERT(box.editor.caret() == 0);
}

TEST(application_text_replacement_cancels_an_unfinished_paste)
{
    auto line = LineEdit{};
    line.size = {10, 1};
    auto app = Application{line};
    app.handle_paste({"old", true, false});
    line.set_text("new");
    ASSERT(!app.handle_paste({"bytes", false, true}).repaint);
    ASSERT(line.text() == "new");
}
