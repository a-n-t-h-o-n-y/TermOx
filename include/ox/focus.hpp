/** @file
 * Application-local focus tracking and traversal.
 */
#pragma once

#include <ox/routing.hpp>

namespace ox {

/** Application-local focus. The root must outlive this object. */
class Focus {
   public:
    /** Borrow the root and start with no selected widget; the root must outlive the manager. */
    explicit Focus(Widget& root) : root_{track(root)} {}
    /** Clear focus and notify a surviving focus owner during cleanup. */
    ~Focus();
    /** A focus manager cannot be copied across independent trees. */
    Focus(Focus const&) = delete;
    /** Focus managers cannot be copy-assigned. */
    auto operator=(Focus const&) -> Focus& = delete;

    /** Reject targets outside the visible, enabled tree or with FocusPolicy::None.
     * Transitions notify widgets and return effects without applying them.
     */
    auto set(Widget& widget) -> EventResponse;
    /** Clear focus and disable automatic recovery; return notification effects without
     * applying them.
     */
    auto clear() -> EventResponse;
    /** Borrow the current focused identity; the view may be empty or expired. */
    auto get() const -> LifetimeView<Widget> { return in_focus_; }
    /** Recover invalidated focus using the first tab candidate. Explicit clear
     * persists. */
    auto reconcile(std::span<WidgetNode const> tree) -> EventResponse;
    /** Move through eligible Tab/Strong widgets, wrapping at either end; return
     * transition effects.
     */
    auto advance(std::span<WidgetNode const> tree, bool backward = false)
        -> EventResponse;

   private:
    LifetimeView<Widget> root_;
    LifetimeView<Widget> in_focus_;
    bool recover_ = true;
    std::size_t revision_ = 0;
    std::vector<std::pair<LifetimeView<Widget>, LifetimeView<Widget>>> scopes_;
};
}  // namespace ox
