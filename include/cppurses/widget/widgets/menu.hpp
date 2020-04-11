#ifndef CPPURSES_WIDGET_WIDGETS_MENU_HPP
#define CPPURSES_WIDGET_WIDGETS_MENU_HPP
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

#include <signals/slot.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/fixed_height.hpp>

namespace cppurses {
class Button;
class Label;

/// Displays a list of items, sending an associated Signal on user input.
/** Signal sent to selected on Key::Enter press or */
class Menu : public layout::Vertical<> {
   public:
    /// Menu Title Widget
    Label& title;

    /// Line Break Widget under Title
    Fixed_height& line_break = this->make_child<Fixed_height>(1);

   public:
    /// Construct an empty Menu with \p title_text.
    explicit Menu(Glyph_string title_text);

    /// Append item to the end of list, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto append_item(Glyph_string label) -> sig::Signal<void()>&
    {
        return this->insert_item(std::move(label), this->size());
    }

    /// Insert item at \p index into the Menu, displayed with \p label.
    /** Returns a Signal ref which will be called when this item is selected. */
    auto insert_item(Glyph_string label, std::size_t index)
        -> sig::Signal<void()>&;

    /// Remove item a \p index in the Menu, no-op if \p index is invalid.
    void remove_item(std::size_t index);

    /// Set \p index as the currently selected item, does not send Signal.
    void select_item(std::size_t index);

    /// Decrement the currently selected index by \p n.
    /** Clamps to index 0 if n is too large. */
    void select_up(std::size_t n = 1)
    {
        auto const new_index = selected_index_ > n ? selected_index_ - n : 0;
        this->select_item(new_index);
    }

    /// Increment the currently selected index by \p n, no-op if
    /** Clamps to index this->size() - 1 if n is too large. */
    void select_down(std::size_t n = 1)
    {
        this->select_item(selected_index_ + n);
    }

    /// Return the number of items in the Menu.
    auto size() const -> std::size_t { return items_.size(); }

    /// Set the Attribute applied to the selected item.
    void set_selected_attribute(Attribute const& attr);

    /// Hide the Menu's title.
    void hide_title()
    {
        title_enabled_ = false;
        this->enable(this->is_enabled());
    }

    /// Show the Menu's title.
    void show_title()
    {
        title_enabled_ = true;
        this->enable(this->is_enabled());
    }

    /// Hide the line break just under the title.
    void hide_line_break()
    {
        line_break_enabled_ = false;
        this->enable(this->is_enabled());
    }

    /// Show the line break just under the title.
    void show_line_break()
    {
        line_break_enabled_ = true;
        this->enable(this->is_enabled());
    }

    /// Only enables title and line_break if they are enabled.
    void enable(bool enable                    = true,
                bool post_child_polished_event = true) override;

   protected:
    /// Arrow keys up/down will select up/down, Enter key will send Signal.
    auto key_press_event(Key::State const& keyboard) -> bool override;

    /// Selects up/down on scroll wheel.
    auto mouse_wheel_event(Mouse::State const& mouse) -> bool override;

    /// Selects up/down on scroll wheel.
    auto mouse_wheel_event_filter(Widget& /* receiver */,
                                  Mouse::State const& mouse) -> bool override;

   private:
    /// Holds reference to Button used to display item, and its Signal.
    struct Menu_item {
        explicit Menu_item(Button& ref) : button{ref} {};
        std::reference_wrapper<Button> button;
        sig::Signal<void()> selected;
    };

    std::vector<Menu_item> items_;
    std::size_t selected_index_ = 0;
    Attribute selected_attr_    = Attribute::Inverse;
    bool title_enabled_         = true;
    bool line_break_enabled_    = true;

   private:
    /// Sends the Signal associated with the currently selected item.
    /** No-op if this-size() == 0. */
    void send_selected_signal()
    {
        if (not items_.empty())
            items_[selected_index_].selected();
    }
};

namespace slot {

auto select_up(Menu& m) -> sig::Slot<void(std::size_t)>;
auto select_up(Menu& m, std::size_t n) -> sig::Slot<void()>;

auto select_down(Menu& m) -> sig::Slot<void(std::size_t)>;
auto select_down(Menu& m, std::size_t n) -> sig::Slot<void()>;

auto select_item(Menu& m) -> sig::Slot<void(std::size_t)>;
auto select_item(Menu& m, std::size_t index) -> sig::Slot<void()>;

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_MENU_HPP
