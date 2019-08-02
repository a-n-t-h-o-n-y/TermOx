#ifndef CPPURSES_SYSTEM_FOCUS_HPP
#define CPPURSES_SYSTEM_FOCUS_HPP

namespace cppurses {
class Widget;

// TODO This is an implementation detail, should be in detail, and private to
// who uses it. Users will interact with Focus through a widget's Focus_policy.

class Focus {
   public:
    static Widget* focus_widget() { return focus_widget_; }

    /// Sets the focus to \p clicked if it has a Focus_policy::Mouse/Strong.
    static void mouse_press(Widget& clicked);

    /// Find the next Focus_policy::Tab/Strong Widget and set focus to it.
    static bool tab_press();

    /// Find the previous Focus_policy::Tab/Strong Widget and set focus to it.
    static bool shift_tab_press();

    /// Set focus to \p new_focus.
    /** If \p new_focus has Focus_policy::None, calls Focus::clear(). */
    static void set_focus_to(Widget& new_focus);

    /// Set the focus widget to nullptr.
    static void clear();

    /// Enable Tab/Back_tab keys to change the focus Widget.
    static void enable_tab_focus() { tab_enabled_ = true; }

    /// Disable Tab/Back_tab keys from changing focus Widget.
    static void disable_tab_focus() { tab_enabled_ = false; }

   private:
    static Widget* focus_widget_;
    static bool tab_enabled_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_FOCUS_HPP
