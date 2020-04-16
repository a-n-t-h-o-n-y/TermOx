#ifndef CPPURSES_WIDGET_LAYOUTS_SELECTING_HPP
#define CPPURSES_WIDGET_LAYOUTS_SELECTING_HPP
#include <algorithm>
#include <vector>

#include <iostream>//temp

#include <cppurses/system/events/key.hpp>
#include <cppurses/widget/pipe.hpp>

namespace cppurses::layout {

/// Adds 'Selected Child' concept to a layout.
/** Keyboard and mouse selecting ability, scrolls when selection is off screeen.
 *  Depends on the Child Widgets of Layout_t to have a select() and unselect()
 *  method. Inherit from this and override key_press_event to perform actions on
 *  the selected_child(). Scroll action also moves the selected child index. */
template <typename Layout_t>
class Selecting : public Layout_t {
   private:
    using Key_codes = std::vector<Key::Code>;

   public:
    Selecting() { *this | pipe::strong_focus(); }

    Selecting(Key_codes increment_selection_keys,
              Key_codes decrement_selection_keys,
              Key_codes increment_scroll_keys,
              Key_codes decrement_scroll_keys)
        : increment_selection_keys_{increment_selection_keys},
          decrement_selection_keys_{decrement_selection_keys},
          increment_scroll_keys_{increment_scroll_keys},
          decrement_scroll_keys_{decrement_scroll_keys}
    {
        *this | pipe::strong_focus();
    }

    void set_increment_selection_keys(Key_codes keys)
    {
        increment_selection_keys_ = std::move(keys);
    }

    void set_decrement_selection_keys(Key_codes keys)
    {
        decrement_selection_keys_ = std::move(keys);
    }

    void set_increment_scroll_keys(Key_codes keys)
    {
        increment_scroll_keys_ = std::move(keys);
    }

    void set_decrement_scroll_keys(Key_codes keys)
    {
        decrement_scroll_keys_ = std::move(keys);
    }

    // Return the currently selected child, UB if no children in Layout.
    auto selected_child() const -> typename Layout_t::Child const&
    {
        return this->get_children()[selected_];
    }

    // Return the currently selected child, UB if no children in Layout.
    auto selected_child() -> typename Layout_t::Child&
    {
        return this->get_children()[selected_];
    }

   protected:
    auto key_press_event(Key::State const& keyboard) -> bool override
    {
        if (contains(keyboard.key, increment_selection_keys_))
            this->increment_selected_and_scroll_if_necessary();
        else if (contains(keyboard.key, decrement_selection_keys_))
            this->decrement_selected_and_scroll_if_necessary();
        else if (contains(keyboard.key, increment_scroll_keys_))
            this->increment_offset_and_increment_selected();
        else if (contains(keyboard.key, decrement_scroll_keys_))
            this->decrement_offset_and_decrement_selected();
        return Layout_t::key_press_event(keyboard);
    }

    /// Reset the selected child if needed.
    auto resize_event(cppurses::Area new_size, cppurses::Area old_size)
        -> bool override
    {
        auto const base_result = Layout_t::resize_event(new_size, old_size);
        this->reset_selected_if_necessary();
        return base_result;
    }

    /// If selected_child is off the screen, select() the last displayed widget.
    void reset_selected_if_necessary()
    {
        if (this->Layout_t::child_count() == 0 ||
            this->selected_child().is_enabled()) {
            return;
        }
        this->set_selected(this->find_bottom_row());
    }

    auto focus_in_event() -> bool override
    {
        this->reset_selected_if_necessary();
        if (this->child_count() != 0)
            this->selected_child().select();
        return Layout_t::focus_in_event();
    }

    auto focus_out_event() -> bool override
    {
        if (this->child_count() != 0)
            this->selected_child().unselect();
        return Layout_t::focus_out_event();
    }

   private:
    std::size_t selected_ = 0uL;
    Key_codes increment_selection_keys_;
    Key_codes decrement_selection_keys_;
    Key_codes increment_scroll_keys_;
    Key_codes decrement_scroll_keys_;

   private:
    void increment_selected()
    {
        if (this->child_count() == 0)
            return;
        if (selected_ + 1 == this->child_count())
            return;
        this->set_selected(selected_ + 1);
    }

    void increment_selected_and_scroll_if_necessary()
    {
        this->increment_selected();
        if (!this->selected_child().is_enabled())
            this->increment_offset();
    }

    void decrement_selected()
    {
        if (this->child_count() == 0)
            return;
        if (selected_ == 0)
            return;
        this->set_selected(selected_ - 1);
    }

    void decrement_selected_and_scroll_if_necessary()
    {
        this->decrement_selected();
        if (!this->selected_child().is_enabled())
            this->decrement_offset();
    }

    /// Scroll down or right.
    void increment_offset()
    {
        auto const child_n = this->child_count();
        if (child_n == 0)
            return;
        if (auto const offset = this->child_offset(); offset + 1 != child_n)
            this->set_offset(offset + 1);
    }

    void increment_offset_and_increment_selected()
    {
        this->increment_offset();
        this->increment_selected();
    }

    /// Scroll up or left.
    void decrement_offset()
    {
        if (this->child_count() == 0)
            return;
        if (auto const offset = this->child_offset(); offset != 0)
            this->set_offset(offset - 1);
    }

    void decrement_offset_and_decrement_selected()
    {
        if (this->child_offset() == 0)
            return;
        this->decrement_offset();
        this->decrement_selected();
    }

    /// unselect() the currently selected child, select() the child at \p index.
    void set_selected(std::size_t index)
    {
        auto const children    = this->get_children();
        auto const is_in_focus = System::focus_widget() == this;
        if (is_in_focus) {
            children[selected_].unselect();
            selected_ = index;
            children[selected_].select();
        }
    }

    /// Find the child index of the last displayed Data_row.
    /** Assumes child_count() > 0. Returns child_offset if all are disabled. */
    auto find_bottom_row() const -> std::size_t
    {
        auto const children = this->Widget::get_children();
        auto const count    = this->child_count();
        auto const offset   = this->child_offset();
        for (auto i = offset + 1; i < count; ++i) {
            if (children[i].is_enabled())
                continue;
            return i - 1;
        }
        return offset;
    }

    /// Return true if \p codes contains the value \p key.
    static auto contains(Key::Code key, Key_codes const& codes) -> bool
    {
        return std::any_of(std::begin(codes), std::end(codes),
                           [=](auto k) { return k == key; });
    }
};

}  // namespace cppurses::layout
#endif  // CPPURSES_WIDGET_LAYOUTS_SELECTING_HPP
