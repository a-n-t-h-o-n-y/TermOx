#include <zzz/test.hpp>

#include <algorithm>
#include <array>
#include <random>

// Editing and layout need no widgets, frames, event loop, or terminal session.
#include <esc/text.hpp>
#include <ox/core/text.hpp>

using namespace ox;

namespace {
void check(TextEdit const& edit)
{
    auto expected = std::vector<std::size_t>{0};
    for (auto cluster : esc::graphemes(edit.text())) {
        expected.push_back(expected.back() + cluster.size());
    }
    ASSERT(std::ranges::equal(expected, edit.boundaries()));
    ASSERT(std::ranges::find(expected, edit.caret()) != expected.end());
    ASSERT(std::ranges::find(expected, edit.anchor()) != expected.end());
}
}  // namespace

TEST(text_normalizes_line_breaks_and_has_explicit_single_line_mode)
{
    auto edit = TextEdit{"a\r\nb\rc\n\td"};
    ASSERT(edit.text() == "a\nb\nc\n\td");
    ASSERT(TextEdit("a\r\nb\tc", false).text() == "abc");
    ASSERT(edit.caret() == 0 && edit.anchor() == 0);
    ASSERT_THROWS(TextEdit("a\x1b"), std::invalid_argument);
    ASSERT_THROWS(TextEdit(std::string_view{"a\0b", 3}), std::invalid_argument);
    ASSERT_THROWS(TextEdit("\xc3"), esc::UnicodeError);
}

TEST(text_navigation_and_deletion_preserve_complete_graphemes)
{
    auto edit = TextEdit{"aé界👩‍💻🇺🇸z"};
    auto const points =
        std::vector<std::size_t>{edit.boundaries().begin(), edit.boundaries().end()};
    for (auto point : points) {
        ASSERT(edit.caret() == point);
        edit.move_right();
    }
    ASSERT(!edit.move_right());
    for (auto i = points.size(); i > 0; --i) {
        ASSERT(edit.caret() == points[i - 1]);
        edit.move_left();
    }
    ASSERT(!edit.move_left());
    edit.set_caret(points[2]);
    edit.erase_backward();
    ASSERT(edit.text() == "a界👩‍💻🇺🇸z");
    edit.erase_forward();
    ASSERT(edit.text() == "a👩‍💻🇺🇸z");
    edit.erase_forward();
    ASSERT(edit.text() == "a🇺🇸z");
    check(edit);
    ASSERT_THROWS(edit.set_caret(2), std::invalid_argument);
}

TEST(text_insertions_resegment_joined_clusters_and_undo_restores_boundaries)
{
    auto edit = TextEdit{"eX"};
    edit.set_caret(1);
    edit.insert("́");
    ASSERT(edit.text() == "éX" && edit.caret() == 3);
    edit.erase_backward();
    ASSERT(edit.text() == "X");
    edit.undo();
    ASSERT(edit.text() == "éX");
    edit.undo();
    ASSERT(edit.text() == "eX" && edit.caret() == 1);
    edit.redo();
    check(edit);
    auto flag = TextEdit{"🇸"};
    flag.insert("🇺");
    ASSERT(flag.text() == "🇺🇸" && flag.caret() == flag.text().size());
    flag.undo();
    ASSERT(flag.text() == "🇸" && flag.caret() == 0);
}

TEST(text_selection_replacement_and_undo_restore_direction_and_caret)
{
    auto edit = TextEdit{"a界éz"};
    edit.set_caret(edit.text().size());
    edit.move_left(true);
    edit.move_left(true);
    auto const old_anchor = edit.anchor();
    auto const old_caret = edit.caret();
    ASSERT((edit.selection() ==
            std::pair<std::size_t, std::size_t>{4, edit.text().size()}));
    edit.insert("🙂");
    ASSERT(edit.text() == "a界🙂" && edit.caret() == edit.anchor());
    edit.undo();
    ASSERT(edit.text() == "a界éz");
    ASSERT(edit.anchor() == old_anchor && edit.caret() == old_caret);
    edit.redo();
    ASSERT(edit.text() == "a界🙂");
    edit.select_all();
    ASSERT(edit.erase_forward() && edit.text().empty());
    edit.undo();
    ASSERT(edit.text() == "a界🙂");
}

