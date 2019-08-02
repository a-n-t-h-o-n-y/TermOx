#ifndef CPPURSES_WIDGET_WIDGETS_MENU_HPP
#define CPPURSES_WIDGET_WIDGETS_MENU_HPP
#include <cstddef>
#include <functional>
#include <vector>

#include <signals/slot.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/fixed_height.hpp>

namespace cppurses {
class Glyph_string;
class Push_button;
struct Label;

/// Displays a list of items, sending an associated Signal on user input.
/** Signal sent to selected on Key::Enter press or */
class Menu : public layout::Vertical {
   public:
    /// Construct an empty Menu with \p title_text.
    explicit Menu(Glyph_string title_text);

    /// Append item to the end of list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    sig::Signal<void()>& append_item(Glyph_string label);

    /// Insert item at \p index into the Menu, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    sig::Signal<void()>& insert_item(Glyph_string label, std::size_t index);

    /// Remove item a \p index in the Menu, no-op if \p index is invalid.
    void remove_item(std::size_t index);

    /// Set \p index as the currently selected item, does not send Signal.
    void select_item(std::size_t index);

    /// Decrement the currently selected index by \p n.
    /** Clamps to index 0 if n is too large. */
    void select_up(std::size_t n = 1);

    /// Increment the currently selected index by \p n, no-op if
    /** Clamps to index this->size() - 1 if n is too large. */
    void select_down(std::size_t n = 1);

    /// Return the number of items in the Menu.
    std::size_t size() const { return items_.size(); }

    /// Set the Attribute applied to the selected item.
    void set_selected_attribute(const Attribute& attr);

    /// Hide the Menu's title.
    void hide_title();

    /// Show the Menu's title.
    void show_title();

    /// Hide the line break just under the title.
    void hide_line_break();

    /// Show the line break just under the title.
    void show_line_break();

    /// Only enables title and line_break if they are enabled.
    void enable(bool enable                    = true,
                bool post_child_polished_event = true) override;

    /// Menu Title Widget
    Label& title;

    /// Line Break Widget under Title
    Fixed_height& line_break{this->make_child<Fixed_height>(1)};

   protected:
    /// Arrow keys up/down will select up/down, Enter key will send Signal.
    bool key_press_event(const Key::State& keyboard) override;

    /// Selects up/down on scroll wheel.
    bool mouse_press_event(const Mouse::State& mouse) override;

    /// Selects up/down on scroll wheel.
    bool mouse_press_event_filter(Widget& /* receiver */,
                                  const Mouse::State& mouse) override;

   private:
    /// Holds reference to Push_button used to display item, and its Signal.
    struct Menu_item {
        explicit Menu_item(Push_button& ref) : button{ref} {};
        std::reference_wrapper<Push_button> button;
        sig::Signal<void()> selected;
    };
    std::vector<Menu_item> items_;
    std::size_t selected_index_{0};
    Attribute selected_attr_{Attribute::Inverse};

    bool title_enabled_{true};
    bool line_break_enabled_{true};

    /// Sends the Signal associated with the currently selected item.
    /** No-op if this-size() == 0. */
    void send_selected_signal();
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
