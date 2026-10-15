#include <algorithm>
#include <array>
#include <ox/ox.hpp>
#include <zzz/test.hpp>
using namespace ox;

TEST(streaming_layout_matches_complete_analysis)
{
    auto const records = std::array<std::string, 8>{"\n",
                                                    "abcd\n",
                                                    "a b c d\n",
                                                    "界é👩‍💻\n",
                                                    "\twide\tend\n",
                                                    "́orphan 🇺🇸\n",
                                                    "crlf\r\n",
                                                    "\u200djoin\n"};
    for (auto wrap : {TextWrap::None, TextWrap::Any, TextWrap::Word}) {
        for (auto width : {0, 1, 4, 13, 80}) {
            auto layout = TextLayout{};
            layout.update({}, width, wrap);
            auto expected = std::string{};
            for (int i = 0; i < 96; ++i) {
                auto discard = std::size_t{0};
                if (i % 3 && !expected.empty()) { discard = expected.find('\n') + 1; }
                auto const& suffix = records[(std::size_t)i % records.size()];
                expected = expected.substr(discard) + suffix;
                layout.append_lines(suffix, discard);
                auto full = TextLayout{};
                full.update(expected, width, wrap);
                ASSERT(layout.text() == full.text());
                ASSERT(std::ranges::equal(layout.lines(), full.lines()));
                ASSERT(std::ranges::equal(layout.boundaries(), full.boundaries()));
                ASSERT(std::ranges::equal(layout.widths(), full.widths()));
                for (auto at : full.boundaries()) {
                    ASSERT(layout.position(at) == full.position(at));
                }
                for (int y = -1; y <= (int)full.lines().size(); ++y) {
                    for (int x = -1; x < 16; ++x) {
                        ASSERT(layout.offset({x, y}) == full.offset({x, y}));
                    }
                }
            }
            layout.append_lines({}, expected.size());
            ASSERT(layout.text().empty());
            ASSERT((layout.lines().size() == 1 && layout.position(0) == Point{}));
        }
    }
}

TEST(streaming_editor_preserves_surviving_selection_and_normalizes_lines)
{
    auto edit = TextEdit{"old\n界é\nlast\n"};
    edit.set_caret(4);
    edit.set_caret(10, true);
    ASSERT(edit.append_lines("new\r\n", 4));
    ASSERT(edit.text() == "界é\nlast\nnew\n");
    ASSERT(edit.anchor() == 0 && edit.caret() == 6);
    auto full = TextEdit{edit.text()};
    ASSERT(std::ranges::equal(edit.boundaries(), full.boundaries()));
    edit.set_caret(0);
    edit.insert("undo\n");
    edit.undo();
    ASSERT(edit.can_redo());
    edit.append_lines("tail\n");
    ASSERT(!edit.can_undo() && !edit.can_redo());
    ASSERT(!edit.paste({"pending", true, false}));
    edit.append_lines("more\n");
    ASSERT(!edit.paste({"late", false, true}));
    ASSERT(edit.text().ends_with("more\n"));
    ASSERT(edit.anchor() == 0 && edit.caret() == 0);
}

TEST(streaming_edits_reject_incomplete_lines_without_changes)
{
    auto edit = TextEdit{"first\nlast\n"};
    auto layout = TextLayout{};
    layout.update(edit.text(), 4);
    for (auto discard : {1u, 100u}) {
        ASSERT_THROWS(edit.append_lines("next\n", discard), std::invalid_argument);
        ASSERT_THROWS(layout.append_lines("next\n", discard), std::invalid_argument);
    }
    ASSERT_THROWS(edit.append_lines("partial"), std::invalid_argument);
    ASSERT_THROWS(layout.append_lines("partial"), std::invalid_argument);
    ASSERT_THROWS(edit.append_lines("bad\033\n"), std::invalid_argument);
    ASSERT_THROWS(layout.append_lines("bad\033\n"), std::invalid_argument);
    ASSERT_THROWS(edit.append_lines("\xff\n"), esc::UnicodeError);
    ASSERT_THROWS(layout.append_lines("\xff\n"), esc::UnicodeError);
    ASSERT(edit.text() == "first\nlast\n" && layout.text() == edit.text());
    auto uninitialized = TextLayout{};
    ASSERT_THROWS(uninitialized.append_lines("a\n"), std::logic_error);
    auto single = TextEdit{"", false};
    ASSERT_THROWS(single.append_lines("a\n"), std::invalid_argument);
}

TEST(streaming_textbox_notifies_once_and_handles_reentrant_changes)
{
    auto box = TextBox{{.text = "first\nsecond\n", .read_only = true}};
    arrange(box, {{}, {6, 3}});
    box.set_caret(6);
    int changes = 0;
    box.on_change.connect([&](auto const&) { ++changes; });
    box.append_lines("界\r\n", 6);
    ASSERT(box.text() == "second\n界\n" && changes == 1);
    ASSERT(box.editor().caret() == 0);
    box.on_change.connect([&](auto const&) {
        if (box.text().ends_with("again\n")) box.set_text("replaced\n");
    });
    box.append_lines("again\n");
    ASSERT(box.text() == "replaced\n" && changes == 3);
}