TEST(text_arrow_navigation_collapses_selection_and_word_moves_use_separators)
{
    auto edit = TextEdit{"one  two\n界\tthree"};
    edit.move_right(false, true);
    ASSERT(edit.caret() == 5);
    edit.move_right(false, true);
    ASSERT(edit.caret() == 9);
    edit.move_left(false, true);
    ASSERT(edit.caret() == 5);
    edit.select_all();
    edit.move_left();
    ASSERT(edit.caret() == 0 && edit.anchor() == 0);
    edit.select_all();
    edit.move_right();
    ASSERT(edit.caret() == edit.text().size());
    edit.erase_backward(true);
    ASSERT(edit.text() == "one  two\n界\t");
    edit.set_caret(0);
    edit.erase_forward(true);
    ASSERT(edit.text() == "two\n界\t");
}

TEST(text_history_is_bounded_new_edits_clear_redo_and_set_text_resets)
{
    auto edit = TextEdit{"", true, 2};
    edit.insert("a");
    edit.insert("b");
    edit.insert("c");
    ASSERT(edit.undo() && edit.undo() && !edit.undo());
    ASSERT(edit.text() == "a");
    edit.redo();
    edit.insert("x");
    ASSERT(!edit.redo() && edit.text() == "abx");
    edit.set_text(edit.text());
    ASSERT(edit.caret() == 0 && !edit.can_undo());
    auto no_history = TextEdit{"", true, 0};
    ASSERT(no_history.insert("x") && !no_history.undo());
}

TEST(text_invalid_or_rejected_edits_are_atomic_and_preserve_selection)
{
    auto edit = TextEdit{"hello"};
    edit.select_all();
    ASSERT(!edit.insert("x", [](char32_t) { return false; }));
    ASSERT(edit.text() == "hello" && edit.anchor() == 0 && edit.caret() == 5);
    ASSERT_THROWS(edit.insert("\xff"), esc::UnicodeError);
    ASSERT_THROWS(edit.insert("\x1b"), std::invalid_argument);
    ASSERT_THROWS(
        edit.insert("x",
                    [](char32_t) -> bool { throw std::runtime_error{"validate"}; }),
        std::runtime_error);
    ASSERT(edit.text() == "hello" && !edit.can_undo());
}

TEST(text_paste_is_one_atomic_edit_handles_split_utf8_abort_and_malformed_input)
{
    auto edit = TextEdit{"old"};
    edit.select_all();
    ASSERT(!edit.paste({"\xf0\x9f", true, false}));
    ASSERT(edit.text() == "old");
    ASSERT(edit.paste({"\x99\x82\r\nnew", false, true}));
    ASSERT(edit.text() == "🙂\nnew");
    ASSERT(edit.undo() && edit.text() == "old" && edit.caret() == 3);
    ASSERT(!edit.paste({"\xff", true, true, true}));
    ASSERT(edit.text() == "old");
    ASSERT(!edit.paste({"late", false, true}));
    ASSERT_THROWS(edit.paste({"\xc3", true, true}), esc::UnicodeError);
    ASSERT(edit.text() == "old" && !edit.can_undo());
    edit.paste({"new", true, false});
    edit.set_text("reset");
    ASSERT(!edit.paste({"late", false, true}));
}

TEST(text_models_copy_and_move_without_dangling_cache_or_cursor_state)
{
    auto edit = TextEdit{"é日"};
    edit.select_all();
    edit.insert("🙂");
    auto copy = edit;
    copy.undo();
    ASSERT(copy.text() == "é日" && edit.text() == "🙂");
    auto moved = std::move(edit);
    ASSERT(moved.text() == "🙂");
    check(moved);
    check(edit);
    ASSERT(edit.text().empty());
    auto layout = TextLayout{};
    layout.update(moved.text(), 4);
    auto cached = layout;
    auto transferred = std::move(layout);
    ASSERT(cached.text() == "🙂" && transferred.position(4).x == 2);
    ASSERT((layout.position(0) == Point{0, 0}));
}

