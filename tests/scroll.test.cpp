#include <zzz/test.hpp>

#include <limits>
#include <random>

#include <ox/ox.hpp>

using namespace ox;

TEST(scroll_functions_clamp_reveal_and_handle_extreme_coordinates)
{
    auto const high = std::numeric_limits<int>::max();
    auto const low = std::numeric_limits<int>::min();
    ASSERT(scroll_limit(10, 4) == 6);
    ASSERT(scroll_limit(2, 4) == 0);
    ASSERT(scroll_limit(10, 0) == 10);
    ASSERT(scroll_by(high, low, 10, 4) == 0);
    ASSERT(scroll_by(low, high, 10, 4) == 6);
    ASSERT(scroll_to_show(0, 8, 1, 10, 4) == 5);
    ASSERT(scroll_to_show(6, 1, 1, 10, 4) == 1);
    ASSERT(scroll_to_show(0, 3, 8, 10, 4) == 3);
    ASSERT(scroll_to_show(2, high, high, 10, 4) == 6);
    ASSERT(scroll_to_show(2, high, 0, 10, 4) == 2);
    ASSERT((scrollbar_thumb(6, 10, 4, 10) == std::pair{6, 4}));
    ASSERT((scrollbar_thumb(1, 101, 100, 10) == std::pair{1, 9}));
    ASSERT((scrollbar_thumb(0, 0, 0, 10) == std::pair{0, 10}));
    ASSERT((scrollbar_thumb(0, 10, 4, 0) == std::pair{0, 0}));
    ASSERT(scrollbar_position(low, 10, 4, 10) == 0);
    ASSERT(scrollbar_position(high, 10, 4, 10) == 6);
    ASSERT((scrollbar_thumb(high, high, 1, high) == std::pair{high - 1, 1}));
    ASSERT_THROWS(scroll_limit(-1, 0), std::invalid_argument);
    ASSERT_THROWS(scroll_limit(1, -1), std::invalid_argument);
    ASSERT_THROWS(scroll_to_show(0, -1, 1, 1, 1), std::invalid_argument);
    ASSERT_THROWS(scrollbar_thumb(0, 1, 1, -1), std::invalid_argument);
}

TEST(scrollbar_geometry_randomly_stays_inside_track_and_reaches_both_ends)
{
    auto random = std::mt19937{2026};
    for (int i = 0; i < 2000; ++i) {
        auto const content = (int)(random() % 100000);
        auto const viewport = (int)(random() % 100000);
        auto const track = (int)(random() % 1000);
        auto const limit = scroll_limit(content, viewport);
        auto const [first, length] = scrollbar_thumb(0, content, viewport, track);
        auto const [last, other] = scrollbar_thumb(limit, content, viewport, track);
        ASSERT(first == 0 && length == other && last + length == track);
        ASSERT(length >= 0 && length <= track);
        auto const [position, size] =
            scrollbar_thumb((int)(random() % 100000), content, viewport, track);
        ASSERT(position >= 0 && position + size <= track);
    }
}

TEST(scrollbar_commands_draw_full_empty_and_horizontal_ranges_without_duplicate_signals)
{
    auto bar = ScrollBar{{.content_length = 10, .viewport_length = 4}};
    arrange(bar, {{}, {1, 10}});
    auto app = Application{bar};
    app.paint();
    int notifications = 0;
    bar.on_scroll.connect([&](int) { ++notifications; });
    ASSERT(bar.set_position(100));
    ASSERT(bar.position() == 6 && notifications == 1);
    ASSERT(!bar.set_position(100));
    auto frame = Frame{bar.size()};
    bar.paint(Canvas{frame});
    ASSERT((frame[Point{0, 5}].text == U" "));
    ASSERT((frame[Point{0, 6}].text == U"█"));
    bar.set_range(0, 4, 10);
    bar.paint(Canvas{frame});
    ASSERT(bar.position() == 0 && notifications == 1);
    ASSERT((frame[Point{0, 9}].text == U"█"));
    bar.vertical = false;
    arrange(bar, {{}, {10, 1}});
    bar.set_range(10, 4, 0);
    bar.handle(MousePress{{9, 0}, MouseButton::Left});
    ASSERT(bar.position() == 6);
    bar.handle(MouseWheel{{}, -2, 0});
    ASSERT(bar.position() == 4);
}

TEST(scrollbar_animation_follows_moves_and_feedback_does_not_stop_it)
{
    auto bar = ScrollBar{{.content_length = 100,
                          .viewport_length = 10,
                          .scroll_settle_time = std::chrono::milliseconds{100}}};
    arrange(bar, {{}, {1, 10}});
    bar.handle(MousePress{{0, 9}, MouseButton::Left});
    auto moved = std::move(bar);
    auto app = Application{moved};
    moved.on_scroll.connect([&](int position) { moved.set_range(100, 10, position); });
    app.step(EventLoop::Clock::now() + std::chrono::milliseconds{20});
    // Timer scheduling is injectable, animation time is elapsed steady time.
    moved.scroll_settle_time = std::chrono::milliseconds{0};
    app.step(EventLoop::Clock::now() + std::chrono::milliseconds{50});
    ASSERT(moved.position() == 90);
}
