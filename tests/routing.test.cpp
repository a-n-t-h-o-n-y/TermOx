#include <zzz/test.hpp>

#include <functional>
#include <limits>
#include <memory>

#include <ox/ox.hpp>

using namespace ox;
using namespace std::chrono_literals;

namespace {
struct Probe : Widget {
    std::vector<std::unique_ptr<Probe>> children;
    EventResponse reply;
    std::vector<Point> presses;
    std::vector<Point> moves;
    int keys = 0;
    int releases = 0;
    int pastes = 0;
    int enters = 0;
    int leaves = 0;
    int focus_ins = 0;
    int focus_outs = 0;
    int ticks = 0;
    std::optional<char32_t> glyph;
    std::function<EventResponse()> enter_action;
    std::function<EventResponse()> key_action;
    std::function<EventResponse()> in_action;
    std::function<EventResponse()> out_action;
    explicit Probe(FocusPolicy policy = FocusPolicy::None) : Widget{policy} {}
    auto get_children() -> zzz::Generator<Widget&> override
    {
        for (auto& child : children) {
            co_yield *child;
        }
    }
    auto mouse_press(MousePress m) -> EventResponse override
    {
        presses.push_back(m.at);
        return reply;
    }
    auto mouse_release(MouseRelease) -> EventResponse override { return reply; }
    auto mouse_wheel(MouseWheel) -> EventResponse override { return reply; }
    auto mouse_move(MouseMove m) -> EventResponse override
    {
        moves.push_back(m.at);
        return reply;
    }
    auto mouse_enter() -> EventResponse override
    {
        ++enters;
        auto action = enter_action;
        return action ? action() : EventResponse{};
    }
    auto mouse_leave() -> EventResponse override
    {
        ++leaves;
        return {};
    }
    auto focus_in() -> EventResponse override
    {
        ++focus_ins;
        auto action = in_action;
        return action ? action() : EventResponse{};
    }
    auto focus_out() -> EventResponse override
    {
        ++focus_outs;
        auto action = out_action;
        return action ? action() : EventResponse{};
    }
    auto key_press(KeyEvent) -> EventResponse override
    {
        ++keys;
        auto action = key_action;
        return action ? action() : reply;
    }
    auto key_release(KeyEvent) -> EventResponse override
    {
        ++releases;
        return reply;
    }
    auto paste(esc::PasteChunk const&) -> EventResponse override
    {
        ++pastes;
        return reply;
    }
    auto timer() -> EventResponse override
    {
        ++ticks;
        return reply;
    }
    void paint(Canvas canvas) override
    {
        if (glyph) { fill(canvas, *glyph); }
    }
};

auto add(Probe& parent, Point at, Area size, FocusPolicy policy = FocusPolicy::None)
    -> Probe&
{
    auto& child = parent.children.emplace_back(std::make_unique<Probe>(policy));
    child->at = at;
    child->size = size;
    return *child;
}

void settle(Application& app)
{
    app.request_repaint();
    app.paint();
    ASSERT(!app.loop().needs_repaint());
}
}  // namespace

TEST(routing_tree_shares_ancestor_clipping_and_local_coordinates_with_paint)
{
    auto root = Probe{};
    root.size = {8, 4};
    auto& parent = add(root, {2, 1}, {3, 2});
    auto& child = add(parent, {-2, -1}, {8, 5});
    child.glyph = U'x';
    child.reply = {.handled = true};
    auto app = Application{root};
    app.post(esc::Resize{root.size});
    app.step();
    ASSERT(app.paint());
    ASSERT((app.loop().screen[Point{2, 1}].text == U"x"));
    ASSERT((app.loop().screen[Point{1, 1}].text == U" "));
    ASSERT((app.loop().screen[Point{5, 1}].text == U" "));
    app.handle_mouse_press({{2, 1}, MouseButton::Left});
    ASSERT((child.presses == std::vector<Point>{{2, 1}}));
    app.handle_mouse_press({{1, 1}, MouseButton::Left});
    ASSERT(child.presses.size() == 1);
    ASSERT((root.presses.back() == Point{1, 1}));
    auto const tree = widget_tree(root);
    ASSERT((tree[2].visible == Rect{{2, 1}, {3, 2}}));
    ASSERT(tree[0].end == 3);
    ASSERT((hit_path(tree, {2, 1}) == std::vector<std::size_t>{0, 1, 2}));
}