TEST(text_layout_wraps_words_graphemes_and_explicit_empty_lines)
{
    ASSERT(
        (layout_text("ab cd", 4) == std::vector<TextLine>{{0, 3, 3, 3}, {3, 5, 5, 2}}));
    ASSERT((layout_text("a\n\n", 4) ==
            std::vector<TextLine>{{0, 1, 2, 1}, {2, 2, 3, 0}, {3, 3, 3, 0}}));
    ASSERT((layout_text("ab", 2, TextWrap::Any) ==
            std::vector<TextLine>{{0, 2, 2, 2}, {2, 2, 2, 0}}));
    ASSERT((layout_text("界a", 1, TextWrap::Any) ==
            std::vector<TextLine>{{0, 3, 3, 2}, {3, 4, 4, 1}, {4, 4, 4, 0}}));
    ASSERT((layout_text("éé", 1, TextWrap::Any) ==
            std::vector<TextLine>{{0, 3, 3, 1}, {3, 6, 6, 1}, {6, 6, 6, 0}}));
    ASSERT(layout_text("abc\ndef", 0).size() == 2);
    ASSERT(layout_text("abc", 1, TextWrap::None).size() == 1);
}

TEST(text_layout_maps_bytes_to_cells_and_hits_wide_clusters_at_their_lead)
{
    auto layout = TextLayout{};
    layout.update("a界é🙂", 20);
    ASSERT((layout.position(4) == Point{3, 0}));
    ASSERT((layout.position(7) == Point{4, 0}));
    ASSERT(layout.offset({1, 0}) == 1 && layout.offset({2, 0}) == 1);
    ASSERT(layout.offset({3, 0}) == 4 && layout.offset({5, 0}) == 7);
    ASSERT(layout.offset({100, 100}) == 11 && layout.offset({-1, -1}) == 0);
    ASSERT_THROWS(layout.position(2), std::invalid_argument);
    layout.update("ab\r\nc", 20);
    ASSERT((layout.position(2) == Point{2, 0}));
    ASSERT((layout.position(4) == Point{0, 1}));
    ASSERT_THROWS(layout.position(3), std::invalid_argument);
}

TEST(text_layout_tabs_orphan_marks_and_owned_width_overrides_match_display)
{
    auto layout = TextLayout{};
    layout.update("a\t界", 20);
    ASSERT((layout.position(2) == Point{4, 0}));
    ASSERT(layout.offset({3, 0}) == 1);
    ASSERT(display_grapheme("\t", 1) == U"   ");
    ASSERT(display_grapheme("́", 0) == U"◌́");
    layout.update("́a", 20);
    ASSERT(layout.position(2).x == 1);
    auto overrides = std::array{esc::ScalarWidthOverride{U'a', 2}};
    layout.update("a", 20, TextWrap::None, {.overrides = overrides});
    overrides[0].cells = 1;
    ASSERT(layout.position(1).x == 2);
    auto copy = layout;
    layout.update("other", 20);
    ASSERT(copy.position(1).x == 2);
}

TEST(text_layout_cache_reuses_rows_and_invalid_updates_leave_it_intact)
{
    auto layout = TextLayout{};
    ASSERT_THROWS(layout.update("", -1), std::invalid_argument);
    ASSERT(layout.update("abc", 10));
    auto const storage = layout.lines().data();
    ASSERT(!layout.update("abc", 10) && layout.lines().data() == storage);
    ASSERT_THROWS(layout.update("bad\x1b", 10), std::invalid_argument);
    ASSERT_THROWS(layout.update("abc", -1), std::invalid_argument);
    ASSERT_THROWS(layout.update("abc", 10, TextWrap::Word, {}, 0),
                  std::invalid_argument);
    ASSERT(layout.text() == "abc" && layout.lines().data() == storage);
    ASSERT(layout.update("abc", 2));
}

TEST(text_vertical_navigation_preserves_preferred_column_through_short_lines)
{
    auto edit = TextEdit{"abcd\nx\n界abc"};
    auto layout = TextLayout{};
    layout.update(edit.text(), 20);
    edit.set_caret(3);
    edit.move_vertical(layout, 1);
    ASSERT(edit.caret() == 6);
    edit.move_vertical(layout, 1);
    ASSERT((layout.position(edit.caret()) == Point{3, 2}));
    edit.move_vertical(layout, -2);
    ASSERT(edit.caret() == 3);
    edit.move_left();
    edit.move_vertical(layout, 2);
    ASSERT((layout.position(edit.caret()) == Point{2, 2}));
    auto other = TextLayout{};
    ASSERT_THROWS(edit.move_vertical(other, 1), std::invalid_argument);
}

