#pragma once

#include <ox/widget.hpp>

namespace ox {

/**
 * Manages the focus of Widgets.
 *
 * @details When a Widget is in focus, it will receive keyboard input. Only one Widget
 * can be in focus at a time.
 */
class Focus {
   public:
    Focus() = default;

   public:
    /**
     * Changes the current focus to \p w. Invokes focus_out on the Widget currently in
     * focus.
     */
    void set(Widget& w);

    /**
     * Returns a LifetimeView of the focused Widget, which may not be valid if nothing
     * is in focus.
     */
    auto get() const -> LifetimeView<Widget> { return in_focus_; }

    /**
     * Clears the focus, removing any in focus Widget.
     */
    void clear();

   private:
    LifetimeView<Widget> in_focus_ = std::shared_ptr<Widget*>{nullptr};
};

}  // namespace ox