TEST(routing_mouse_bubbles_to_ancestors_and_stops_at_handling)
{
    auto root = Probe{};
    root.size = {12, 5};
    auto& parent = add(root, {3, 1}, {6, 3});
    auto& child = add(parent, {2, 1}, {2, 1});
    parent.reply = {.handled = true};
    child.reply = {.repaint = true};
    auto app = Application{root};
    auto const result = app.handle_mouse_press({{5, 2}, MouseButton::Right});
    ASSERT(result.handled && result.repaint);
    ASSERT((child.presses.back() == Point{0, 0}));
    ASSERT((parent.presses.back() == Point{2, 1}));
    ASSERT(root.presses.empty());
}

TEST(routing_later_siblings_win_overlap_and_outside_input_is_ignored)
{
    auto root = Probe{};
    root.size = {5, 2};
    auto& first = add(root, {0, 0}, {5, 2});
    auto& last = add(root, {0, 0}, {5, 2});
    first.reply = last.reply = {.handled = true};
    auto app = Application{root};
    app.handle_mouse_press({{1, 0}, MouseButton::Left});
    ASSERT(last.presses.size() == 1 && first.presses.empty());
    app.handle_mouse_press({{-1, 0}, MouseButton::Left});
    app.handle_mouse_press({{5, 0}, MouseButton::Left});
    ASSERT(last.presses.size() == 1 && root.presses.empty());
}

TEST(routing_hidden_and_disabled_ancestors_prune_all_input_and_focus)
{
    auto root = Probe{};
    root.size = {10, 3};
    auto& hidden = add(root, {0, 0}, {3, 3});
    hidden.active = false;
    auto& hidden_child = add(hidden, {0, 0}, {3, 3}, FocusPolicy::Strong);
    auto& disabled = add(root, {3, 0}, {3, 3});
    disabled.enabled = false;
    auto& disabled_child = add(disabled, {0, 0}, {3, 3}, FocusPolicy::Strong);
    disabled_child.glyph = U'd';
    auto& chosen = add(root, {6, 0}, {4, 3}, FocusPolicy::Tab);
    auto app = Application{root};
    ASSERT(app.focus().get() == track(chosen));
    ASSERT_THROWS(app.focus().set(hidden_child), std::invalid_argument);
    ASSERT_THROWS(app.focus().set(disabled_child), std::invalid_argument);
    app.handle_mouse_press({{0, 0}, MouseButton::Left});
    app.handle_mouse_press({{3, 0}, MouseButton::Left});
    ASSERT(hidden_child.presses.empty() && disabled_child.presses.empty());
    app.post(esc::Resize{root.size});
    app.step();
    app.paint();
    ASSERT((app.loop().screen[Point{3, 0}].text == U"d"));
    ASSERT((app.loop().screen[Point{0, 0}].text == U" "));
}

TEST(routing_hover_notifies_only_changed_ancestry_and_handles_leaving_root)
{
    auto root = Probe{};
    root.size = {8, 2};
    auto& parent = add(root, {0, 0}, {8, 2});
    auto& left = add(parent, {0, 0}, {4, 2});
    auto& right = add(parent, {4, 0}, {4, 2});
    auto app = Application{root};
    app.handle_mouse_move({.at = {1, 0}, .held_button = {}});
    app.handle_mouse_move({.at = {2, 0}, .held_button = {}});
    ASSERT(root.enters == 1 && parent.enters == 1 && left.enters == 1);
    ASSERT(root.leaves == 0 && parent.leaves == 0 && left.leaves == 0);
    app.handle_mouse_move({.at = {5, 0}, .held_button = {}});
    ASSERT(left.leaves == 1 && right.enters == 1 && parent.leaves == 0);
    app.handle_mouse_move({.at = {8, 0}, .held_button = {}});
    ASSERT(right.leaves == 1 && parent.leaves == 1 && root.leaves == 1);
    app.handle_mouse_move({.at = {9, 0}, .held_button = {}});
    ASSERT(root.leaves == 1);
}

