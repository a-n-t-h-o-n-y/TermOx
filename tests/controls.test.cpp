#include <zzz/test.hpp>

#include <limits>

#include <ox/ox.hpp>

using namespace ox;

TEST(control_functions_match_activation_and_keep_optional_indices_in_range)
{
    ASSERT(activates(KeyEvent{.key = U' '}));
    ASSERT(!activates(KeyEvent{.key = KeyCode::Enter, .action = KeyAction::Repeat}));
    ASSERT(!activates(KeyEvent{.key = U' ', .modifiers = {.ctrl = true}}));
    ASSERT(!activates(KeyEvent{.key = KeyCode::Enter, .action = KeyAction::Release}));
    ASSERT(releases(KeyEvent{
        .key = U' ', .modifiers = {.shift = true}, .action = KeyAction::Release}));
    ASSERT(releases(MouseRelease{{0, 0}, {}}));
    ASSERT(!releases(MouseRelease{{0, 0}, MouseButton::Right}));
    auto selected = std::optional<std::size_t>{};
    ASSERT(select_index(selected, 2, 3).repaint && selected == 2);
    ASSERT(!select_index(selected, 3, 3).handled && selected == 2);
    ASSERT(!select_index(selected, 2, 3).repaint);
    ASSERT(move_selection(selected, 3, std::numeric_limits<int>::min()).repaint &&
           selected == 0);
    selected = std::numeric_limits<std::size_t>::max();
    ASSERT(move_selection(selected, 3, 1).repaint && selected == 2);
    auto response = move_selection(selected, 0, 1);
    ASSERT(!response.handled && response.repaint && !selected);
}

TEST(checkbox_commands_coalesce_notifications_and_preserve_glyph_styles_in_focus)
{
    auto box = CheckBox{};
    for (auto& glyph : box.checked_glyphs) {
        glyph.style.foreground = colors::blue;
    }
    arrange(box, {{0, 0}, {4, 1}});
    auto app = Application{box};
    app.dispatch(esc::Resize{box.size()});
    app.paint();
    auto changes = std::vector<bool>{};
    box.on_change.connect([&](bool checked) { changes.push_back(checked); });
    ASSERT(box.set_checked(true));
    ASSERT(!box.set_checked(true));
    ASSERT(app.loop().needs_repaint());
    app.paint();
    ASSERT((changes == std::vector<bool>{true}));
    ASSERT((app.loop().screen[Point{1, 0}].style.foreground == Color{colors::blue}));
    ASSERT((app.loop().screen[Point{1, 0}].style.traits.contains(Trait::Bold)));
    ASSERT(app.dispatch(KeyEvent{.key = U' '}).repaint && !box.checked());
    ASSERT((changes == std::vector<bool>{true, false}));
}

TEST(button_keyboard_visual_state_and_release_use_shared_activation)
{
    auto button = Button{{.label = {.text = "go", .align = Align::Left},
                          .pressed_style = {.background = {},
                                            .foreground = {},
                                            .enabled = Trait::Bold,
                                            .disabled = {}},
                          .focused_style = {.background = {},
                                            .foreground = colors::red,
                                            .enabled = {},
                                            .disabled = {}}}};
    arrange(button, {{0, 0}, {8, 1}});
    auto app = Application{button};
    app.dispatch(esc::Resize{button.size()});
    int presses = 0;
    int releases_count = 0;
    button.on_press.connect([&] { ++presses; });
    button.on_release.connect([&] { ++releases_count; });
    ASSERT(app.dispatch(MousePress{{0, 0}, MouseButton::Left}).handled &&
           button.is_pressed());
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].style.foreground == Color{colors::red}));
    ASSERT((app.loop().screen[Point{0, 0}].style.traits.contains(Trait::Bold)));
    ASSERT(app.dispatch(MouseRelease{{0, 0}, MouseButton::Left}).handled);
    ASSERT(!button.is_pressed() && presses == 1 && releases_count == 1);
    ASSERT(!button.handle(MouseRelease{{0, 0}, {}}).handled);
}

TEST(button_fade_combines_hover_and_focus_and_follows_moves)
{
    auto button =
        Button{{.decoration = Fade{.paint_fn = {},
                                   .fade_in = std::chrono::milliseconds{0},
                                   .fade_out = std::chrono::milliseconds{0}}}};
    arrange(button, {{0, 0}, {4, 1}});
    auto app = Application{button};
    ASSERT(button.fade_percent == 1.f);
    button.mouse_enter();
    app.clear_focus();
    ASSERT(button.fade_percent == 1.f);
    button.mouse_leave();
    ASSERT(button.fade_percent == 0.f);
    auto animated = Button{{.decoration = Fade{}}};
    arrange(animated, {{0, 0}, {4, 1}});
    animated.mouse_enter();
    auto moved = std::move(animated);
    auto other = Application{moved};
    other.step(EventLoop::Clock::now() + std::chrono::milliseconds{30});
    ASSERT(moved.fade_percent > 0.f);
}

TEST(radio_group_selection_snapshots_survive_item_changes_and_reject_invalid_indices)
{
    auto group = RadioGroup{{.items = {"one", "two"}}};
    arrange(group, {{}, {12, 2}});
    auto notifications = std::vector<std::optional<std::size_t>>{};
    group.on_select.connect([&](std::optional<std::size_t> value) {
        if (value) { group.set_items({"replacement"}); }
    });
    group.on_select.connect(
        [&](std::optional<std::size_t> value) { notifications.push_back(value); });
    ASSERT(group.select(0));
    ASSERT((notifications == std::vector<std::optional<std::size_t>>{std::nullopt, 0}));
    ASSERT(!group.selected_index() && group.items().size() == 1);
    ASSERT_THROWS(group.select(std::numeric_limits<std::size_t>::max()),
                  std::out_of_range);
    group.set_items({});
    ASSERT(!group.handle(KeyEvent{.key = KeyCode::Enter}).handled);
}

TEST(radio_group_indicator_offsets_use_terminal_columns)
{
    auto group =
        RadioGroup{{.items = {"a"}, .selected_index = 0, .on = U"👩‍💻"_gs}};
    arrange(group, {{0, 0}, {6, 1}});
    auto app = Application{group};
    app.dispatch(esc::Resize{group.size()});
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].columns == 2));
    ASSERT((app.loop().screen[Point{2, 0}].text == U" "));
    ASSERT((app.loop().screen[Point{3, 0}].text == U"a"));
}
