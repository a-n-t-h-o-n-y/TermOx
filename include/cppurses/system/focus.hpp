#ifndef SYSTEM_FOCUS_HPP
#define SYSTEM_FOCUS_HPP

namespace cppurses {
class Widget;

class Focus {
   public:
    static Widget* focus_widget();

    static void mouse_press(Widget* clicked);
    static bool tab_press();
    static void set_focus_to(Widget* new_focus);
    static void clear_focus();

   private:
    static Widget* focus_widget_;
};

}  // namespace cppurses
#endif  // SYSTEM_FOCUS_HPP