TEST(routing_hover_reconciles_geometry_and_activation_without_pointer_motion)
{
    auto root = Probe{};
    root.size = {8, 2};
    auto& child = add(root, {0, 0}, {4, 2});
    auto app = Application{root};
    app.handle_mouse_move({.at = {1, 0}, .held_button = {}});
    child.active = false;
    app.step();
    ASSERT(child.leaves == 1);
    child.active = true;
    app.step();
    ASSERT(child.enters == 2);
    child.at = {4, 0};
    app.step();
    ASSERT(child.leaves == 2);
    root.children.clear();
    app.step();  // No callback through the destroyed hover path.
}

TEST(routing_keyboard_release_and_paste_bubble_from_focus)
{
    auto root = Probe{};
    root.size = {4, 2};
    root.reply = {.handled = true};
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    settle(app);
    ASSERT(app.handle_key_press(KeyEvent{.text = U"x"}).handled);
    ASSERT(child.keys == 1 && root.keys == 1);
    ASSERT(!app.loop().needs_repaint());
    ASSERT(app.handle_key_release(KeyEvent{.key = U'x', .action = KeyAction::Release})
               .handled);
    ASSERT(child.releases == 1 && root.releases == 1);
    ASSERT(app.handle_paste({"text", true, true}).handled);
    ASSERT(child.pastes == 1 && root.pastes == 1);
    child.reply = {.handled = true};
    app.handle_key_press(KeyEvent{.text = U"y"});
    ASSERT(child.keys == 2 && root.keys == 1);
}

TEST(routing_tab_is_consumed_wraps_and_works_after_click_focus_or_explicit_clear)
{
    auto root = Probe{};
    root.size = {8, 2};
    auto& first = add(root, {0, 0}, {2, 2}, FocusPolicy::Tab);
    auto& click = add(root, {2, 0}, {2, 2}, FocusPolicy::Click);
    auto& last = add(root, {4, 0}, {2, 2}, FocusPolicy::Strong);
    auto& clipped = add(root, {8, 0}, {2, 2}, FocusPolicy::Tab);
    auto app = Application{root};
    app.handle_key_press(KeyEvent{.key = KeyCode::Tab});
    ASSERT(app.focus().get() == track(last));
    ASSERT(first.keys == 0 && last.keys == 0 && root.keys == 0);
    app.handle_key_press(KeyEvent{.key = KeyCode::Tab});
    ASSERT(app.focus().get() == track(first));
    app.handle_key_press(KeyEvent{.key = KeyCode::Tab, .modifiers = {.shift = true}});
    ASSERT(app.focus().get() == track(last));
    app.focus().set(click);
    app.handle_key_press(KeyEvent{.key = KeyCode::Tab});
    ASSERT(app.focus().get() == track(first));
    app.focus().clear();
    app.step();
    ASSERT(!app.focus().get().valid());
    app.handle_key_press(KeyEvent{.key = KeyCode::Tab, .modifiers = {.shift = true}});
    ASSERT(app.focus().get() == track(last));
    ASSERT(clipped.focus_ins == 0);
}

TEST(routing_click_uses_nearest_eligible_ancestor_and_only_left_button)
{
    auto root = Probe{FocusPolicy::Click};
    root.size = {4, 2};
    auto& child = add(root, {0, 0}, {4, 2});
    child.reply = {.handled = true};
    auto app = Application{root};
    app.handle_mouse_press({{1, 0}, MouseButton::Right});
    ASSERT(!root.focused);
    app.handle_mouse_press({{1, 0}, MouseButton::Left});
    ASSERT(root.focused);
    ASSERT(app.focus().get() == track(root));
}

