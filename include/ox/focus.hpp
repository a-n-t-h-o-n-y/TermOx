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
    /**
     * Changes the current focus to \p w. Invokes focus_out on the Widget currently in
     * focus. If \p w is not nullptr, it triggers focus_in(w).
     *
     * @param w The Widget to set focus to.
     */
    static void set(Widget& w);

    /**
     * Returns a LifetimeView of the focused Object, which may not be valid if nothing
     * is in focus.
     */
    static auto get() -> LifetimeView<Widget> { return in_focus_; }

    /**
     * Clears the focus, setting the focused Widget to nullptr.
     */
    static void clear();

   private:
    inline static LifetimeView<Widget> in_focus_ = std::shared_ptr<Widget*>{nullptr};
};

}  // namespace ox