TEST(text_key_commands_support_selection_history_and_ignore_releases_and_unknown_keys)
{
    auto edit = TextEdit{"abc"};
    auto layout = TextLayout{};
    layout.update(edit.text(), 10);
    ASSERT(!edit.key_press(KeyEvent{.key = KeyCode::Escape}, layout).handled);
    ASSERT(!edit.key_press(
                    KeyEvent{.key = KeyCode::ArrowRight, .action = KeyAction::Release},
                    layout)
                .handled);
    ASSERT(!edit.key_press(KeyEvent{.key = KeyCode::ArrowLeft}, layout).repaint);
    edit.key_press(KeyEvent{.key = KeyCode::End}, layout);
    edit.key_press(KeyEvent{.key = KeyCode::ArrowLeft, .modifiers = {.shift = true}},
                   layout);
    ASSERT((edit.selection() == std::pair<std::size_t, std::size_t>{2, 3}));
    edit.key_press(KeyEvent{.text = U"🙂"}, layout);
    ASSERT(edit.text() == "ab🙂");
    edit.key_press(KeyEvent{.key = U'z', .modifiers = {.ctrl = true}}, layout);
    ASSERT(edit.text() == "abc");
    edit.key_press(KeyEvent{.key = U'y', .modifiers = {.ctrl = true}}, layout);
    ASSERT(edit.text() == "ab🙂");
}

TEST(text_random_edits_selection_and_history_keep_valid_grapheme_boundaries)
{
    auto edit = TextEdit{};
    auto random = std::mt19937{23};
    auto const fragments =
        std::array{"x", "界", "é", "👩‍💻", "🇺", "🇸", "́", "\n", "\t"};
    for (int i = 0; i < 600; ++i) {
        switch (random() % 8) {
            case 0: edit.insert(fragments[random() % fragments.size()]); break;
            case 1: edit.move_left(random() % 2); break;
            case 2: edit.move_right(random() % 2); break;
            case 3: edit.erase_backward(); break;
            case 4: edit.erase_forward(); break;
            case 5: edit.undo(); break;
            case 6: edit.redo(); break;
            case 7: edit.select_all(); break;
        }
        check(edit);
        auto layout = TextLayout{};
        layout.update(edit.text(), (int)(random() % 8));
        for (auto point : edit.boundaries()) {
            auto const position = layout.position(point);
            ASSERT(layout.offset(position) == point);
        }
    }
}

TEST(text_layout_coordinate_index_matches_display_across_wrapping_and_line_breaks)
{
    auto const text = std::string{"́a\t界 é🙂\r\n\nword wide界\tend\n"};
    for (auto wrap : {TextWrap::None, TextWrap::Any, TextWrap::Word}) {
        for (auto width : {0, 1, 4, 9, 40}) {
            auto layout = TextLayout{};
            layout.update(text, width, wrap);
            for (auto y = std::size_t{0}; y < layout.lines().size(); ++y) {
                auto const line = layout.lines()[y];
                auto at = line.begin;
                auto x = 0;
                for (auto cluster : esc::graphemes(std::string_view{text}.substr(
                         line.begin, line.end - line.begin))) {
                    ASSERT((layout.position(at) == Point{x, (int)y}));
                    auto cells = 0;
                    auto const display_text = display_grapheme(cluster, x);
                    for (auto display : esc::graphemes(display_text)) {
                        cells += (int)*esc::grapheme_width(display);
                    }
                    for (auto column = x; column < x + cells; ++column) {
                        ASSERT(layout.offset({column, (int)y}) == at);
                    }
                    at += cluster.size();
                    x += cells;
                }
                ASSERT(x == line.columns);
                ASSERT(layout.offset({-100, (int)y}) == line.begin);
                ASSERT(layout.offset({1000, (int)y}) == line.end);
                if (line.next > line.end || y + 1 == layout.lines().size()) {
                    ASSERT((layout.position(line.end) == Point{x, (int)y}));
                }
                else {
                    ASSERT((layout.position(line.end) == Point{0, (int)y + 1}));
                }
            }
        }
    }
}

