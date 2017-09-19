#ifndef SYSTEM_FOCUS_HPP
#define SYSTEM_FOCUS_HPP

namespace cppurses {
class Widget*;

class Focus {
   public:
    Widget* focus_widget() const;

    void mouse_press(Widget* clicked) const;
    bool tab_press() const;
    void set_focus_to(Widget* new_focus);

   private:
    Widget* focus_widget_;
    Widget* next_tab_focus();  // make a free function, anon namespace
};

}  // namespace cppurses
#endif  // SYSTEM_FOCUS_HPP
