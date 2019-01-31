#ifndef CPPURSES_SYSTEM_FOCUS_HPP
#define CPPURSES_SYSTEM_FOCUS_HPP

namespace cppurses {
class Widget;

// TODO This is an implementation detail, should be in detail, and private to
// who uses it. Users will interact with Focus through a widget's Focus_policy.
// Except for set_focus which is used throughout...

class Focus {
   public:
    static Widget* focus_widget() { return focus_widget_; }

    static void mouse_press(Widget* clicked);

    static bool tab_press();

    static bool shift_tab_press();

    static void set_focus_to(Widget* new_focus);

    static void clear_focus();

   private:
    static Widget* focus_widget_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_FOCUS_HPP