TEST(text_edits_resegment_whole_lines_for_long_unicode_context_and_joined_newlines)
{
    for (auto fragment : {"🇺", "क्", "؀", "́", "👩‍"}) {
        auto text = std::string{"prefix\n"};
        for (auto i = 0; i < 31; ++i) {
            text += fragment;
        }
        text += '\n';
        for (auto i = 0; i < 33; ++i) {
            text += fragment;
        }
        text += "a\nsuffix\nunchanged";
        for (auto inserted : {"🇸", "́", "क्", "x\n؀"}) {
            auto edit = TextEdit{text};
            edit.set_caret(7);
            ASSERT(edit.insert(inserted));
            check(edit);
            auto const after = edit.text();
            auto const caret = edit.caret();
            ASSERT(edit.undo() && edit.text() == text && edit.caret() == 7);
            check(edit);
            ASSERT(edit.redo() && edit.text() == after && edit.caret() == caret);
            check(edit);
        }
        auto edit = TextEdit{text};
        auto const newline = text.find('\n', 7);
        edit.set_caret(newline);
        edit.move_right(true);
        ASSERT(edit.erase_backward());
        check(edit);
        auto expected = text;
        expected.erase(newline, 1);
        ASSERT(edit.text() == expected);
        ASSERT(edit.undo() && edit.text() == text);
        ASSERT(edit.anchor() == newline && edit.caret() == newline + 1);
        ASSERT(edit.redo() && edit.text() == expected);
        check(edit);
    }
}

TEST(text_random_multiline_replacements_match_full_segmentation_and_restore_history)
{
    auto document = std::string{};
    for (auto i = 0; i < 200; ++i) {
        document += "line é界 🇺🇸 👩‍💻 क्\n";
    }
    auto edit = TextEdit{document, true, 20};
    auto random = std::mt19937{89};
    auto const fragments = std::array{"x", "界", "é", "🇺", "🇸", "́", "\n", "क्", "؀"};
    for (auto i = 0; i < 400; ++i) {
        auto const a = (std::size_t)random() % edit.boundaries().size();
        auto const b = std::min(a + random() % 40, edit.boundaries().size() - 1);
        auto const left = edit.boundaries()[a];
        auto const right = edit.boundaries()[b];
        auto const reverse = random() % 2;
        edit.set_caret(reverse ? right : left);
        edit.set_caret(reverse ? left : right, true);
        auto const anchor = edit.anchor();
        auto const caret = edit.caret();
        auto const before = edit.text();
        auto expected = before;
        auto const fragment = std::string_view{fragments[random() % fragments.size()]};
        expected.replace(left, right - left, fragment);
        ASSERT(edit.insert(fragment) && edit.text() == expected);
        check(edit);
        auto const after_caret = edit.caret();
        ASSERT(edit.undo() && edit.text() == before);
        ASSERT(edit.anchor() == anchor && edit.caret() == caret);
        check(edit);
        ASSERT(edit.redo() && edit.text() == expected && edit.caret() == after_caret);
        check(edit);
    }
}

TEST(text_prepared_edits_survive_copy_move_aliasing_and_reset)
{
    auto edit = TextEdit{"prefix\né界\nsuffix", true, 2};
    edit.set_caret(7);
    edit.insert("́");
    auto const changed = edit.text();
    auto copy = edit;
    ASSERT(copy.undo() && copy.text() == "prefix\né界\nsuffix");
    ASSERT(edit.text() == changed);
    auto moved = std::move(copy);
    ASSERT(moved.redo() && moved.text() == changed);
    ASSERT(copy.insert("new") && copy.text() == "new");
    check(copy);
    moved.set_text(moved.text());
    ASSERT(!moved.can_undo() && !moved.can_redo() && moved.caret() == 0);
    moved.set_caret(moved.text().size());
    auto const original = moved.text();
    ASSERT(moved.insert(moved.text()) && moved.text() == original + original);
    ASSERT(moved.undo() && moved.text() == original);
    check(moved);
}

