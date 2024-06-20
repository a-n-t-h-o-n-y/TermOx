#pragma once

namespace ox::widgets {
class Widget;

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
     * Returns the Widget that currently has focus, or nullptr if no Widget has focus.
     */
    static auto get() -> Widget* { return in_focus_; }

    /**
     * Clears the focus, setting the focused Widget to nullptr.
     */
    static void clear();

   private:
    // Widget special member functions will update this if it is currently in focus to
    // avoid a dangling pointer.
    friend Widget;
    inline static Widget* in_focus_ = nullptr;
};

}  // namespace ox::widgets