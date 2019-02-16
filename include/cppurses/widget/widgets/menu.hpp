#ifndef CPPURSES_WIDGET_WIDGETS_MENU_HPP
#define CPPURSES_WIDGET_WIDGETS_MENU_HPP
#include <cstddef>
#include <functional>
#include <vector>

#include <signals/slot.hpp>

#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/fixed_height.hpp>

namespace cppurses {
class Glyph_string;
class Push_button;
struct Keyboard_data;
struct Label;

class Menu : public layout::Vertical {
   public:
    explicit Menu(Glyph_string title);

    sig::Signal<void()>& add_item(Glyph_string label);
    sig::Signal<void()>& insert_item(Glyph_string label, std::size_t index);
    void remove_item(std::size_t index);

    void select_up(std::size_t n = 1);
    void select_down(std::size_t n = 1);
    void select_item(std::size_t index);

    std::size_t size() const;

   protected:
    bool key_press_event(const Keyboard_data& keyboard) override;

    bool mouse_press_event(const Mouse::State& mouse) override;

    bool mouse_press_event_filter(Widget& receiver,
                                  const Mouse::State& mouse) override;

   private:
    struct Menu_item {
        explicit Menu_item(Push_button& ref);
        std::reference_wrapper<Push_button> button;
        sig::Signal<void()> selected;
    };

    std::vector<Menu_item> items_;
    std::size_t selected_index_{0};

    Label& title_;
    Fixed_height& space1{this->make_child<Fixed_height>(1)};

    void call_current_item();
};

namespace slot {

sig::Slot<void(std::size_t)> select_up(Menu& m);
sig::Slot<void()> select_up(Menu& m, std::size_t n);

sig::Slot<void(std::size_t)> select_down(Menu& m);
sig::Slot<void()> select_down(Menu& m, std::size_t n);

sig::Slot<void(std::size_t)> select_item(Menu& m);
sig::Slot<void()> select_item(Menu& m, std::size_t index);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_MENU_HPP
