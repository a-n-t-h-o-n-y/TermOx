#include <zzz/test.hpp>

#include <ox/ox.hpp>

using namespace ox;
using namespace std::chrono_literals;

namespace {
struct Counter : Widget {
    using Widget::handle;
    int keys = 0;
    int first = 0;
    int second = 0;
    EventResponse reply;
    Counter() : Widget{FocusPolicy::Strong} {}
    auto handle(KeyEvent const&) -> EventResponse override
    {
        ++keys;
        return reply;
    }
};
struct LayoutProbe : Widget {
    std::vector<Rect> previous;
    void on_arrange(Rect bounds) override { previous.push_back(bounds); }
};
}  // namespace

TEST(widget_arrangement_validates_bounds_and_exposes_previous_geometry)
{
    auto widget = LayoutProbe{};
    arrange(widget, {{2, 3}, {10, 4}});
    ASSERT((widget.bounds() == Rect{{2, 3}, {10, 4}}));
    ASSERT((widget.previous == std::vector<Rect>{{{0, 0}, {0, 0}}}));
    arrange(widget, widget.bounds());
    ASSERT(widget.previous.size() == 2);
    ASSERT(widget.previous.back() == widget.bounds());
    ASSERT_THROWS((arrange(widget, {{7, 8}, {-1, 4}})), std::invalid_argument);
    ASSERT((widget.bounds() == Rect{{2, 3}, {10, 4}}));
    ASSERT(widget.previous.size() == 2);
}

TEST(widget_handlers_return_effects_and_application_dispatch_applies_them)
{
    auto widget = Counter{};
    arrange(widget, {{0, 0}, {10, 1}});
    auto app = Application{widget};
    app.paint();
    widget.reply = {.handled = true, .repaint = true, .quit = 7};
    auto const response = app.handle(KeyEvent{.key = U'x'});
    ASSERT(response.handled && response.repaint && response.quit == 7);
    ASSERT(!app.loop().needs_repaint() && !app.loop().exit_code());
    auto const applied = app.dispatch(KeyEvent{.key = U'x'});
    ASSERT(applied.quit == 7 && app.loop().needs_repaint());
    ASSERT(app.loop().exit_code() == 7 && widget.keys == 2);
}

TEST(application_focus_commands_apply_effects_and_focus_state_is_read_only)
{
    auto row = Row{Counter{}, Counter{}};
    arrange(row, {{0, 0}, {10, 1}});
    auto app = Application{row};
    app.paint();
    auto& first = get_child<0>(row);
    auto& second = get_child<1>(row);
    ASSERT(first.is_focused());
    app.set_focus(second);
    ASSERT(second.is_focused() && !first.is_focused());
    ASSERT(app.loop().needs_repaint());
    app.paint();
    app.clear_focus();
    ASSERT(!second.is_focused() && app.focus().get().try_get() == nullptr);
    ASSERT(app.loop().needs_repaint());
}

TEST(tab_releases_are_delivered_without_advancing_focus)
{
    auto row = Row{Counter{}, Counter{}};
    arrange(row, {{0, 0}, {10, 1}});
    auto app = Application{row};
    app.dispatch(KeyEvent{.key = KeyCode::Tab, .action = KeyAction::Release});
    ASSERT(get_child<0>(row).is_focused() && get_child<0>(row).keys == 1);
    app.dispatch(KeyEvent{.key = KeyCode::Tab, .action = KeyAction::Repeat});
    ASSERT(get_child<1>(row).is_focused() && get_child<1>(row).keys == 0);
}

