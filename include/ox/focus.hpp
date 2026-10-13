#pragma once

#include <ox/routing.hpp>

namespace ox {

/** Application-local focus. The root and event loop must outlive this object. */
class Focus {
   public:
    Focus(Widget& root, EventLoop& loop) : root_{root}, loop_{loop} {}

    /** Reject targets outside the active, enabled tree or with FocusPolicy::None.
     * Transitions notify widgets and schedule their effects on the owned loop.
     */
    auto set(Widget& widget) -> EventResponse;
    auto clear() -> EventResponse;
    auto get() const -> LifetimeView<Widget> { return in_focus_; }
    /** Recover invalidated focus using the first tab candidate. Explicit clear
     * persists. */
    auto reconcile(std::span<WidgetNode const> tree) -> EventResponse;
    auto advance(std::span<WidgetNode const> tree, bool backward = false)
        -> EventResponse;

   private:
    Widget& root_;
    EventLoop& loop_;
    LifetimeView<Widget> in_focus_;
    bool recover_ = true;
    std::size_t revision_ = 0;

    void report(EventResponse response);
};
}  // namespace ox
