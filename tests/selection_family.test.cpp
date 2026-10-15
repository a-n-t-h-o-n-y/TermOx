#include <memory>
#include <ox/ox.hpp>
#include <zzz/test.hpp>
using namespace ox;

TEST(row_selection_ranges_toggle_disabled_rows_and_boundaries)
{
    auto s = RowSelection{};
    ASSERT(select_rows(s, 1, 6));
    ASSERT(
        select_rows(s, 4, 6, SelectionAction::Extend, [](auto i) { return i != 3; }));
    ASSERT((s.rows == std::vector<std::size_t>{1, 2, 4}));
    select_rows(s, 2, 6, SelectionAction::Toggle);
    ASSERT((s.rows == std::vector<std::size_t>{1, 4}));
    select_rows(s, 5, 6, SelectionAction::Move);
    ASSERT((s.rows == std::vector<std::size_t>{1, 4} && s.current == 5));
    ASSERT(row_target({}, 6, std::numeric_limits<int>::max()) == 5);
    ASSERT(row_target(5, 6, 1, [](auto i) { return i < 4; }) == 3);
    ASSERT(!row_target({}, 6, 1, [](auto) { return false; }));
    auto before = s;
    ASSERT_THROWS(select_rows(s, 6, 6), std::out_of_range);
    ASSERT(s == before);
}

TEST(list_selection_navigation_typeahead_scrolling_and_notifications)
{
    auto list = ListView{
        {.items = {{"Alpha"}, {"Disabled", false}, {"Beta"}, {"Bravo"}, {"界界界"}},
         .multiple = true}};
    arrange(list, {{}, {4, 2}});
    int selections = 0, cursors = 0, actions = 0;
    list.on_select.connect([&](auto const&) { ++selections; });
    list.on_current.connect([&](auto) { ++cursors; });
    list.on_activate.connect([&](auto) { ++actions; });
    list.handle(KeyEvent{.key = KeyCode::ArrowDown});
    list.handle(KeyEvent{.key = KeyCode::ArrowDown, .modifiers = {.shift = true}});
    ASSERT((std::vector<std::size_t>{list.selected_rows().begin(),
                                     list.selected_rows().end()} ==
            std::vector<std::size_t>{0, 2}));
    list.handle(KeyEvent{.key = KeyCode::End, .modifiers = {.ctrl = true}});
    ASSERT(list.current_row() == 4 && list.scroll_offset().y == 3 && selections == 2);
    list.handle(KeyEvent{.key = U' ', .modifiers = {.ctrl = true}});
    ASSERT(list.selected_rows().size() == 3);
    list.handle(KeyEvent{.key = U'b'});
    ASSERT(list.current_row() == 2);
    list.handle(KeyEvent{.key = U'b'});
    ASSERT(list.current_row() == 3);
    list.handle(KeyEvent{.key = KeyCode::Enter});
    list.handle(KeyEvent{.key = KeyCode::Enter, .action = KeyAction::Repeat});
    ASSERT(actions == 1 && cursors >= 4);
    auto bar = ScrollBar{};
    link(list, bar);
    bar.set_position(0);
    ASSERT(list.scroll_offset().y == 0);
    list.select_all();
    ASSERT(list.selected_rows().size() == 4);
    auto frame = Frame{{4, 2}};
    list.set_scroll_offset({0, 3});
    list.paint(Canvas{frame});
    ASSERT((frame[Point{0, 1}].text == U"界"));
    auto value = std::vector<std::size_t>{list.selected_rows().begin(),
                                          list.selected_rows().end()};
    ASSERT_THROWS(list.set_items({{"bad\nrow"}}), std::invalid_argument);
    ASSERT(list.items().size() == 5 && list.selected_rows().size() == value.size());
}

TEST(list_callbacks_follow_moves_and_can_destroy_the_owner)
{
    auto list =
        std::make_unique<ListView>(ListView::Options{.items = {{"one"}, {"two"}}});
    auto moved = std::unique_ptr<ListView>{};
    list->on_select.connect([&](auto const&) {
        if (list) {
            moved = std::make_unique<ListView>(std::move(*list));
            list.reset();
        }
    });
    list->select(1);
    ASSERT(moved->current_row() == 1);
    moved->on_current.connect([&](auto) { moved.reset(); });
    moved->select(0);
    ASSERT(!moved);
}

