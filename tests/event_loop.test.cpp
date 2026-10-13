#include <zzz/test.hpp>

#include <chrono>
#include <memory>
#include <thread>

#ifndef _WIN32
#    include <unistd.h>
#endif

#include <ox/ox.hpp>

using namespace ox;
using namespace std::chrono_literals;

namespace {
struct Handler {
    int events = 0;
    int paints = 0;
    bool redraw = false;
    auto handle_key_press(KeyEvent) -> EventResponse
    {
        ++events;
        return {.handled = true, .repaint = redraw};
    }
    auto handle_paint(Canvas) -> Cursor
    {
        ++paints;
        return {};
    }
};
}  // namespace

TEST(handled_events_do_not_implicitly_request_paint)
{
    auto loop = EventLoop{};
    auto handler = Handler{};
    loop.post(KeyEvent{.key = U'x'});
    auto const result = loop.step(handler);
    ASSERT(result.handled);
    ASSERT(!result.repaint);
    ASSERT(!loop.paint(handler));
    ASSERT(handler.paints == 0);
    loop.request_repaint();
    ASSERT(loop.paint(handler));
    ASSERT(!loop.paint(handler));
    ASSERT(handler.paints == 1);
}

TEST(event_batches_paint_once_and_defer_posts_from_callbacks)
{
    auto loop = EventLoop{};
    auto handler = Handler{.redraw = true};
    for (int i = 0; i < 20; ++i) {
        loop.post(KeyEvent{.key = U'x'});
    }
    loop.post(event::Custom{[&] {
        loop.post(KeyEvent{.key = U'y'});
        return EventResponse{.handled = true};
    }});
    loop.step(handler);
    ASSERT(handler.events == 20);
    ASSERT(loop.paint(handler));
    ASSERT(handler.paints == 1);
    loop.step(handler);
    ASSERT(handler.events == 21);
    ASSERT(loop.paint(handler));
    ASSERT(handler.paints == 2);
}

TEST(scheduled_timers_use_deadlines_and_skip_missed_ticks)
{
    auto loop = EventLoop{};
    auto handler = Handler{};
    auto const now = EventLoop::Clock::time_point{};
    int ticks = 0;
    auto timer = Timer{10ms,
                       [&] {
                           ++ticks;
                           return EventResponse{.handled = true, .repaint = true};
                       },
                       false};
    loop.watch(timer);
    loop.watch(timer);
    timer.start(now);
    ASSERT(loop.wait_duration(now) == 10ms);
    loop.step(handler, now + 9ms);
    ASSERT(ticks == 0);
    loop.step(handler, now + 10ms);
    ASSERT(ticks == 1);
    loop.step(handler, now + 95ms);
    ASSERT(ticks == 2);
    loop.paint(handler);
    ASSERT(loop.wait_duration(now + 95ms) == 5ms);
    timer.stop();
    loop.step(handler, now + 100ms);
    ASSERT(ticks == 2);
    timer.start(now + 100ms);
    loop.step(handler, now + 110ms);
    ASSERT(ticks == 3);
}

TEST(timer_callbacks_can_cancel_other_due_timers_and_register_new_timers)
{
    auto loop = EventLoop{};
    auto handler = Handler{};
    auto const now = EventLoop::Clock::time_point{};
    int ticks = 0;
    auto second = Timer{10ms,
                        [&] {
                            ++ticks;
                            return EventResponse{};
                        },
                        false};
    std::optional<Timer> added;
    auto first = Timer{10ms,
                       [&] {
                           second.stop();
                           added.emplace(
                               10ms,
                               [&] {
                                   ++ticks;
                                   return EventResponse{};
                               },
                               false);
                           added->start(now + 10ms);
                           loop.watch(*added);
                           return EventResponse{};
                       },
                       false};
    loop.watch(first);
    loop.watch(second);
    first.start(now);
    second.start(now);
    loop.step(handler, now + 10ms);
    ASSERT(ticks == 0);
    first.stop();
    loop.step(handler, now + 20ms);
    ASSERT(ticks == 1);
}

