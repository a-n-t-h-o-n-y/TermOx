#include <zzz/test.hpp>

#include <ox/ox.hpp>

using namespace ox;

TEST(line_edit_edits_unicode_scalars_and_repeated_keys)
{
    auto edit = LineEdit{{.text = "aé日"}};
    edit.size = {8, 1};
    edit.key_press(KeyEvent{.key = KeyCode::End});
    edit.key_press(KeyEvent{.key = KeyCode::Backspace, .action = KeyAction::Repeat});
    ASSERT((edit.text == "aé"));
    edit.key_press(KeyEvent{.key = KeyCode::ArrowLeft});
    edit.key_press(KeyEvent{.text = U"界🙂"});
    ASSERT((edit.text == "a界🙂é"));
    edit.key_press(KeyEvent{.key = KeyCode::Delete});
    ASSERT((edit.text == "a界🙂"));
    edit.key_press(KeyEvent{.key = U'x', .modifiers = {.ctrl = true}});
    ASSERT((edit.text == "a界🙂"));
    auto buffer = ScreenBuffer{{8, 1}};
    edit.paint(Canvas{buffer, {0, 0}, {8, 1}});
    ASSERT((buffer[Point{1, 0}].symbol == U'界'));
    ASSERT((buffer[Point{2, 0}].symbol == U'🙂'));
}

TEST(line_edit_buffers_split_paste_and_applies_scalar_validation)
{
    auto edit = LineEdit{{.validator = [](char32_t c) { return c != U'x'; }}};
    edit.size = {8, 1};
    edit.paste(esc::PasteChunk{"\xc3", true, false});
    ASSERT((edit.text.empty()));
    edit.paste(esc::PasteChunk{"\xa9x\n", false, true});
    ASSERT((edit.text == "é"));
    edit.paste(esc::PasteChunk{"discard", true, true, true});
    ASSERT((edit.text == "é"));
}

TEST(text_box_handles_associated_text_and_multiline_paste)
{
    auto box = TextBox{{.text = "é日", .focus_policy = FocusPolicy::Strong}};
    box.size = {10, 4};
    box.key_press(KeyEvent{.key = KeyCode::End});
    box.key_press(KeyEvent{.key = KeyCode::Backspace});
    ASSERT((box.text == "é"));
    box.key_press(KeyEvent{.text = U"界🙂"});
    ASSERT((box.text == "é界🙂"));
    box.paste(esc::PasteChunk{"\n\xc3", true, false});
    ASSERT((box.text == "é界🙂"));
    box.paste(esc::PasteChunk{"\xa9", false, true});
    ASSERT((box.text == "é界🙂\né"));
    box.paste(esc::PasteChunk{"discard", true, true, true});
    ASSERT((box.text == "é界🙂\né"));
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
    edit.on_enter.connect([&](std::string const&) { edit.text.clear(); });
    edit.key_press(KeyEvent{.key = KeyCode::Enter});
    ASSERT(edit.text.empty());
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