TEST(radio_navigation_skips_disabled_items_and_scrolls_to_selection)
{
    auto radio = RadioGroup{{.items = {"one", "two", "three", "four"}}};
    arrange(radio, {{}, {12, 2}});
    radio.set_item_enabled(1, false);
    radio.handle(KeyEvent{.key = KeyCode::End});
    ASSERT(radio.selected_index() == 3 && radio.scroll_offset().y == 2);
    radio.handle(KeyEvent{.key = KeyCode::Home});
    radio.handle(KeyEvent{.key = KeyCode::ArrowDown});
    ASSERT(radio.selected_index() == 2 && radio.scroll_offset().y == 1);
    radio.set_item_enabled(2, false);
    ASSERT(!radio.selected_index());
    radio.set_scroll_offset({0, 2});
    radio.handle(MousePress{{1, 1}, MouseButton::Left});
    ASSERT(radio.selected_index() == 3);
}

TEST(select_popup_escapes_control_clipping_commits_and_restores_focus)
{
    auto root = Column{Select{{.items = {{"one"}, {"two"}, {"off", false}}}},
                       Button{{.label = {.text = "Other"}}}};
    auto app = Application{root};
    app.dispatch(esc::Resize{{20, 8}});
    auto& select = std::get<0>(root.children);
    auto& other = std::get<1>(root.children);
    ASSERT(select.size().height == 1);
    app.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(select.popup.is_open());
    ASSERT(app.focus().get() == track(select.popup.child));
    ASSERT(!app.is_interactive(other));
    auto tree = widget_tree(root);
    auto node = std::ranges::find(tree, track(static_cast<Widget&>(select.popup.child)),
                                  &WidgetNode::widget);
    ASSERT(node != tree.end() && node->visible.size.height == 3 &&
           node->bounds.at.y >= 1);
    app.dispatch(KeyEvent{.key = KeyCode::ArrowDown});
    app.dispatch(KeyEvent{.key = KeyCode::ArrowDown});
    ASSERT(!select.selected_index());
    app.dispatch(KeyEvent{.key = KeyCode::Escape});
    ASSERT(!select.popup.is_open() && !select.selected_index());
    ASSERT(app.focus().get() == track(select.button) && app.is_interactive(other));
    select.open();
    app.step();
    app.dispatch(KeyEvent{.key = KeyCode::End});
    app.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(select.selected_index() == 1 && select.button.label.text == "two" &&
           !select.popup.is_open());
    select.open();
    app.step();
    app.dispatch(MousePress{{19, 7}, MouseButton::Left});
    ASSERT(!select.popup.is_open() && select.selected_index() == 1);
}

TEST(combo_filters_choices_commits_original_indices_and_allows_custom_text)
{
    auto root = Column{ComboBox{{.items = {{"Apple"}, {"Apricot"}, {"Banana"}}}},
                       Label{"Below"}};
    auto& combo = std::get<0>(root.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{25, 6}});
    combo.set_text("ap");
    ASSERT(combo.popup.child.items().size() == 2 && !combo.selected_index());
    app.dispatch(KeyEvent{.key = KeyCode::F4});
    app.dispatch(KeyEvent{.key = KeyCode::End});
    app.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(combo.selected_index() == 1 && combo.text() == "Apricot");
    ASSERT(app.focus().get() == track(combo.editor));
    combo.set_text("custom");
    ASSERT(!combo.selected_index() && combo.text() == "custom" &&
           combo.popup.child.items().empty());
    combo.set_text("");
    ASSERT(combo.popup.child.items().size() == 3);
}

TEST(tabs_arrange_only_selected_pages_and_ctrl_navigation_bubbles_from_editors)
{
    auto tabs =
        Tabs{std::tuple{LineEdit{{.text = "first"}}, LineEdit{{.text = "second"}}},
             std::vector<std::string>{"One", "Two"}};
    auto app = Application{tabs};
    app.dispatch(esc::Resize{{12, 5}});
    app.set_focus(std::get<0>(tabs.pages.children));
    app.dispatch(KeyEvent{.key = KeyCode::Tab, .modifiers = {.ctrl = true}});
    ASSERT(tabs.selected_index() == 1 &&
           app.focus().get() == track(std::get<1>(tabs.pages.children)));
    ASSERT(std::get<1>(tabs.pages.children).at().y == 0 && tabs.pages.at().y == 1);
    app.dispatch(
        KeyEvent{.key = KeyCode::Tab, .modifiers = {.shift = true, .ctrl = true}});
    ASSERT(tabs.selected_index() == 0);
    ASSERT_THROWS(tabs.set_labels({"wrong"}), std::invalid_argument);
}