TEST(widget_timers_have_independent_callbacks_and_follow_widget_moves)
{
    auto original = Counter{};
    arrange(original, {{0, 0}, {10, 1}});
    auto first = Timer{original, 10ms, [](Counter& widget) {
                           ++widget.first;
                           return EventResponse{};
                       }};
    auto second = Timer{original, 20ms, [](Counter& widget) {
                            ++widget.second;
                            return EventResponse{.repaint = true};
                        }};
    auto moved = std::move(original);
    auto app = Application{moved};
    auto const now = EventLoop::Clock::time_point{};
    first.start(now);
    second.start(now);
    app.step(now + 10ms);
    ASSERT(moved.first == 1 && moved.second == 0);
    app.step(now + 20ms);
    ASSERT(moved.first == 2 && moved.second == 1);
    second.stop();
    app.step(now + 30ms);
    ASSERT(moved.first == 3 && moved.second == 1);
}

TEST(typed_lifetime_lookup_rejects_sliced_targets_without_dangling_casts)
{
    auto original = Counter{};
    auto derived = track(original);
    auto base = track(static_cast<Widget&>(original));
    auto calls = 0;
    auto timer = Timer{original, 10ms, [&](Counter&) {
                           ++calls;
                           return EventResponse{};
                       }};
    auto sliced = Widget{std::move(original)};
    ASSERT(!derived.valid() && derived.try_get() == nullptr);
    ASSERT(base.try_get() == &sliced);
    auto loop = EventLoop{};
    struct Handler {
    } handler;
    loop.watch(timer);
    auto const now = EventLoop::Clock::time_point{};
    timer.start(now);
    loop.step(handler, now + 10ms);
    ASSERT(calls == 0);
}

TEST(widget_timers_reject_empty_callbacks_and_moved_handles_are_well_defined)
{
    auto widget = Counter{};
    auto empty = std::function<EventResponse(Counter&)>{};
    ASSERT_THROWS((Timer{widget, 10ms, empty}), std::invalid_argument);
    auto timer = Timer{10ms, [] { return EventResponse{}; }};
    auto moved = std::move(timer);
    timer.stop();
    ASSERT(!timer.is_running() && moved.is_running());
    ASSERT_THROWS(timer.start(), std::logic_error);
    ASSERT_THROWS(timer.duration(), std::logic_error);
}

TEST(built_in_actions_ignore_key_releases)
{
    auto button = Button{};
    int presses = 0;
    button.on_press.connect([&] { ++presses; });
    auto release = KeyEvent{.key = KeyCode::Enter, .action = KeyAction::Release};
    ASSERT(!button.handle(release).handled && presses == 0);
    auto checkbox = CheckBox{};
    auto const state = checkbox.checked();
    ASSERT(!checkbox.handle(release).handled && checkbox.checked() == state);
    auto group = RadioGroup{{.items = {"one", "two"}}};
    auto const selection = group.selected_index();
    ASSERT(!group.handle(release).handled && group.selected_index() == selection);
}

TEST(typed_dispatch_supports_new_event_handlers_without_handler_concepts)
{
    struct Observer {
        int events = 0;
        auto handle(esc::Focus const&) -> EventResponse
        {
            ++events;
            return {.handled = true};
        }
    } observer;
    auto screen = Frame{};
    ASSERT(dispatch(esc::Focus{true}, observer, screen).handled);
    ASSERT(observer.events == 1);
    ASSERT(!dispatch(KeyEvent{.key = U'x'}, observer, screen).handled);
}

TEST(standalone_focus_returns_effects_and_releases_its_owned_focus_state)
{
    auto widget = Counter{};
    arrange(widget, {{0, 0}, {10, 1}});
    {
        auto focus = Focus{widget};
        auto const response = focus.set(widget);
        ASSERT(response.repaint && widget.is_focused());
        ASSERT(focus.get() == track(widget));
    }
    ASSERT(!widget.is_focused());
}

TEST(event_loop_effects_preserve_the_first_exit_across_explicit_commands)
{
    auto loop = EventLoop{};
    loop.apply({.quit = 7});
    loop.quit(9);
    ASSERT(loop.exit_code() == 7);
    auto other = EventLoop{};
    other.quit(0);
    other.apply({.quit = 5});
    ASSERT(other.exit_code() == 0);
    other.request_repaint();
    ASSERT(other.needs_repaint());
}