TEST(timer_handles_cancel_on_destruction_and_preserve_schedule_when_moved)
{
    auto loop = EventLoop{};
    auto handler = Handler{};
    auto const now = EventLoop::Clock::time_point{};
    int ticks = 0;
    {
        auto timer = Timer{10ms,
                           [&] {
                               ++ticks;
                               return EventResponse{};
                           },
                           false};
        timer.start(now);
        loop.watch(timer);
        auto moved = std::move(timer);
        loop.step(handler, now + 10ms);
        ASSERT(ticks == 1);
    }
    loop.step(handler, now + 100ms);
    ASSERT(ticks == 1);
    ASSERT_THROWS((Timer{0ms, [] { return EventResponse{}; }}), std::invalid_argument);
}

TEST(quit_stops_dispatch_and_suppresses_pending_paint)
{
    auto loop = EventLoop{};
    auto handler = Handler{.redraw = true};
    loop.post(event::Custom{
        [] { return EventResponse{.handled = true, .repaint = true, .quit = 7}; }});
    loop.post(KeyEvent{.key = U'x'});
    auto result = loop.step(handler);
    ASSERT(result.quit == 7);
    ASSERT(loop.exit_code() == 7);
    ASSERT(handler.events == 0);
    ASSERT(!loop.paint(handler));
    loop.step(handler);
    ASSERT(handler.events == 0);
}

TEST(worker_posts_are_owned_by_the_target_loop)
{
    auto first = EventLoop{};
    auto second = EventLoop{};
    auto first_handler = Handler{};
    auto second_handler = Handler{};
    {
        auto worker = std::jthread{[&] {
            for (int i = 0; i < 100; ++i) {
                first.post(KeyEvent{.key = U'x'});
            }
        }};
    }
    first.step(first_handler);
    second.step(second_handler);
    ASSERT(first_handler.events == 100);
    ASSERT(second_handler.events == 0);
}

TEST(applications_own_focus_frames_and_exit_requests_without_a_terminal)
{
    auto first_edit = LineEdit{};
    auto second_edit = LineEdit{};
    auto first = Application{first_edit};
    auto second = Application{second_edit};
    first.post(esc::Resize{{10, 1}});
    second.post(esc::Resize{{10, 1}});
    first.post(KeyEvent{.key = U'a'});
    second.post(KeyEvent{.key = U'b'});
    first.step();
    second.step();
    ASSERT(first_edit.text() == "a");
    ASSERT(second_edit.text() == "b");
    ASSERT(&first.focus().get().get() == &first_edit);
    ASSERT(&second.focus().get().get() == &second_edit);
    ASSERT(first.paint());
    ASSERT(second.paint());
    ASSERT((first.loop().screen[Point{0, 0}].text == U"a"));
    ASSERT((second.loop().screen[Point{0, 0}].text == U"b"));
    first.quit(3);
    ASSERT(first.loop().exit_code() == 3);
    ASSERT(first.run() == 3);  // An already stopped application opens no terminal.
    ASSERT(!second.loop().exit_code());
}

TEST(widget_timers_follow_moved_widgets_and_ignore_destroyed_targets)
{
    struct Counter : Widget {
        int ticks = 0;
        auto timer() -> EventResponse override
        {
            ++ticks;
            return {.handled = true, .repaint = true};
        }
    };
    auto original = Counter{};
    auto timer = Timer{original, 10ms};
    auto widget = std::move(original);
    auto app = Application{widget};
    auto const now = EventLoop::Clock::time_point{};
    timer.start(now);
    app.step(now + 10ms);
    ASSERT(widget.ticks == 1);

    auto loop = EventLoop{};
    auto handler = Handler{};
    auto target = std::make_unique<Counter>();
    auto orphan = Timer{*target, 10ms};
    loop.watch(orphan);
    orphan.start(now);
    target.reset();
    loop.step(handler, now + 10ms);
    ASSERT(!loop.needs_repaint());
}

TEST(timers_belong_to_one_loop_and_can_be_reused_after_release)
{
    auto first = EventLoop{};
    auto second = EventLoop{};
    auto handler = Handler{};
    auto const now = EventLoop::Clock::time_point{};
    int ticks = 0;
    auto timer = Timer{10ms,
                       [&] {
                           ++ticks;
                           return EventResponse{};
                       },
                       false};
    first.watch(timer);
    ASSERT_THROWS(second.watch(timer), std::logic_error);
    timer.start(now);
    first.step(handler, now + 10ms);
    ASSERT(ticks == 1);
    first.unwatch(timer);
    ASSERT(!timer.is_running());
    second.watch(timer);
    timer.start(now);
    first.step(handler, now + 10ms);
    second.step(handler, now + 10ms);
    ASSERT(ticks == 2);
    timer.stop();
    // Destroying the owning loop releases its surviving handles.
    auto reused = Timer{10ms,
                        [&] {
                            ++ticks;
                            return EventResponse{};
                        },
                        false};
    {
        auto temporary = EventLoop{};
        temporary.watch(reused);
    }
    second.watch(reused);
    reused.start(now);
    second.step(handler, now + 10ms);
    ASSERT(ticks == 3);
}