TEST(dialog_modal_tab_capture_cancellation_nested_scopes_and_results)
{
    auto root = Column{Button{}, Dialog{Column{LineEdit{}, Button{}}, "Outer"},
                       Dialog{Button{}, "Inner"}};
    auto& base = std::get<0>(root.children);
    auto& outer = std::get<1>(root.children);
    auto& inner = std::get<2>(root.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{30, 12}});
    app.dispatch(MousePress{{0, 0}, MouseButton::Left});
    ASSERT(base.is_pointer_captured());
    outer.requested_size = Area{20, 8};
    outer.show();
    app.step();
    ASSERT(!base.is_pointer_captured() && !base.is_pressed());
    auto& input = std::get<0>(outer.child.child.children);
    auto& button = std::get<1>(outer.child.child.children);
    ASSERT(app.focus().get() == track(input));
    app.dispatch(KeyEvent{.key = KeyCode::Tab});
    ASSERT(app.focus().get() == track(button));
    app.dispatch(KeyEvent{.key = KeyCode::Tab});
    ASSERT(app.focus().get() == track(input));
    inner.requested_size = Area{10, 4};
    inner.show();
    app.step();
    ASSERT(app.focus().get() == track(inner.child.child));
    int rejected = 0;
    inner.on_result.connect(
        [&](auto result) { rejected += (result == DialogResult::Rejected); });
    app.dispatch(KeyEvent{.key = KeyCode::Escape});
    ASSERT(rejected == 1 && app.focus().get() == track(input));
    outer.accept();
    app.step();
    ASSERT(app.focus().get() == track(base) && !outer.is_open());
}

TEST(popup_repositions_on_resize_and_hides_with_its_owner)
{
    auto root =
        Column{Select{{.items = {{"wide choice"}, {"next"}, {"last"}}}}, Label{"tail"}};
    auto& choice = std::get<0>(root.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{20, 8}});
    choice.open();
    app.step();
    app.dispatch(esc::Resize{{6, 2}});
    ASSERT((choice.popup.size() == Area{6, 2}));
    auto tree = widget_tree(root);
    auto node = std::ranges::find(tree, track(static_cast<Widget&>(choice.popup.child)),
                                  &WidgetNode::widget);
    ASSERT(node->visible.size.width <= 6 && node->visible.size.height <= 2);
    choice.visible = false;
    app.step();
    tree = widget_tree(root);
    node = std::ranges::find(tree, track(static_cast<Widget&>(choice.popup.child)),
                             &WidgetNode::widget);
    ASSERT(node->visible.size == Area{} && !app.is_interactive(choice.popup.child));
}

TEST(nested_menu_paths_disabled_entries_and_action_destruction)
{
    int actions = 0;
    auto root = Column{
        MenuBar{{{"File",
                  {{.command = command({.text = "Disabled", .enabled = false})},
                   {.command = command({.text = "More"}),
                    .children = {{.command = command({.text = "Run",
                                                      .action =
                                                          [&] {
                                                              ++actions;
                                                              return EventResponse{};
                                                          }})}}}}}}},
        Label{"body"}};
    auto& bar = std::get<0>(root.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{35, 10}});
    auto paths = std::vector<std::vector<std::size_t>>{};
    bar.on_trigger.connect([&](auto const& path) { paths.push_back(path); });
    bar.open(0);
    app.step();
    app.dispatch(KeyEvent{.key = KeyCode::Home});
    ASSERT(bar.menu(0).current_row() == 1);
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    app.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(actions == 1 && (paths == std::vector<std::vector<std::size_t>>{{0, 1, 0}}));
    auto menu = std::unique_ptr<Menu>{};
    menu = std::make_unique<Menu>(
        std::vector<MenuItem>{{.command = command({.text = "Delete", .action = [&] {
                                                       menu.reset();
                                                       return EventResponse{};
                                                   }})}});
    menu->activate(0);
    ASSERT(!menu);
}