TEST(routing_focus_recovers_on_disable_hide_detach_and_policy_change)
{
    auto root = Probe{};
    root.size = {8, 2};
    auto& first = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto& second = add(root, {4, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    first.enabled = false;
    app.step();
    ASSERT(!first.focused && first.focus_outs == 1 && second.focused);
    first.enabled = true;
    second.active = false;
    app.step();
    ASSERT(first.focused);
    first.focus_policy = FocusPolicy::None;
    app.step();
    ASSERT(!app.focus().get().valid());
    second.active = true;
    app.step();
    ASSERT(second.focused);
    auto detached = std::move(root.children.back());
    root.children.pop_back();
    app.step();
    ASSERT(!detached->focused && detached->focus_outs == 2);
    auto other = Probe{FocusPolicy::Strong};
    other.size = {1, 1};
    ASSERT_THROWS(app.focus().set(other), std::invalid_argument);
}

TEST(routing_focus_tracks_widget_relocation_and_move_assignment_invalidates_old_target)
{
    auto root = Row<std::vector<Probe>>{};
    root.children.emplace_back(FocusPolicy::Strong);
    root.size = {10, 2};
    root.resize({});
    auto app = Application{root};
    auto original = track(root.children.front());
    for (int i = 0; i < 10; ++i) {
        root.children.emplace_back(FocusPolicy::Strong);
    }
    root.resize(root.size);
    app.step();
    ASSERT(original.valid() && app.focus().get() == original);
    ASSERT(original.get().focused && original.get().focus_ins == 1);
    auto target = Probe{};
    auto old = track(target);
    auto external = target.lifetime;
    auto source = Probe{};
    auto moved = track(source);
    target = std::move(source);
    ASSERT(!old.valid() && *external == nullptr && moved.valid());
    ASSERT(&moved.get() == &target);
    auto& same_target = target;
    target = std::move(same_target);
    ASSERT(moved.valid());
}

TEST(routing_destroyed_focus_and_external_lifetime_storage_never_dangle)
{
    auto root = Probe{};
    root.size = {4, 2};
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    auto view = track(child);
    auto external = child.lifetime;
    root.children.clear();
    app.step();
    ASSERT(!view.valid() && !app.focus().get().valid() && *external == nullptr);
    ASSERT_THROWS(view.get(), std::logic_error);
    auto& next = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    app.step();
    ASSERT(next.focused);
}

TEST(routing_callback_can_destroy_the_target_and_still_bubble_safely)
{
    auto root = Probe{};
    root.size = {4, 2};
    root.reply = {.handled = true};
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    child.key_action = [&] {
        root.children.clear();
        return EventResponse{};
    };
    ASSERT(app.handle_key_press(KeyEvent{.text = U"x"}).handled);
    ASSERT(root.keys == 1 && !app.focus().get().valid());
}

TEST(routing_reentrant_focus_out_can_override_the_requested_transition)
{
    auto root = Probe{};
    root.size = {6, 2};
    auto& first = add(root, {0, 0}, {2, 2}, FocusPolicy::Strong);
    auto& second = add(root, {2, 0}, {2, 2}, FocusPolicy::Strong);
    auto& third = add(root, {4, 0}, {2, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    first.out_action = [&] { return app.focus().set(third); };
    app.focus().set(second);
    ASSERT(third.focused && !second.focused && !first.focused);
    ASSERT(app.focus().get() == track(third));
}

TEST(routing_quit_stops_propagation_and_repaint_is_explicit)
{
    auto root = Probe{};
    root.size = {4, 2};
    root.reply = {.handled = true};
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    settle(app);
    child.reply = {.handled = true};
    ASSERT(!app.handle_key_press(KeyEvent{.text = U"x"}).repaint);
    ASSERT(!app.paint());
    child.reply = {.repaint = true};
    ASSERT(app.handle_key_press(KeyEvent{.text = U"y"}).repaint);
    ASSERT(app.paint());
    child.reply = {.quit = 7};
    auto const result = app.handle_key_press(KeyEvent{.text = U"q"});
    ASSERT(result.quit == 7 && app.loop().exit_code() == 7);
    ASSERT(root.keys == 1);
}

TEST(routing_widget_timer_results_and_ancestor_eligibility_control_paint)
{
    auto root = Probe{};
    root.size = {4, 2};
    auto& parent = add(root, {0, 0}, {4, 2});
    auto& child = add(parent, {0, 0}, {4, 2});
    auto app = Application{root};
    settle(app);
    auto timer = Timer{child, 10ms};
    auto const now = EventLoop::Clock::time_point{};
    timer.start(now);
    app.step(now + 10ms);
    ASSERT(child.ticks == 1 && !app.loop().needs_repaint());
    parent.enabled = false;
    app.step(now + 20ms);
    ASSERT(child.ticks == 1);
    parent.enabled = true;
    child.reply = {.repaint = true};
    app.step(now + 30ms);
    ASSERT(child.ticks == 2 && app.loop().needs_repaint());
    parent.children.clear();
    ASSERT(!timer.is_running());
    app.step(now + 40ms);
}

TEST(routing_builtin_widgets_ignore_unused_keys_and_partial_paste_without_repaint)
{
    auto edit = LineEdit{};
    edit.size = {8, 1};
    auto app = Application{edit};
    settle(app);
    ASSERT(!app.handle_key_press(KeyEvent{.key = KeyCode::Escape}).handled);
    ASSERT(!app.paint());
    ASSERT(!app.handle_key_press(KeyEvent{.key = KeyCode::ArrowLeft}).repaint);
    ASSERT(!app.handle_paste({"hel", true, false}).repaint);
    ASSERT(app.handle_paste({"lo", false, true}).repaint && edit.text() == "hello");
    auto button = BasicButton{};
    ASSERT(!button.key_press(KeyEvent{.key = KeyCode::Escape}).handled);
    ASSERT(!button.mouse_press({{0, 0}, MouseButton::Right}).handled);
    auto box = TextBox{{.text = "short"}};
    box.size = {8, 8};
    box.paint(Canvas{app.loop().screen});
    ASSERT(box.mouse_press({{0, 7}, MouseButton::Left}).handled);
}

TEST(routing_checked_geometry_rejects_overflow_without_hit_test_overflow)
{
    auto root = Probe{};
    root.size = {8, 2};
    auto& child = add(root, {std::numeric_limits<int>::max(), 0}, {8, 2});
    ASSERT(hit_path(widget_tree(root), {7, 0}) == std::vector<std::size_t>{0});
    add(child, {1, 0}, {1, 1});
    ASSERT_THROWS(widget_tree(root), std::overflow_error);
}

TEST(routing_focus_out_relocation_reacquires_pending_target)
{
    auto root = Row<std::vector<Probe>>{};
    root.children.emplace_back(FocusPolicy::Strong);
    root.children.emplace_back(FocusPolicy::Strong);
    root.size = {10, 2};
    root.resize({});
    auto app = Application{root};
    auto target = track(root.children[1]);
    root.children[0].out_action = [&] {
        root.children.reserve(root.children.capacity() + 20);
        return EventResponse{};
    };
    app.focus().set(root.children[1]);
    ASSERT(target.valid() && target.get().focused && target.get().focus_ins == 1);
    ASSERT(app.focus().get() == target);
}

TEST(routing_focus_and_hover_mutations_precede_mouse_delivery)
{
    auto root = Probe{};
    root.size = {6, 2};
    auto& first = add(root, {0, 0}, {3, 2}, FocusPolicy::Strong);
    auto& second = add(root, {3, 0}, {3, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    first.out_action = [&] {
        second.enabled = false;
        return EventResponse{};
    };
    app.handle_mouse_press({{4, 0}, MouseButton::Left});
    ASSERT(second.presses.empty());
    ASSERT(first.focused && !second.focused);
    first.out_action = {};
    second.enabled = true;
    second.enter_action = [&] {
        second.enabled = false;
        return EventResponse{};
    };
    app.handle_mouse_move({.at = {-1, 0}, .held_button = {}});
    app.handle_mouse_press({{4, 0}, MouseButton::Left});
    ASSERT(second.presses.empty());
}

TEST(routing_focused_cursor_uses_the_painted_clip_and_disappears_when_disabled)
{
    auto root = Probe{};
    root.size = {8, 4};
    auto& parent = add(root, {2, 1}, {3, 2});
    auto& child = add(parent, {-2, -1}, {8, 5}, FocusPolicy::Strong);
    child.cursor = Point{0, 0};
    auto app = Application{root};
    app.post(esc::Resize{root.size});
    app.step();
    app.paint();
    ASSERT(!app.loop().cursor);
    child.cursor = Point{2, 1};
    app.request_repaint();
    app.paint();
    ASSERT((app.loop().cursor == Point{2, 1}));
    child.enabled = false;
    app.step();
    app.paint();
    ASSERT(!app.loop().cursor && !child.focused);
}

TEST(routing_failed_initial_focus_detaches_and_restores_widget_flags)
{
    auto root = Probe{FocusPolicy::Strong};
    root.size = {4, 2};
    root.in_action = []() -> EventResponse {
        throw std::runtime_error{"focus callback"};
    };
    ASSERT_THROWS(Application{root}, std::runtime_error);
    ASSERT(!root.focused);
    root.in_action = {};
    auto app = Application{root};
    ASSERT(root.focused && app.focus().get() == track(root));
}