TEST(text_identical_selection_replacement_records_only_the_selection_change)
{
    auto edit = TextEdit{"prefix\né界\nsuffix"};
    auto const original = edit.text();
    edit.select_all();
    ASSERT(edit.insert(edit.text()) && edit.text() == original);
    ASSERT(edit.anchor() == edit.caret() && edit.caret() == original.size());
    ASSERT(edit.undo() && edit.text() == original);
    ASSERT(edit.anchor() == 0 && edit.caret() == original.size());
    ASSERT(edit.redo() && edit.text() == original && edit.anchor() == edit.caret());
    check(edit);
}

TEST(text_layout_reuses_analysis_across_geometry_tabs_policy_copy_and_move)
{
    auto layout = TextLayout{};
    layout.update("a\t界 a", 8, TextWrap::Any);
    ASSERT(layout.lines().size() == 2 && layout.lines()[0].columns == 8);
    ASSERT((layout.position(7) == Point{0, 1}));
    layout.update(layout.text(), 7, TextWrap::Any);
    ASSERT((layout.position(6) == Point{0, 1}));
    ASSERT((layout.position(7) == Point{1, 1}));
    layout.update(layout.text(), 7, TextWrap::Any, {}, 8);
    ASSERT(layout.lines().size() == 3 && layout.lines()[1].columns == 8);
    ASSERT((layout.position(2) == Point{0, 2}));
    ASSERT(layout.offset({7, 1}) == 1);
    layout.update(layout.text(), 7, TextWrap::None, {}, 8);
    ASSERT((layout.position(7) == Point{12, 0}));
    auto overrides = std::array{esc::ScalarWidthOverride{U'a', 2}};
    layout.update(layout.text(), 7, TextWrap::None, {.overrides = overrides}, 8);
    ASSERT((layout.position(1) == Point{2, 0}));
    ASSERT((layout.position(7) == Point{13, 0}));
    auto copy = layout;
    overrides[0].cells = 1;
    layout.update(layout.text(), 7, TextWrap::None, {.overrides = overrides}, 8);
    ASSERT((layout.position(7) == Point{12, 0}));
    copy.update(copy.text(), 4, TextWrap::None, copy.policy(), 8);
    ASSERT((copy.position(7) == Point{13, 0}));
    auto moved = std::move(copy);
    ASSERT((copy.position(0) == Point{0, 0}));
    moved.update("\r\n", 3);
    ASSERT((moved.position(2) == Point{0, 1}));
    ASSERT((copy.update("a", 3) && copy.position(1) == Point{1, 0}));
}

TEST(text_layout_validation_preserves_unicode_error_offsets_and_owned_cache)
{
    auto layout = TextLayout{};
    layout.update("a界", 4, TextWrap::None);
    auto const rows = layout.lines().data();
    auto const bytes = layout.text().data();
    auto const prefix = std::string{"a界éx"};
    for (auto text : {prefix + "\xf0\x9f", std::string{"\x01"} + prefix + "\xc3"}) {
        try {
            layout.update(text, 1);
            ASSERT(false);
        }
        catch (esc::UnicodeError const& error) {
            ASSERT(error.offset() == text.size() - (text.ends_with("\xc3") ? 1 : 2));
        }
    }
    for (auto c = 0; c < 32; ++c) {
        if (c == '\r' || c == '\n' || c == '\t') { continue; }
        ASSERT_THROWS(layout.update(std::string(1, (char)c), 1), std::invalid_argument);
    }
    ASSERT_THROWS(layout.update("\x7f", 1), std::invalid_argument);
    ASSERT_THROWS(layout.update("\xc2\x85", 1), std::invalid_argument);
    auto spaces = std::array{esc::ScalarWidthOverride{U' ', 2}};
    ASSERT_THROWS(
        layout.update(layout.text(), 1, TextWrap::Word, {.overrides = spaces}),
        std::invalid_argument);
    ASSERT(layout.text().data() == bytes && layout.lines().data() == rows);
    ASSERT(!layout.update("a界", 4, TextWrap::None));
    ASSERT((layout.position(4) == Point{3, 0}));
    layout.update(layout.text(), 2, TextWrap::Any);
    ASSERT((layout.position(1) == Point{0, 1}));
    ASSERT((layout.position(4) == Point{0, 2}));
}