TEST(list_mutations_preserve_selected_identity_and_double_click_activates_once)
{
    auto list = ListView{{.items = {{"A"}, {"B"}, {"C"}}, .multiple = true}};
    arrange(list, {{}, {10, 4}});
    list.select(1);
    list.insert_item(0, {"First"});
    ASSERT(list.current_row() == 2 && list.selected_rows().front() == 2 &&
           list.items()[2].text == "B");
    list.erase_item(0);
    ASSERT(list.current_row() == 1 && list.selected_rows().front() == 1);
    list.set_item(1, {"B", false});
    ASSERT(list.selected_rows().empty() && list.current_row() == 2);
    int actions = 0;
    list.on_activate.connect([&](auto) { ++actions; });
    list.handle(MousePress{{1, 2}, MouseButton::Left});
    list.handle(MousePress{{1, 2}, MouseButton::Left});
    ASSERT(actions == 1);
    list.handle(KeyEvent{.key = U' '});
    ASSERT(list.selected_rows().empty());
}

TEST(list_reentrant_model_replacement_cancels_pending_activation)
{
    auto list = ListView{{.items = {{"Original"}}}};
    int actions = 0;
    list.on_activate.connect([&](auto) { ++actions; });
    bool once = true;
    list.on_select.connect([&](auto const&) {
        if (std::exchange(once, false)) {
            list.set_items({{"Replacement"}});
            list.select(0);
        }
    });
    list.handle(KeyEvent{.key = KeyCode::Enter});
    ASSERT(actions == 0 && list.items().front().text == "Replacement");
}

TEST(label_dialog_has_keyboard_fallback_and_overlay_order_tracks_opening_order)
{
    auto root = Column{Button{}, Dialog{Label{"First"}, "First"},
                       Dialog{Label{"Second"}, "Second"}};
    auto& first = std::get<1>(root.children);
    auto& second = std::get<2>(root.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{30, 10}});
    second.show();
    app.step();
    ASSERT(app.focus().get() == track(second));
    first.show();
    app.step();
    ASSERT(app.focus().get() == track(first));
    app.dispatch(KeyEvent{.key = KeyCode::Tab});
    ASSERT(app.focus().get() == track(first));
    app.dispatch(KeyEvent{.key = KeyCode::Escape});
    ASSERT(!first.is_open() && second.is_open() && app.focus().get() == track(second));
    app.dispatch(KeyEvent{.key = KeyCode::Escape});
    ASSERT(!second.is_open() && app.focus().get() == track(std::get<0>(root.children)));
}

TEST(menu_shortcuts_are_global_but_respect_modal_scopes_and_reopening_resets_submenus)
{
    auto root =
        Column{MenuBar{{{"File",
                         {{.command = command({.text = "More"}),
                           .children = {{.command = command({.text = "Action"})}}}}}}},
               LineEdit{}, Dialog{Button{}, "Modal"}};
    auto& menu = std::get<0>(root.children);
    auto& editor = std::get<1>(root.children);
    auto& dialog = std::get<2>(root.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{30, 10}});
    app.set_focus(editor);
    app.dispatch(KeyEvent{.key = U'f', .modifiers = {.alt = true}});
    ASSERT(app.focus().get() == track(menu.menu(0)));
    app.dispatch(KeyEvent{.key = KeyCode::ArrowDown});
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    menu.close();
    app.step();
    menu.open(0);
    app.step();
    ASSERT(app.focus().get() == track(menu.menu(0)));
    menu.close();
    app.step();
    dialog.requested_size = Area{10, 4};
    dialog.show();
    app.step();
    auto before = app.focus().get();
    app.dispatch(KeyEvent{.key = KeyCode::F10});
    ASSERT(app.focus().get() == before);
}

TEST(combo_selection_changes_emit_once_and_callbacks_can_destroy_the_control)
{
    auto combo =
        std::make_unique<ComboBox>(ComboBox::Options{.items = {{"One"}, {"Two"}}});
    auto seen = std::vector<std::optional<std::size_t>>{};
    combo->on_select.connect([&](auto value) { seen.push_back(value); });
    combo->select(0);
    combo->select(1);
    ASSERT((seen == std::vector<std::optional<std::size_t>>{0, 1}));
    combo->on_change.connect([&](auto const&) { combo.reset(); });
    combo->select(0);
    ASSERT(!combo);
}

