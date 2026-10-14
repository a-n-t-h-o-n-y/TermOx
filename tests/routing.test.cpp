#include <zzz/test.hpp>

#include <functional>
#include <limits>
#include <memory>

#include <ox/ox.hpp>

using namespace ox;
using namespace std::chrono_literals;

namespace {
struct Probe : Widget {
   public:
    using Widget::handle;
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
    Cursor paint_cursor;
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
    auto handle(MousePress const& m) -> EventResponse override
    {
        presses.push_back(m.at);
        return reply;
    }
    auto handle(MouseRelease const&) -> EventResponse override { return reply; }
    auto handle(MouseWheel const&) -> EventResponse override { return reply; }
    auto handle(MouseMove const& m) -> EventResponse override
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
    auto handle(KeyEvent const& event) -> EventResponse override
    {
        if (event.action == KeyAction::Release) {
            ++releases;
            return reply;
        }
        ++keys;
        auto action = key_action;
        return action ? action() : reply;
    }
    auto handle(esc::PasteChunk const&) -> EventResponse override
    {
        ++pastes;
        return reply;
    }
    auto tick() -> EventResponse
    {
        ++ticks;
        return reply;
    }
    auto paint(Canvas canvas) -> Cursor override
    {
        if (glyph) { fill(canvas, *glyph); }
        return paint_cursor;
    }
};

auto add(Probe& parent, Point at, Area size, FocusPolicy policy = FocusPolicy::None)
    -> Probe&
{
    auto& child = parent.children.emplace_back(std::make_unique<Probe>(policy));
    arrange(*child, {at, size});
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
    arrange(root, {root.at(), {8, 4}});
    auto& parent = add(root, {2, 1}, {3, 2});
    auto& child = add(parent, {-2, -1}, {8, 5});
    child.glyph = U'x';
    child.reply = {.handled = true};
    auto app = Application{root};
    app.post(esc::Resize{root.size()});
    app.step();
    ASSERT(app.paint());
    ASSERT((app.loop().screen[Point{2, 1}].text == U"x"));
    ASSERT((app.loop().screen[Point{1, 1}].text == U" "));
    ASSERT((app.loop().screen[Point{5, 1}].text == U" "));
    app.dispatch(MousePress{{2, 1}, MouseButton::Left});
    ASSERT((child.presses == std::vector<Point>{{2, 1}}));
    app.dispatch(MousePress{{1, 1}, MouseButton::Left});
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
    arrange(root, {root.at(), {12, 5}});
    auto& parent = add(root, {3, 1}, {6, 3});
    auto& child = add(parent, {2, 1}, {2, 1});
    parent.reply = {.handled = true};
    child.reply = {.repaint = true};
    auto app = Application{root};
    auto const result = app.dispatch(MousePress{{5, 2}, MouseButton::Right});
    ASSERT(result.handled && result.repaint);
    ASSERT((child.presses.back() == Point{0, 0}));
    ASSERT((parent.presses.back() == Point{2, 1}));
    ASSERT(root.presses.empty());
}

TEST(routing_later_siblings_win_overlap_and_outside_input_is_ignored)
{
    auto root = Probe{};
    arrange(root, {root.at(), {5, 2}});
    auto& first = add(root, {0, 0}, {5, 2});
    auto& last = add(root, {0, 0}, {5, 2});
    first.reply = last.reply = {.handled = true};
    auto app = Application{root};
    app.dispatch(MousePress{{1, 0}, MouseButton::Left});
    ASSERT(last.presses.size() == 1 && first.presses.empty());
    app.dispatch(MousePress{{-1, 0}, MouseButton::Left});
    app.dispatch(MousePress{{5, 0}, MouseButton::Left});
    ASSERT(last.presses.size() == 1 && root.presses.empty());
}

TEST(routing_hidden_and_disabled_ancestors_prune_all_input_and_focus)
{
    auto root = Probe{};
    arrange(root, {root.at(), {10, 3}});
    auto& hidden = add(root, {0, 0}, {3, 3});
    hidden.visible = false;
    auto& hidden_child = add(hidden, {0, 0}, {3, 3}, FocusPolicy::Strong);
    auto& disabled = add(root, {3, 0}, {3, 3});
    disabled.enabled = false;
    auto& disabled_child = add(disabled, {0, 0}, {3, 3}, FocusPolicy::Strong);
    disabled_child.glyph = U'd';
    auto& chosen = add(root, {6, 0}, {4, 3}, FocusPolicy::Tab);
    auto app = Application{root};
    ASSERT(app.focus().get() == track(chosen));
    ASSERT_THROWS(app.set_focus(hidden_child), std::invalid_argument);
    ASSERT_THROWS(app.set_focus(disabled_child), std::invalid_argument);
    app.dispatch(MousePress{{0, 0}, MouseButton::Left});
    app.dispatch(MousePress{{3, 0}, MouseButton::Left});
    ASSERT(hidden_child.presses.empty() && disabled_child.presses.empty());
    app.post(esc::Resize{root.size()});
    app.step();
    app.paint();
    ASSERT((app.loop().screen[Point{3, 0}].text == U"d"));
    ASSERT((app.loop().screen[Point{0, 0}].text == U" "));
}

TEST(routing_hover_notifies_only_changed_ancestry_and_handles_leaving_root)
{
    auto root = Probe{};
    arrange(root, {root.at(), {8, 2}});
    auto& parent = add(root, {0, 0}, {8, 2});
    auto& left = add(parent, {0, 0}, {4, 2});
    auto& right = add(parent, {4, 0}, {4, 2});
    auto app = Application{root};
    app.dispatch(MouseMove{.at = {1, 0}, .held_button = {}});
    app.dispatch(MouseMove{.at = {2, 0}, .held_button = {}});
    ASSERT(root.enters == 1 && parent.enters == 1 && left.enters == 1);
    ASSERT(root.leaves == 0 && parent.leaves == 0 && left.leaves == 0);
    app.dispatch(MouseMove{.at = {5, 0}, .held_button = {}});
    ASSERT(left.leaves == 1 && right.enters == 1 && parent.leaves == 0);
    app.dispatch(MouseMove{.at = {8, 0}, .held_button = {}});
    ASSERT(right.leaves == 1 && parent.leaves == 1 && root.leaves == 1);
    app.dispatch(MouseMove{.at = {9, 0}, .held_button = {}});
    ASSERT(root.leaves == 1);
}

TEST(routing_hover_reconciles_geometry_and_activation_without_pointer_motion)
{
    auto root = Probe{};
    arrange(root, {root.at(), {8, 2}});
    auto& child = add(root, {0, 0}, {4, 2});
    auto app = Application{root};
    app.dispatch(MouseMove{.at = {1, 0}, .held_button = {}});
    child.visible = false;
    app.step();
    ASSERT(child.leaves == 1);
    child.visible = true;
    app.step();
    ASSERT(child.enters == 2);
    arrange(child, {{4, 0}, child.size()});
    app.step();
    ASSERT(child.leaves == 2);
    root.children.clear();
    app.step();  // No callback through the destroyed hover path.
}

TEST(routing_keyboard_release_and_paste_bubble_from_focus)
{
    auto root = Probe{};
    arrange(root, {root.at(), {4, 2}});
    root.reply = {.handled = true};
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    settle(app);
    ASSERT(app.dispatch(KeyEvent{.text = U"x"}).handled);
    ASSERT(child.keys == 1 && root.keys == 1);
    ASSERT(!app.loop().needs_repaint());
    ASSERT(app.dispatch(KeyEvent{.key = U'x', .action = KeyAction::Release}).handled);
    ASSERT(child.releases == 1 && root.releases == 1);
    ASSERT(app.dispatch(esc::PasteChunk{"text", true, true}).handled);
    ASSERT(child.pastes == 1 && root.pastes == 1);
    child.reply = {.handled = true};
    app.dispatch(KeyEvent{.text = U"y"});
    ASSERT(child.keys == 2 && root.keys == 1);
}

TEST(routing_tab_is_consumed_wraps_and_works_after_click_focus_or_explicit_clear)
{
    auto root = Probe{};
    arrange(root, {root.at(), {8, 2}});
    auto& first = add(root, {0, 0}, {2, 2}, FocusPolicy::Tab);
    auto& click = add(root, {2, 0}, {2, 2}, FocusPolicy::Click);
    auto& last = add(root, {4, 0}, {2, 2}, FocusPolicy::Strong);
    auto& clipped = add(root, {8, 0}, {2, 2}, FocusPolicy::Tab);
    auto app = Application{root};
    app.dispatch(KeyEvent{.key = KeyCode::Tab});
    ASSERT(app.focus().get() == track(last));
    ASSERT(first.keys == 0 && last.keys == 0 && root.keys == 0);
    app.dispatch(KeyEvent{.key = KeyCode::Tab});
    ASSERT(app.focus().get() == track(first));
    app.dispatch(KeyEvent{.key = KeyCode::Tab, .modifiers = {.shift = true}});
    ASSERT(app.focus().get() == track(last));
    app.set_focus(click);
    app.dispatch(KeyEvent{.key = KeyCode::Tab});
    ASSERT(app.focus().get() == track(first));
    app.clear_focus();
    app.step();
    ASSERT(!app.focus().get().valid());
    app.dispatch(KeyEvent{.key = KeyCode::Tab, .modifiers = {.shift = true}});
    ASSERT(app.focus().get() == track(last));
    ASSERT(clipped.focus_ins == 0);
}

TEST(routing_click_uses_nearest_eligible_ancestor_and_only_left_button)
{
    auto root = Probe{FocusPolicy::Click};
    arrange(root, {root.at(), {4, 2}});
    auto& child = add(root, {0, 0}, {4, 2});
    child.reply = {.handled = true};
    auto app = Application{root};
    app.dispatch(MousePress{{1, 0}, MouseButton::Right});
    ASSERT(!root.is_focused());
    app.dispatch(MousePress{{1, 0}, MouseButton::Left});
    ASSERT(root.is_focused());
    ASSERT(app.focus().get() == track(root));
}

TEST(routing_focus_recovers_on_disable_hide_detach_and_policy_change)
{
    auto root = Probe{};
    arrange(root, {root.at(), {8, 2}});
    auto& first = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto& second = add(root, {4, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    first.enabled = false;
    app.step();
    ASSERT(!first.is_focused() && first.focus_outs == 1 && second.is_focused());
    first.enabled = true;
    second.visible = false;
    app.step();
    ASSERT(first.is_focused());
    first.focus_policy = FocusPolicy::None;
    app.step();
    ASSERT(!app.focus().get().valid());
    second.visible = true;
    app.step();
    ASSERT(second.is_focused());
    auto detached = std::move(root.children.back());
    root.children.pop_back();
    app.step();
    ASSERT(!detached->is_focused() && detached->focus_outs == 2);
    auto other = Probe{FocusPolicy::Strong};
    arrange(other, {other.at(), {1, 1}});
    ASSERT_THROWS(app.set_focus(other), std::invalid_argument);
}

TEST(routing_focus_tracks_widget_relocation_and_move_assignment_invalidates_old_target)
{
    auto root = Row<std::vector<Probe>>{};
    root.children.emplace_back(FocusPolicy::Strong);
    arrange(root, {root.at(), {10, 2}});
    auto app = Application{root};
    auto original = track(root.children.front());
    for (int i = 0; i < 10; ++i) {
        root.children.emplace_back(FocusPolicy::Strong);
    }
    arrange(root, root.bounds());
    app.step();
    ASSERT(original.valid() && app.focus().get() == original);
    ASSERT(original.get().is_focused() && original.get().focus_ins == 1);
    auto target = Probe{};
    auto old = track(target);
    auto external = track(target);
    auto source = Probe{};
    auto moved = track(source);
    target = std::move(source);
    ASSERT(!old.valid() && external.try_get() == nullptr && moved.valid());
    ASSERT(&moved.get() == &target);
    auto& same_target = target;
    target = std::move(same_target);
    ASSERT(moved.valid());
}

TEST(routing_destroyed_focus_and_external_lifetime_storage_never_dangle)
{
    auto root = Probe{};
    arrange(root, {root.at(), {4, 2}});
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    auto view = track(child);
    auto external = track(child);
    root.children.clear();
    app.step();
    ASSERT(!view.valid() && !app.focus().get().valid() &&
           external.try_get() == nullptr);
    ASSERT_THROWS(view.get(), std::logic_error);
    auto& next = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    app.step();
    ASSERT(next.is_focused());
}

TEST(routing_callback_can_destroy_the_target_and_still_bubble_safely)
{
    auto root = Probe{};
    arrange(root, {root.at(), {4, 2}});
    root.reply = {.handled = true};
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    child.key_action = [&] {
        root.children.clear();
        return EventResponse{};
    };
    ASSERT(app.dispatch(KeyEvent{.text = U"x"}).handled);
    ASSERT(root.keys == 1 && !app.focus().get().valid());
}

TEST(routing_reentrant_focus_out_can_override_the_requested_transition)
{
    auto root = Probe{};
    arrange(root, {root.at(), {6, 2}});
    auto& first = add(root, {0, 0}, {2, 2}, FocusPolicy::Strong);
    auto& second = add(root, {2, 0}, {2, 2}, FocusPolicy::Strong);
    auto& third = add(root, {4, 0}, {2, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    first.out_action = [&] {
        app.set_focus(third);
        return EventResponse{};
    };
    app.set_focus(second);
    ASSERT(third.is_focused() && !second.is_focused() && !first.is_focused());
    ASSERT(app.focus().get() == track(third));
}

TEST(routing_quit_stops_propagation_and_repaint_is_explicit)
{
    auto root = Probe{};
    arrange(root, {root.at(), {4, 2}});
    root.reply = {.handled = true};
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    settle(app);
    child.reply = {.handled = true};
    ASSERT(!app.dispatch(KeyEvent{.text = U"x"}).repaint);
    ASSERT(!app.paint());
    child.reply = {.repaint = true};
    ASSERT(app.dispatch(KeyEvent{.text = U"y"}).repaint);
    ASSERT(app.paint());
    child.reply = {.quit = 7};
    auto const result = app.dispatch(KeyEvent{.text = U"q"});
    ASSERT(result.quit == 7 && app.loop().exit_code() == 7);
    ASSERT(root.keys == 1);
}

TEST(routing_widget_timer_results_and_ancestor_eligibility_control_paint)
{
    auto root = Probe{};
    arrange(root, {root.at(), {4, 2}});
    auto& parent = add(root, {0, 0}, {4, 2});
    auto& child = add(parent, {0, 0}, {4, 2});
    auto app = Application{root};
    settle(app);
    auto timer = Timer{child, 10ms, [](auto& widget) { return widget.tick(); }};
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
    arrange(edit, {edit.at(), {8, 1}});
    auto app = Application{edit};
    settle(app);
    ASSERT(!app.dispatch(KeyEvent{.key = KeyCode::Escape}).handled);
    ASSERT(!app.paint());
    ASSERT(!app.dispatch(KeyEvent{.key = KeyCode::ArrowLeft}).repaint);
    ASSERT(!app.dispatch(esc::PasteChunk{"hel", true, false}).repaint);
    ASSERT(app.dispatch(esc::PasteChunk{"lo", false, true}).repaint &&
           edit.text() == "hello");
    auto button = BasicButton{};
    ASSERT(!button.handle(KeyEvent{.key = KeyCode::Escape}).handled);
    ASSERT(!button.handle(MousePress{{0, 0}, MouseButton::Right}).handled);
    auto box = TextBox{{.text = "short"}};
    arrange(box, {box.at(), {8, 8}});
    box.paint(Canvas{app.loop().screen});
    ASSERT(box.handle(MousePress{{0, 7}, MouseButton::Left}).handled);
}

TEST(routing_checked_geometry_rejects_overflow_without_hit_test_overflow)
{
    auto root = Probe{};
    arrange(root, {root.at(), {8, 2}});
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
    arrange(root, {root.at(), {10, 2}});
    auto app = Application{root};
    auto target = track(root.children[1]);
    root.children[0].out_action = [&] {
        root.children.reserve(root.children.capacity() + 20);
        return EventResponse{};
    };
    app.set_focus(root.children[1]);
    ASSERT(target.valid() && target.get().is_focused() && target.get().focus_ins == 1);
    ASSERT(app.focus().get() == target);
}

TEST(routing_focus_and_hover_mutations_precede_mouse_delivery)
{
    auto root = Probe{};
    arrange(root, {root.at(), {6, 2}});
    auto& first = add(root, {0, 0}, {3, 2}, FocusPolicy::Strong);
    auto& second = add(root, {3, 0}, {3, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    first.out_action = [&] {
        second.enabled = false;
        return EventResponse{};
    };
    app.dispatch(MousePress{{4, 0}, MouseButton::Left});
    ASSERT(second.presses.empty());
    ASSERT(first.is_focused() && !second.is_focused());
    first.out_action = {};
    second.enabled = true;
    second.enter_action = [&] {
        second.enabled = false;
        return EventResponse{};
    };
    app.dispatch(MouseMove{.at = {-1, 0}, .held_button = {}});
    app.dispatch(MousePress{{4, 0}, MouseButton::Left});
    ASSERT(second.presses.empty());
}

TEST(routing_focused_cursor_uses_the_painted_clip_and_disappears_when_disabled)
{
    auto root = Probe{};
    arrange(root, {root.at(), {8, 4}});
    auto& parent = add(root, {2, 1}, {3, 2});
    auto& child = add(parent, {-2, -1}, {8, 5}, FocusPolicy::Strong);
    child.paint_cursor = Point{0, 0};
    auto app = Application{root};
    app.post(esc::Resize{root.size()});
    app.step();
    app.paint();
    ASSERT(!app.loop().cursor);
    child.paint_cursor = Point{2, 1};
    app.request_repaint();
    app.paint();
    ASSERT((app.loop().cursor == Point{2, 1}));
    child.enabled = false;
    app.step();
    app.paint();
    ASSERT(!app.loop().cursor && !child.is_focused());
}

TEST(routing_failed_initial_focus_detaches_and_restores_widget_flags)
{
    auto root = Probe{FocusPolicy::Strong};
    arrange(root, {root.at(), {4, 2}});
    root.in_action = []() -> EventResponse {
        throw std::runtime_error{"focus callback"};
    };
    ASSERT_THROWS(Application{root}, std::runtime_error);
    ASSERT(!root.is_focused());
    root.in_action = {};
    auto app = Application{root};
    ASSERT(root.is_focused() && app.focus().get() == track(root));
}

TEST(routing_nested_dispatch_preserves_outer_ancestry_when_a_subtree_is_destroyed)
{
    auto root = Probe{};
    arrange(root, {root.at(), {8, 2}});
    auto& parent = add(root, {0, 0}, {4, 2});
    auto& first = add(parent, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto& second = add(root, {4, 0}, {4, 2}, FocusPolicy::Strong);
    second.reply = {.handled = true};
    root.reply = {.handled = true};
    auto app = Application{root};
    auto first_view = track(first);
    first.key_action = [&] {
        app.set_focus(second);
        ASSERT(app.dispatch(KeyEvent{.text = U"nested"}).handled);
        root.children.erase(root.children.begin());
        return EventResponse{};
    };
    ASSERT(app.dispatch(KeyEvent{.text = U"outer"}).handled);
    ASSERT(!first_view.valid() && second.is_focused());
    ASSERT(second.keys == 1 && root.keys == 1);
}

TEST(routing_tab_consumes_its_snapshot_before_reentrant_focus_callbacks)
{
    auto root = Probe{};
    arrange(root, {root.at(), {8, 2}});
    auto& first = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto& second = add(root, {4, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    first.out_action = [&] {
        for (auto i = 0; i < 200; ++i) {
            add(root, {0, 0}, {0, 0});
        }
        app.step();
        return EventResponse{};
    };
    ASSERT(app.dispatch(KeyEvent{.key = KeyCode::Tab}).handled);
    ASSERT(second.is_focused() && !first.is_focused() && root.children.size() == 202);
    ASSERT(app.focus().get() == track(second));
}

TEST(routing_mouse_bubbles_with_original_coordinates_after_siblings_are_reordered)
{
    struct MovingProbe : Probe {
        std::function<void()> move_action;
        auto handle(MouseMove const& move) -> EventResponse override
        {
            auto const response = Probe::handle(move);
            move_action();
            return response;
        }
    };
    auto root = Probe{};
    arrange(root, {root.at(), {10, 3}});
    auto& parent = add(root, {2, 1}, {4, 2});
    add(root, {0, 0}, {0, 0});
    auto child = std::make_unique<MovingProbe>();
    arrange(*child, {{1, 0}, {2, 1}});
    child->move_action = [&] {
        arrange(parent, {{4, 1}, parent.size()});
        std::ranges::reverse(root.children);
    };
    parent.children.push_back(std::move(child));
    auto app = Application{root};
    app.dispatch(MouseMove{.at = {3, 1}, .held_button = {}});
    ASSERT((parent.moves == std::vector<Point>{{1, 0}}));
    ASSERT((root.moves == std::vector<Point>{{3, 1}}));
}

TEST(routing_failed_tree_refresh_preserves_focus_and_recovers_after_correction)
{
    auto root = Probe{};
    arrange(root, {root.at(), {4, 2}});
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    auto& invalid = add(root, {std::numeric_limits<int>::max(), 0}, {1, 1});
    add(invalid, {1, 0}, {1, 1});
    ASSERT_THROWS(app.step(), std::overflow_error);
    ASSERT(child.is_focused() && app.focus().get() == track(child));
    root.children.pop_back();
    app.dispatch(KeyEvent{.text = U"x"});
    ASSERT(child.keys == 1 && root.keys == 1);
}

TEST(routing_live_detachment_and_reattachment_update_timer_ownership)
{
    auto root = Probe{};
    arrange(root, {root.at(), {4, 2}});
    auto& child = add(root, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto app = Application{root};
    auto timer = Timer{child, 10ms, [](auto& widget) { return widget.tick(); }};
    auto const now = EventLoop::Clock::time_point{};
    timer.start(now);
    auto detached = std::move(root.children.back());
    root.children.pop_back();
    app.step(now + 10ms);
    ASSERT(!timer.is_running() && child.ticks == 0 && !child.is_focused());
    root.children.push_back(std::move(detached));
    timer.start(now + 10ms);
    app.step(now + 20ms);
    ASSERT(timer.is_running() && child.ticks == 1 && child.is_focused());
}

TEST(routing_failed_attachment_keeps_rejecting_foreign_widgets_until_detached)
{
    auto first = Probe{};
    auto second = Probe{};
    arrange(first, {{0, 0}, {4, 2}});
    arrange(second, {{0, 0}, {4, 2}});
    auto& child = add(second, {0, 0}, {4, 2}, FocusPolicy::Strong);
    auto first_app = Application{first};
    auto second_app = Application{second};
    first.children.push_back(std::move(second.children.back()));
    second.children.pop_back();
    ASSERT_THROWS(first_app.step(), std::logic_error);
    ASSERT_THROWS(first_app.step(), std::logic_error);
    ASSERT(child.is_focused() && child.keys == 0);
    second_app.step();
    first_app.dispatch(KeyEvent{.text = U"x"});
    ASSERT(child.is_focused() && child.keys == 1);
    ASSERT(first_app.focus().get() == track(child) &&
           !second_app.focus().get().valid());
}
