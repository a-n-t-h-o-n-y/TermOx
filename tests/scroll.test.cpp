#include <zzz/test.hpp>

#include <chrono>
#include <limits>
#include <memory>
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

namespace {
auto thumb_row(ScrollBar& bar) -> int
{
    auto frame = Frame{bar.size()};
    bar.paint(Canvas{frame});
    for (int y = 0; y < bar.size().height; ++y) {
        if (frame[Point{0, y}].text == U"█") return y;
    }
    return -1;
}
}  // namespace

TEST(scrollbar_commits_linked_content_once_while_only_the_thumb_animates)
{
    using namespace std::chrono_literals;
    auto view = ListView{{.items = std::vector<ListItem>(100, {.text = "row"})}};
    auto bar = ScrollBar{{.scroll_settle_time = 1s}};
    auto root = Row{std::tie(view, bar)};
    auto app = Application{root};
    app.dispatch(esc::Resize{{9, 10}});
    link(view, bar);
    auto notifications = std::vector<int>{};
    bar.on_scroll.connect([&](int position) { notifications.push_back(position); });

    bar.handle(MouseWheel{{}, 0, 60});
    ASSERT(bar.position() == 60 && view.scroll_offset().y == 60);
    ASSERT(notifications == std::vector<int>{60});
    ASSERT(thumb_row(bar) == 0);
    auto now = EventLoop::Clock::now();
    app.step(now + 250ms);
    auto midway = thumb_row(bar);
    ASSERT(midway > 0 && midway < 6);
    ASSERT(bar.position() == 60 && view.scroll_offset().y == 60);
    ASSERT(notifications.size() == 1);

    bar.handle(MouseWheel{{}, 0, 30});
    ASSERT(bar.position() == 90 && view.scroll_offset().y == 90);
    ASSERT((notifications == std::vector<int>{60, 90}));
    app.step(now + 2s);
    ASSERT(thumb_row(bar) == 9);
    ASSERT(bar.position() == 90 && view.scroll_offset().y == 90);
    ASSERT(notifications.size() == 2);
}

TEST(scrollbar_thumb_animation_follows_moves_and_stops_on_range_or_motion_changes)
{
    using namespace std::chrono_literals;
    auto bar = ScrollBar{{.content_length = 100,
                          .viewport_length = 10,
                          .scroll_settle_time = 1s}};
    arrange(bar, {{}, {1, 10}});
    bar.handle(MousePress{{0, 9}, MouseButton::Left});
    ASSERT(bar.position() == 90 && thumb_row(bar) == 0);
    auto moved = std::move(bar);
    auto assigned = ScrollBar{};
    assigned = std::move(moved);
    auto app = Application{assigned};
    int notifications = 0;
    assigned.on_scroll.connect([&](int) { ++notifications; });
    auto now = EventLoop::Clock::now();
    app.step(now + 250ms);
    ASSERT(assigned.position() == 90 && thumb_row(assigned) > 0);
    ASSERT(thumb_row(assigned) < 9 && notifications == 0);
    assigned.set_range(100, 10, 90);
    ASSERT(thumb_row(assigned) < 9);
    assigned.set_range(50, 10, 90);
    ASSERT(assigned.position() == 40 && thumb_row(assigned) == 8);
    ASSERT(notifications == 0);

    assigned.handle(MouseWheel{{}, 0, -40});
    ASSERT(assigned.position() == 0 && notifications == 1);
    assigned.scroll_settle_time = 0ms;
    app.step(now + 500ms);
    ASSERT(thumb_row(assigned) == 0 && notifications == 1);
    app.paint();
    ASSERT(app.loop().wait_duration(now + 500ms) == std::chrono::milliseconds::max());

    auto theme = Theme::plain();
    theme.motion.enabled = false;
    app.set_theme(theme);
    assigned.scroll_settle_time = 1s;
    assigned.handle(MouseWheel{{}, 0, 40});
    ASSERT(assigned.position() == 40 && thumb_row(assigned) == 8);
    ASSERT(notifications == 2);
}

TEST(scrollbar_drag_grabs_the_displayed_thumb_and_notifications_can_destroy_the_owner)
{
    using namespace std::chrono_literals;
    auto bar = ScrollBar{{.content_length = 100,
                          .viewport_length = 10,
                          .scroll_settle_time = 1s}};
    auto app = Application{bar};
    app.dispatch(esc::Resize{{1, 10}});
    bar.handle(MouseWheel{{}, 0, 90});
    app.step(EventLoop::Clock::now() + 250ms);
    auto row = thumb_row(bar);
    ASSERT(row > 0 && row < 9);
    bar.handle(MousePress{{0, row}, MouseButton::Left});
    auto grabbed = bar.position();
    ASSERT(grabbed > 0 && grabbed < 90 && bar.is_pointer_captured());
    ASSERT(thumb_row(bar) == row);
    app.step(EventLoop::Clock::now() + 2s);
    ASSERT(bar.position() == grabbed && thumb_row(bar) == row);
    bar.handle(MouseMove{{0, 9}, MouseButton::Left});
    ASSERT(bar.position() == 90 && thumb_row(bar) == 9);
    bar.handle(MouseRelease{{0, 9}, MouseButton::Left});
    ASSERT(!bar.is_pointer_captured());

    auto owned = std::make_unique<ScrollBar>(ScrollBar::Options{
        .content_length = 100, .viewport_length = 10, .scroll_settle_time = 1s});
    auto other = Application{*owned};
    other.dispatch(esc::Resize{{1, 10}});
    int notifications = 0;
    owned->on_scroll.connect([&](int position) {
        ASSERT(position == 90);
        ++notifications;
        owned.reset();
    });
    auto response = owned->handle(MousePress{{0, 9}, MouseButton::Left});
    ASSERT(response.handled && response.repaint && !owned && notifications == 1);
    other.step(EventLoop::Clock::now() + 2s);
    ASSERT(notifications == 1);
}