TEST(root_overlay_paints_after_the_entire_normal_tree_and_shortcut_effects_survive)
{
    struct Scene : Widget {
        Popup<Label> popup{Label{"Popup"}};
        auto get_children() -> zzz::Generator<Widget&> override { co_yield popup; }
        void paint_overlay(Canvas canvas) override { fill(canvas, U'X'); }
    } scene;
    auto app = Application{scene};
    app.dispatch(esc::Resize{{15, 5}});
    scene.popup.show();
    app.step();
    app.paint();
    ASSERT((app.loop().screen[Point{5, 2}].text == U"P"));
    scene.popup.hide();
    app.step();
    scene.shortcuts = [&](KeyEvent const&) {
        scene.shortcuts = {};
        return EventResponse{.repaint = true};
    };
    auto response = app.dispatch(KeyEvent{.key = U'q'});
    ASSERT(response.repaint && !response.handled && !scene.shortcuts);
}

TEST(combo_popup_keeps_logical_ownership_inside_a_modal_dialog)
{
    auto root = Column{
        Button{},
        Dialog{Column{ComboBox{{.items = {{"One"}, {"Two"}}}}, Button{}}, "Choices"}};
    auto& dialog = std::get<1>(root.children);
    auto& combo = std::get<0>(dialog.child.child.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{40, 12}});
    dialog.requested_size = Area{30, 8};
    dialog.show();
    app.step();
    app.dispatch(KeyEvent{.key = KeyCode::F4});
    ASSERT(app.focus().get() == track(combo.popup.child));
    ASSERT(app.is_interactive(combo.popup.child) &&
           !app.is_interactive(std::get<0>(root.children)));
    app.dispatch(KeyEvent{.key = KeyCode::End});
    app.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(combo.selected_index() == 1 && dialog.is_open() &&
           app.focus().get() == track(combo.editor));
}

TEST(menu_parent_mouse_hover_and_top_level_keyboard_switching)
{
    auto bar = MenuBar{{{"File",
                         {{.command = command({.text = "More"}),
                           .children = {{.command = command({.text = "Child"})}}},
                          {.command = command({.text = "Leaf"})}}},
                        {"Edit", {{.command = command({.text = "Edit action"})}}}}};
    auto app = Application{bar};
    app.dispatch(esc::Resize{{40, 12}});
    bar.open(0);
    app.step();
    app.dispatch(KeyEvent{.key = KeyCode::ArrowDown});
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    auto parent = bar.menu(0).global_bounds();
    app.dispatch(MouseMove{{parent.at.x + 1, parent.at.y + 1}, std::nullopt});
    ASSERT(bar.menu(0).current_row() == 1 && app.focus().get() == track(bar.menu(0)));
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    ASSERT(app.focus().get() == track(bar.menu(1)));
    app.dispatch(KeyEvent{.key = KeyCode::ArrowLeft});
    ASSERT(app.focus().get() == track(bar.menu(0)));
    app.dispatch(KeyEvent{.key = U'e', .modifiers = {.alt = true}});
    ASSERT(app.focus().get() == track(bar.menu(1)));
    app.dispatch(KeyEvent{.key = KeyCode::F10});
    ASSERT(!app.is_interactive(bar.menu(1)));
}

TEST(tab_dismisses_select_before_advancing_through_the_form)
{
    auto root = Row{Select{{.items = {{"One"}, {"Two"}}}}, Button{}};
    auto& select = std::get<0>(root.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{20, 4}});
    select.open();
    app.step();
    app.dispatch(KeyEvent{.key = KeyCode::Tab});
    ASSERT(!select.popup.is_open() &&
           app.focus().get() == track(std::get<1>(root.children)));
}

