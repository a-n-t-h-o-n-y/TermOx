#ifndef TERMOX_WIDGET_LAYOUTS_SELECTING_HPP
#define TERMOX_WIDGET_LAYOUTS_SELECTING_HPP
#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include <termox/system/key.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/pipe.hpp>

namespace ox::layout {

/// Adds a 'Selected Child' concept to Layout_t.
/** There is only a single selected child at a time.
 *  Keyboard and mouse selecting ability, scrolls when selection is off screeen.
 *  Depends on the Child Widgets of Layout_t to have a select() and unselect()
 *  method. Inherit from this and override key_press_event to perform actions on
 *  the selected_child(). Scroll action also moves the selected child index. */
template <typename Layout_t, bool unselect_on_focus_out = true>
class Selecting : public Layout_t {
   private:
    using Key_codes = std::vector<Key>;

    template <typename Unary_predicate>
    using Enable_if_invocable_with_child_t = std::enable_if_t<
        std::is_invocable_v<Unary_predicate,
                            std::add_const_t<std::add_lvalue_reference_t<
                                typename Layout_t::Child_t>>>,
        int>;

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

   public:
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

   public:
    /// Return whether or not a child widget is currently selected.
    /** Useful to call before selected_child(), if you don't want to catch. */
    auto has_selected_child() const -> bool { return selected_ != nullptr; }

    /// Return the currently selected child, UB if no children in Layout.
    auto selected_child() const -> typename Layout_t::Child_t const&
    {
        if (selected_ != nullptr)
            return *selected_;
        throw std::out_of_range{"Selecting::select_child(): No Child Selected"};
    }

    /// Return the currently selected child, UB if no children in Layout.
    auto selected_child() -> typename Layout_t::Child_t&
    {
        if (selected_ != nullptr)
            return *selected_;
        throw std::out_of_range{"Selecting::select_child(): No Child Selected"};
    }

    /// Return the index into get_children() corresponding to the selected child
    /** Returns this->Widget::child_count() if no child is selected. */
    auto selected_index() const -> std::size_t
    {
        return this->find_child_position(selected_);
    }

    /// Set the first visible child as the selected child.
    void select_first_child()
    {
        if (this->child_count() > 0)
            this->set_selected_by_index(this->get_child_offset());
    }

   protected:
    auto key_press_event(Key k) -> bool override
    {
        if (contains(k, increment_selection_keys_))
            this->increment_selected_and_scroll_if_necessary();
        else if (contains(k, decrement_selection_keys_))
            this->decrement_selected_and_scroll_if_necessary();
        else if (contains(k, increment_scroll_keys_))
            this->increment_offset_and_increment_selected();
        else if (contains(k, decrement_scroll_keys_))
            this->decrement_offset_and_decrement_selected();
        return Layout_t::key_press_event(k);
    }

    /// Reset the selected child if needed.
    auto resize_event(ox::Area new_size, ox::Area old_size) -> bool override
    {
        auto const base_result = Layout_t::resize_event(new_size, old_size);
        this->reset_selected_if_necessary();
        return base_result;
    }

    auto focus_in_event() -> bool override
    {
        this->reset_selected_if_necessary();
        if (this->has_selected_child())
            this->selected_child().select();
        else if (this->child_count() > 0uL)
            this->select_first_child();
        return Layout_t::focus_in_event();
    }

    auto focus_out_event() -> bool override
    {
        if constexpr (unselect_on_focus_out) {
            if (this->has_selected_child())
                this->selected_child().unselect();
        }
        return Layout_t::focus_out_event();
    }

    auto enable_event() -> bool override
    {
        if (this->has_selected_child() && System::focus_widget() == this)
            this->selected_child().select();
        return Layout_t::enable_event();
    }

    auto disable_event() -> bool override
    {
        if (this->has_selected_child())
            this->selected_child().unselect();
        return Layout_t::disable_event();
    }

    auto child_removed_event(Widget& child) -> bool override
    {
        if (&child == selected_) {
            if (this->child_count() > 0uL)
                this->set_selected_by_index(this->get_child_offset());
            else
                selected_ = nullptr;
        }
        return Layout_t::child_removed_event(child);
    }

   private:
    typename Layout_t::Child_t* selected_{nullptr};
    Key_codes increment_selection_keys_;
    Key_codes decrement_selection_keys_;
    Key_codes increment_scroll_keys_;
    Key_codes decrement_scroll_keys_;

   private:
    void increment_selected()
    {
        auto const next = this->selected_index() + 1;
        if (next >= this->child_count())
            return;
        this->set_selected_by_index(next);
    }

    void increment_selected_and_scroll_if_necessary()
    {
        if (!this->has_selected_child())
            return;
        this->increment_selected();
        while (!this->selected_child().is_enabled()) {
            this->increment_offset();
            this->update_geometry();
        }
    }

    void decrement_selected()
    {
        auto const current = this->selected_index();
        if (current == 0)
            return;
        this->set_selected_by_index(current - 1);
    }

    void decrement_selected_and_scroll_if_necessary()
    {
        if (!this->has_selected_child())
            return;
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
        if (auto const offset = this->get_child_offset(); offset + 1 != child_n)
            this->set_child_offset(offset + 1);
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
        if (auto const offset = this->get_child_offset(); offset != 0)
            this->set_child_offset(offset - 1);
    }

    void decrement_offset_and_decrement_selected()
    {
        if (this->get_child_offset() == 0)
            return;
        this->decrement_offset();
        this->decrement_selected();
    }

    /// unselect() the currently selected child, select() the child at \p index.
    /** Throws std::out_of_range if \p index is invalid. */
    void set_selected_by_index(std::size_t index)
    {
        if (index < this->child_count())
            this->set_selected_by_pointer(&(this->get_children()[index]));
        else
            throw std::out_of_range{"Selecting::set_selected_by_index"};
    }

    /// unselect() the currently selected child, if any, and select() \p child.
    void set_selected_by_pointer(typename Layout_t::Child_t* child)
    {
        if (this->has_selected_child())
            selected_->unselect();
        selected_ = child;
        selected_->select();
    }

    /// Find the child index of the last displayed Data_row.
    /** Assumes child_count() > 0. Returns child_offset if all are disabled. */
    auto find_bottom_row_index() const -> std::size_t
    {
        auto const children = this->Widget::get_children();
        auto const count    = this->child_count();
        auto const offset   = this->get_child_offset();
        for (auto i = offset + 1; i < count; ++i) {
            if (children[i].is_enabled())
                continue;
            else
                return i - 1;
        }
        return offset;
    }

    /// If selected_child is off the screen, select() the last displayed widget.
    void reset_selected_if_necessary()
    {
        if (this->has_selected_child()) {
            if (this->selected_child().is_enabled())
                return;
            else
                this->set_selected_by_index(this->find_bottom_row_index());
        }
    }

    /// Return true if \p codes contains the value \p key.
    static auto contains(Key k, Key_codes const& codes) -> bool
    {
        return std::any_of(std::begin(codes), std::end(codes),
                           [=](auto code) { return code == k; });
    }
};

/// Helper function to create an instance.
template <typename Layout_t,
          bool unselect_on_focus_out = true,
          typename... Args>
auto selecting(Args&&... args)
    -> std::unique_ptr<Selecting<Layout_t, unselect_on_focus_out>>
{
    return std::make_unique<Selecting<Layout_t, unselect_on_focus_out>>(
        std::forward<Args>(args)...);
}

}  // namespace ox::layout
#endif  // TERMOX_WIDGET_LAYOUTS_SELECTING_HPP
