#pragma once

#include <chrono>
#include <optional>

#include <ox/timer.hpp>
#include <ox/widget.hpp>

namespace ox {

/** Scroll positions and lengths use the same units as the controlled content. */
class ScrollBar : public Widget {
   public:
    using Widget::handle;
    struct Options {
        int content_length = 0;
        int viewport_length = 0;
        int position = 0;
        bool vertical = true;
        Style style = {};
        std::chrono::milliseconds scroll_settle_time = std::chrono::milliseconds{0};
    } static const init;

    auto content_length() const -> int { return content_length_; }
    auto viewport_length() const -> int { return viewport_length_; }
    auto position() const -> int { return position_; }
    bool vertical;
    Style style;
    std::chrono::milliseconds scroll_settle_time;
    sl::Signal<void(int)> on_scroll;

    ScrollBar(Options x = init);
    ScrollBar(ScrollBar&& other);
    auto operator=(ScrollBar&& other) -> ScrollBar&;
    /** Synchronize content without emitting on_scroll; cancel pending animation. */
    void set_range(int content, int viewport, int position);
    /** Explicit position changes notify only on change. */
    auto set_position(int position) -> bool;
    auto measure(Area available, esc::WidthPolicy = {}) const -> Area override
    {
        validate(available);
        return {1, 1};
    }
    auto handle(MouseMove const&) -> EventResponse override;
    auto handle(MouseRelease const&) -> EventResponse override;
    auto pointer_cancel() -> EventResponse override;
    auto paint(Canvas) -> Cursor override;
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseWheel const&) -> EventResponse override;
    auto tick() -> EventResponse;

   private:
    int content_length_;
    int viewport_length_;
    int position_;
    std::optional<int> drag_offset_;
    std::optional<Timer> timer_;
    std::chrono::steady_clock::time_point started_;
    int start_ = 0;
    int target_ = 0;
    auto move_to(int position) -> EventResponse;
};
}  // namespace ox

namespace ox {
/** A scrollable widget uses terminal columns/rows and reports geometry changes. */
template <typename T>
concept Scrollable = WidgetDerived<T> && requires(T& view, T const& c, Point at) {
    { c.content_size() } -> std::same_as<Area>;
    { c.viewport_size() } -> std::same_as<Area>;
    { c.scroll_offset() } -> std::same_as<Point>;
    { view.set_scroll_offset(at) } -> std::same_as<bool>;
    { view.on_scroll } -> std::same_as<sl::Signal<void(Point, Area, Area)>&>;
};
/** Synchronize one axis, tracking both lifetimes. Disconnect using returned IDs. */
template <Scrollable View>
auto link(View& view, ScrollBar& bar)
    -> std::pair<sl::Identifier, sl::Identifier>
{
    auto synchronize = [](View& view, ScrollBar& bar) {
        auto content = view.content_size();
        auto viewport = view.viewport_size();
        auto at = view.scroll_offset();
        bar.set_range(bar.vertical ? content.height : content.width,
                      bar.vertical ? viewport.height : viewport.width,
                      bar.vertical ? at.y : at.x);
    };
    synchronize(view, bar);
    auto first = Connection{.signal = view.on_scroll,
        .slot = [synchronize](Point, Area, Area, View& v, ScrollBar& b) {
            synchronize(v, b);
        }}(view, bar);
    try {
        return {first, Connection{.signal = bar.on_scroll,
            .slot = [](int position, View& v, ScrollBar& b) {
                auto at = v.scroll_offset();
                if (b.vertical) at.y = position;
                else at.x = position;
                v.set_scroll_offset(at);
            }}(view, bar)};
    }
    catch (...) {
        view.on_scroll.disconnect(first);
        throw;
    }
}
}  // namespace ox