TEST(palette_cached_search_matches_scoring_and_refreshes_shared_commands)
{
    auto commands = std::vector<std::shared_ptr<Command>>{};
    for (int i = 0; i < 80; ++i) {
        commands.push_back(command({.text = "Command " + std::to_string(i) + " 界",
                                    .keywords = i % 2 ? "Open File" : "Close View"}));
    }
    auto palette = CommandPalette{commands};
    for (auto query : {"", "op", "c v", "界", "zz", "COMMAND 7"}) {
        palette.query.set_text(query);
        auto expected = std::vector<std::pair<std::size_t, std::size_t>>{};
        for (std::size_t i = 0; i < commands.size(); ++i) {
            if (auto rank = command_score(
                    query, commands[i]->text + " " + commands[i]->keywords))
                expected.emplace_back(*rank, i);
        }
        std::stable_sort(expected.begin(), expected.end(),
                         [](auto a, auto b) { return a.first < b.first; });
        ASSERT(palette.matches().size() == expected.size());
        for (std::size_t i = 0; i < expected.size(); ++i)
            ASSERT(palette.matches()[i] == expected[i].second);
    }
    commands[0]->text = "Replacement";
    commands[0]->keywords = "unique";
    commands[0]->enabled = false;
    palette.refresh();
    palette.query.set_text("unique");
    ASSERT(palette.matches().size() == 1 && palette.matches()[0] == 0);
    ASSERT(palette.choices().items()[0].text == "Replacement");
    ASSERT(!palette.choices().items()[0].enabled);
    ASSERT(!palette.choices().current_row());
}

TEST(tree_lookup_and_row_caches_survive_collapsing_nested_paths)
{
    auto nodes = std::vector<TreeNode>{};
    for (int i = 0; i < 14; ++i)
        nodes.push_back({std::to_string(i), true, true, {{"界"}, {"child"}}});
    auto tree = TreeView{{.nodes = nodes, .multiple = true}};
    arrange(tree, {{}, {12, 4}});
    for (std::size_t i = 0; i < nodes.size(); ++i) {
        ASSERT(tree.select({i, 1}));
        ASSERT((tree.current_path() == std::vector<std::size_t>{i, 1}));
        ASSERT(tree.set_expanded({i}, false));
        tree.select({i, 1});
        ASSERT(tree.nodes()[i].expanded);
        ASSERT((tree.current_path() == std::vector<std::size_t>{i, 1}));
        ASSERT(tree.select({i, 0}));
        ASSERT((tree.current_path() == std::vector<std::size_t>{i, 0}));
    }
    tree.set_nodes({{"replacement"}});
    arrange(tree, {{}, {3, 2}});
    ASSERT(tree.list().items().size() == 1);
    ASSERT(tree.select({0}));
    ASSERT(tree.list().items()[0].text.find("replacement") != std::string::npos);
}

TEST(log_streaming_handles_batches_larger_than_capacity)
{
    auto log = LogView{{.max_lines = 2}};
    log.append("one\ntwo\nthree\nfour\n");
    ASSERT(log.editor.text() == "three\nfour\n");
    log.editor.select_all();
    log.append("five\nsix\nseven\n");
    ASSERT(log.editor.text() == "six\nseven\n");
    ASSERT(log.editor.editor().caret() == 0 && log.editor.editor().anchor() == 0);
    log.set_max_lines(1);
    ASSERT(log.editor.text() == "seven\n");
}

TEST(streaming_measure_cache_matches_fresh_textboxes_across_geometry_and_placeholder)
{
    auto box = TextBox{{.placeholder = "placeholder"}};
    arrange(box, {{}, {7, 3}});
    auto text = std::string{};
    for (int i = 0; i < 30; ++i) {
        auto width = i % 2 ? 4 : 12;
        box.measure({width, 100});
        auto discard = text.empty() || i % 3 == 0 ? 0 : text.find('\n') + 1;
        text = text.substr(discard) + "界 row\n";
        box.append_lines("界 row\n", discard);
        auto full = TextBox{{.text = text}};
        ASSERT(box.measure({width, 100}) == full.measure({width, 100}));
        ASSERT(box.measure({width + 1, 100}) == full.measure({width + 1, 100}));
        arrange(box, {{}, {width, 3}});
        arrange(full, {{}, {width, 3}});
        ASSERT(box.content_size() == full.content_size());
    }
}

TEST(retained_list_layouts_paint_like_fresh_lists_after_model_changes)
{
    auto list = ListView{};
    for (auto count : {40, 1, 20, 0, 3, 40}) {
        auto items = std::vector<ListItem>{};
        for (int i = 0; i < count; ++i)
            items.push_back({std::to_string(count + i) + "界é", i % 3 != 0});
        list.set_items(items);
        auto fresh = ListView{{.items = items}};
        for (auto width : {3, 12}) {
            arrange(list, {{}, {width, 5}});
            arrange(fresh, {{}, {width, 5}});
            list.set_scroll_offset({0, 10});
            fresh.set_scroll_offset({0, 10});
            auto frame = Frame{{width, 5}}, expected = Frame{{width, 5}};
            list.paint(Canvas{frame});
            fresh.paint(Canvas{expected});
            for (int y = 0; y < 5; ++y)
                ASSERT(std::ranges::equal(frame.row(y), expected.row(y)));
            ASSERT(list.content_size() == fresh.content_size());
        }
    }
}

TEST(palette_equivalent_matches_preserve_current_row_scroll_and_notifications)
{
    auto commands = std::vector<std::shared_ptr<Command>>{};
    for (int i = 0; i < 50; ++i)
        commands.push_back(
            command({.text = std::to_string(i), .keywords = "alpha beta"}));
    int current = 0, selected = 0;
    auto palette = CommandPalette{commands};
    arrange(palette, {{}, {10, 5}});
    for (auto& child : palette.get_children()) {
        if (auto* choices = dynamic_cast<ListView*>(&child)) {
            choices->select(30);
            choices->set_scroll_offset({0, 25});
            choices->on_current.connect([&](auto) { ++current; });
            choices->on_select.connect([&](auto const&) { ++selected; });
            palette.query.set_text("alpha");
            palette.query.set_text("beta");
            ASSERT(choices->current_row() == 30);
            ASSERT(choices->scroll_offset().y == 25);
            ASSERT(current == 0 && selected == 0);
        }
    }
}
