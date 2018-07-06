#ifndef WIDGET_WIDGETS_MENU_HPP
#define WIDGET_WIDGETS_MENU_HPP
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

#include <signals/slot.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/widget/layouts/vertical_layout.hpp>
#include <cppurses/widget/widgets/blank_height.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

class Menu : public Vertical_layout {
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
    bool paint_event(Painter& p) override;

    bool key_press_event(Key key, char symbol) override;

    bool mouse_press_event(Mouse_button button,
                           Point global,
                           Point local,
                           std::uint8_t device_id) override;

    bool mouse_press_event_filter(Event_handler* receiver,
                                  Mouse_button button,
                                  Point global,
                                  Point local,
                                  std::uint8_t device_id) override;

   private:
    struct Menu_item {
        explicit Menu_item(Push_button& ref);
        std::reference_wrapper<Push_button> button;
        sig::Signal<void()> selected;
    };

    std::vector<Menu_item> items_;
    std::size_t selected_index_{0};

    Label& title_;
    Blank_height& space1{this->make_child<Blank_height>(1)};

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
#endif  // WIDGET_WIDGETS_MENU_HPP
