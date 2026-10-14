#pragma once

#include <ox/routing.hpp>

namespace ox {

/** Application-local focus. The root must outlive this object. */
class Focus {
   public:
    explicit Focus(Widget& root) : root_{root} {}
    ~Focus();
    Focus(Focus const&) = delete;
    auto operator=(Focus const&) -> Focus& = delete;

    /** Reject targets outside the visible, enabled tree or with FocusPolicy::None.
     * Transitions notify widgets and return effects without applying them.
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
    LifetimeView<Widget> in_focus_;
    bool recover_ = true;
    std::size_t revision_ = 0;
};
}  // namespace ox