TEST(detached_widgets_stop_their_timers_and_can_join_another_application)
{
    struct Counter : Widget {
        int ticks = 0;
        auto timer() -> EventResponse override
        {
            ++ticks;
            return {.handled = true, .repaint = true};
        }
    };
    auto root = Column<std::vector<Counter>>{};
    root.children.emplace_back();
    auto timer = Timer{root.children.front(), 10ms};
    auto first = Application{root};
    auto const now = EventLoop::Clock::time_point{};
    timer.start(now);
    first.step(now + 10ms);
    ASSERT(root.children.front().ticks == 1);
    auto detached = std::move(root.children.front());
    root.children.clear();
    first.step(now + 20ms);
    ASSERT(!timer.is_running());
    auto second = Application{detached};
    timer.start(now + 20ms);
    first.step(now + 30ms);
    second.step(now + 30ms);
    ASSERT(detached.ticks == 2);
}

TEST(event_loop_wait_wakes_for_worker_posts_and_resets_between_batches)
{
    auto loop = EventLoop{};
    auto handler = Handler{};
    for (int batch = 0; batch < 10; ++batch) {
        auto worker = std::jthread{[&] {
            std::this_thread::sleep_for(2ms);
            for (int i = 0; i < 100; ++i) {
                loop.post(KeyEvent{.key = U'x'});
            }
        }};
        auto const start = EventLoop::Clock::now();
        loop.wait({.source = {}, .deadline = start + 2s});
        ASSERT(EventLoop::Clock::now() - start < 1s);
        worker.join();
        loop.step(handler);
        ASSERT(handler.events == (batch + 1) * 100);
    }
    // No stale wakeup remains once the last batch has been consumed.
    auto const start = EventLoop::Clock::now();
    loop.wait({.source = {}, .deadline = start + 10ms});
    ASSERT(EventLoop::Clock::now() - start >= 10ms);
}

TEST(event_loop_wait_observes_ready_input_queue_repaint_quit_and_timer_deadlines)
{
    auto loop = EventLoop{};
    auto handler = Handler{};
    loop.wait({.source = {}, .ready = true, .deadline = {}});
    loop.wait({.source = {}, .deadline = EventLoop::Clock::now()});
    loop.post(KeyEvent{.key = U'x'});
    loop.wait();
    loop.step(handler);
    loop.request_repaint();
    loop.wait();
    loop.paint(handler);
    int ticks = 0;
    auto timer = Timer{10ms,
                       [&] {
                           ++ticks;
                           return EventResponse{};
                       },
                       false};
    loop.watch(timer);
    timer.start();
    loop.wait();
    loop.step(handler);
    ASSERT(ticks == 1);
    loop.quit();
    loop.wait();
}

TEST(event_loop_wait_preserves_posts_from_callbacks)
{
    auto loop = EventLoop{};
    auto handler = Handler{};
    loop.post(event::Custom{[&] {
        loop.post(KeyEvent{.key = U'x'});
        return EventResponse{};
    }});
    loop.step(handler);
    ASSERT(handler.events == 0);
    loop.wait();
    loop.step(handler);
    ASSERT(handler.events == 1);
}

#ifndef _WIN32
TEST(event_loop_wait_observes_borrowed_input_without_consuming_it)
{
    int descriptors[2];
    ASSERT(::pipe(descriptors) == 0);
    auto loop = EventLoop{};
    auto worker = std::jthread{[&] {
        std::this_thread::sleep_for(2ms);
        char byte = 'x';
        (void)::write(descriptors[1], &byte, 1);
    }};
    auto const start = EventLoop::Clock::now();
    loop.wait({.source = {esc::WaitSourceKind::FileDescriptor, descriptors[0]},
               .deadline = start + 2s});
    worker.join();
    char byte = 0;
    ASSERT(EventLoop::Clock::now() - start < 1s);
    ASSERT(::read(descriptors[0], &byte, 1) == 1);
    ASSERT(byte == 'x');
    ::close(descriptors[0]);
    ::close(descriptors[1]);
}
#endif