TEST(requested_popup_extent_overrides_intrinsic_measurement)
{
    struct Content : Widget {
        auto measure(Area, esc::WidthPolicy = {}) const -> Area override
        { throw std::logic_error{"No intrinsic extent"}; }
    };
    auto popup = Popup{Content{}};
    popup.requested_size = Area{8, 3};
    arrange(popup, {{}, {20, 10}});
    ASSERT((popup.child.size() == Area{8, 3}));
    auto anchor = Widget{};
    arrange(anchor,
            {{std::numeric_limits<int>::max(), std::numeric_limits<int>::max()},
             {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()}});
    popup.anchor = track(anchor);
    popup.placement = PopupPlacement::Right;
    popup.offset = {std::numeric_limits<int>::max(), std::numeric_limits<int>::max()};
    arrange(popup, {{}, {20, 10}});
    ASSERT((popup.child.bounds() == Rect{{12, 7}, {8, 3}}));
}

TEST(replacing_a_menu_scope_preserves_the_original_focus_for_dismissal)
{
    auto root = Column{MenuBar{{{"File", {{.command = command({.text = "Open"})}}},
                                {"Edit", {{.command = command({.text = "Undo"})}}}}},
                       LineEdit{}};
    auto& bar = std::get<0>(root.children);
    auto& editor = std::get<1>(root.children);
    auto app = Application{root};
    app.dispatch(esc::Resize{{30, 10}});
    app.set_focus(editor);
    app.dispatch(KeyEvent{.key = U'f', .modifiers = {.alt = true}});
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    ASSERT(app.focus().get() == track(bar.menu(1)));
    app.dispatch(KeyEvent{.key = KeyCode::Escape});
    ASSERT(app.focus().get() == track(editor));
}

TEST(dropdowns_reveal_committed_rows_and_keep_duplicate_label_identity)
{
    auto items = std::vector<ListItem>{};
    for (int i = 0; i < 20; ++i) {
        items.push_back({std::to_string(i)});
    }
    auto select = Select{{.items = std::move(items), .selected = 18}};
    auto app = Application{select};
    app.dispatch(esc::Resize{{20, 5}});
    select.open();
    app.step();
    auto& list = select.popup.child;
    ASSERT(list.current_row() == 18 && list.scroll_offset().y <= 18 &&
           list.scroll_offset().y + list.viewport_size().height > 18);
    list.set_scroll_offset({0, 0});
    ASSERT(list.select(18) && list.scroll_offset().y > 0);
    auto combo = ComboBox{{.items = {{"Same"}, {"Different"}, {"Same"}}}};
    auto edit = Application{combo};
    edit.dispatch(esc::Resize{{20, 5}});
    combo.select(2);
    combo.open();
    edit.step();
    ASSERT(combo.popup.child.current_row() == 1);
    edit.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(combo.selected_index() == 2);
}

TEST(shared_scrollbar_links_radio_tree_and_log_without_inner_widget_access)
{
    static_assert(Scrollable<RadioGroup> && Scrollable<TreeView> &&
                  Scrollable<LogView>);
    auto radio = RadioGroup{{.items = {"a", "b", "c", "d"}}};
    arrange(radio, {{}, {10, 2}});
    auto bar = ScrollBar{};
    auto connections = link(radio, bar);
    ASSERT(bar.content_length() == 4 && bar.viewport_length() == 2);
    bar.set_position(2);
    ASSERT((radio.scroll_offset() == Point{0, 2}));
    radio.select(0);
    ASSERT(bar.position() == 0);
    arrange(radio, {{}, {10, 1}});
    ASSERT(bar.viewport_length() == 1);
    radio.set_items({"a"});
    ASSERT(bar.content_length() == 1 && bar.position() == 0);
    radio.on_scroll.disconnect(connections.first);
    bar.on_scroll.disconnect(connections.second);
    auto tree = TreeView{{.nodes = {{"a"}, {"b"}, {"c"}}}};
    arrange(tree, {{}, {10, 1}});
    auto treebar = ScrollBar{};
    link(tree, treebar);
    treebar.set_position(2);
    ASSERT(tree.scroll_offset().y == 2);
    auto log = LogView{};
    arrange(log, {{}, {10, 1}});
    auto logbar = ScrollBar{};
    link(log, logbar);
    log.append("a\nb\nc");
    ASSERT(logbar.position() == log.scroll_offset().y);
    logbar.set_position(0);
    ASSERT(log.scroll_offset().y == 0 && !log.follows_tail());
}
