#ifndef TERMOX_WIDGET_LAYOUTS_STACK_HPP
#define TERMOX_WIDGET_LAYOUTS_STACK_HPP
#include <algorithm>
#include <cstddef>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>
#include <termox/widget/layout.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox::layout {

/// A Layout enabling only a single Widget at a time.
/** A Stack is made up of pages, which are child Widgets that can be displayed
 *  one at a time within the Stack. The active page determines which child
 *  Widget is currently displayed. */
template <typename Child_t = Widget>
class Stack : public Layout<Child_t> {
   public:
    /// Emitted when the active page is changed, sends the new index along.
    sl::Signal<void(std::size_t)> page_changed;

   public:
    using Layout<Child_t>::Layout;

    /// Set child Widget to be enabled/visible via its index into child vector.
    /** The index is typically the same order as child Widgets were added.
     *  Throws std::out_of_range if \p index is invalid. */
    void set_active_page(std::size_t index)
    {
        if (index > this->Stack::size())
            throw std::out_of_range{"Stack::set_active_page: index is invalid"};
        active_page_ = &(this->get_children()[index]);
        this->enable(this->is_enabled(), false);  // sends enable/disable events
        this->move_active_page();
        this->resize_active_page();
        if (sets_focus_)
            System::set_focus(*active_page_);
        this->page_changed(index);
    }

    /// Set whether Focus is given to a child Widget when it becomes active.
    /** Enabled by default. */
    void give_focus_on_change(bool sets_focus = true)
    {
        sets_focus_ = sets_focus;
    }

    /// Construct and append a page to the Stack.
    /** This will construct a child Widget of type T, using \p args passed to
     *  T's constructor, and then automatically disable it. Returns a reference
     *  to the created child Widget. */
    template <typename Widget_t = Child_t, typename... Args>
    auto make_page(Args&&... args) -> Widget_t&
    {
        static_assert(std::is_base_of<Child_t, Widget_t>::value,
                      "Stack::make_page: Widget_t must be a Child_t type");
        return static_cast<Widget_t&>(this->append_page(
            std::make_unique<Widget_t>(std::forward<Args>(args)...)));
    }

    /// Add an existing Widget as a page to the end of the Stack.
    /** Returns a reference to the appended Widget as Child_t&. */
    template <typename Widget_t>
    auto append_page(std::unique_ptr<Widget_t> w_ptr) -> Widget_t&
    {
        static_assert(std::is_base_of<Child_t, Widget_t>::value,
                      "Stack::append_page: Widget_t must be a Child_t type");
        auto& result = this->append_child(std::move(w_ptr));
        result.disable();
        return result;
    }

    /// Add an existing Widget as a page to the end of the Stack.
    /** Returns a reference to the appended Widget as Child_t&. */
    template <typename Widget_t>
    auto append_page(Widget_t&& w) -> Child_t&
    {
        return this->append_page(
            std::make_unique<Widget_t>(std::forward<Widget_t>(w)));
    }

    /// Insert an existing Widget \p child at \p index.
    /** Throws std::invalid_argument if \p child is nullptr.
     *  Throws std::out_of_range if \p index > number of children.
     *  Returns a reference to the inserted Child_t object. */
    auto insert_page(std::unique_ptr<Child_t> child, std::size_t index)
        -> Child_t&
    {
        auto& result = this->insert_child(std::move(child), index);
        result.disable();
        return result;
    }

    /// Remove a page from the list, by \p index value, and delete it.
    /** Throws std::out_of_range if \p index is invalid. Sets active page to
     *  nullptr if the active page is being deleted. */
    void delete_page(std::size_t index)
    {
        if (index >= this->Stack::size())
            throw std::out_of_range{"Stack::delete_page: index is invalid"};
        auto* page_to_delete = std::addressof(this->get_children()[index]);
        if (page_to_delete == this->get_active_page())
            active_page_ = nullptr;
        this->remove_and_delete_child(page_to_delete);
    }

    /// Remove page at \p index from the list and return it.
    /** Useful if you need to move a page into another Widget. Use
     *  Stack::delete_page() if you want to remove a page and destroy it.
     *  Letting the returned Widget destroy itself will potentially leave
     *  dangling pointers in the event system. Throws std::out_of_range if \p
     *  index is invalid. Sets active page to nullptr if active page removed. */
    auto remove_page(std::size_t index) -> std::unique_ptr<Widget>
    {
        if (index >= this->size())
            throw std::out_of_range{"Stack::remove_page: index is invalid."};
        auto* page_to_remove = &(this->get_children()[index]);
        if (page_to_remove == this->get_active_page())
            active_page_ = nullptr;
        return this->remove_child(page_to_remove);
    }

    /// Remove and delete all pages.
    void clear()
    {
        // Can't use a range-for loop, delete_page modifies the child_list_
        while (!this->get_children().empty())
            this->delete_page(0);
    }

    /// Return number of pages in this Stack.
    // TODO change to page_count()
    auto size() const -> std::size_t { return this->child_count(); }

    /// Return a pointer to the current active page, or nullptr if none.
    auto get_active_page() const -> Child_t* { return active_page_; }

    /// Return the index of the current active page.
    /** Returns Stack::invalid_index if active_page_ is nullptr. */
    auto active_page_index() const -> std::size_t
    {
        if (active_page_ == nullptr)
            return Stack::invalid_index;
        auto begin     = std::cbegin(this->get_children());
        auto const end = std::cend(this->get_children());
        auto distance  = 0uL;
        for (; begin != end; ++begin) {
            if (&(*begin) != active_page_)
                ++distance;
            else
                break;
        }
        return distance;
    }

    /// Post an Enable_event or Disable_event to the active page.
    void enable(bool enable                    = true,
                bool post_child_polished_event = true) override
    {
        this->Widget::enable_and_post_events(enable, post_child_polished_event);
        for (auto& child : this->get_children()) {
            if (&child == active_page_)
                child.enable(enable, post_child_polished_event);
            else
                child.disable();
        }
    }

    /// Used to indicate an error on return values of index type.
    static auto constexpr invalid_index = static_cast<std::size_t>(-1);

   protected:
    auto move_event(Point new_position, Point old_position) -> bool override
    {
        this->move_active_page();
        return Layout<Child_t>::move_event(new_position, old_position);
    }

    auto resize_event(Area new_size, Area old_size) -> bool override
    {
        this->resize_active_page();
        return Layout<Child_t>::resize_event(new_size, old_size);
    }

    auto focus_in_event() -> bool override
    {
        if (active_page_ != nullptr)
            ox::System::set_focus(*active_page_);
        return this->Layout<Child_t>::focus_in_event();
    }

    void update_geometry() override {}

   private:
    Child_t* active_page_ = nullptr;
    bool sets_focus_      = true;

   private:
    void move_active_page()
    {
        if (active_page_ != nullptr)
            System::post_event(Move_event{*active_page_, this->top_left()});
    }

    void resize_active_page()
    {
        if (active_page_ != nullptr)
            System::post_event(Resize_event{*active_page_, this->outer_area()});
    }
};

/// Helper function to create an instance.
template <typename Widget_t = Widget, typename... Args>
auto stack(Args&&... args) -> std::unique_ptr<Stack<Widget_t>>
{
    return std::make_unique<Stack<Widget_t>>(std::forward<Args>(args)...);
}

}  // namespace ox::layout

// - - - - - - - - - - - - - - - - Slots - - - - - - - - - - - - - - - - - - - -
namespace ox::slot {

template <typename Child_t>
auto set_active_page(layout::Stack<Child_t>& stack)
    -> sl::Slot<void(std::size_t)>
{
    return link_lifetimes(
        [&stack](auto index) { stack.set_active_page(index); }, stack);
}

template <typename Child_t>
auto set_active_page(layout::Stack<Child_t>& stack, std::size_t index)
    -> sl::Slot<void()>
{
    return link_lifetimes([&stack, index] { stack.set_active_page(index); },
                          stack);
}

template <typename Child_t>
auto delete_page(layout::Stack<Child_t>& stack) -> sl::Slot<void(std::size_t)>
{
    return link_lifetimes([&stack](auto index) { stack.delete_page(index); },
                          stack);
}

template <typename Child_t>
auto delete_page(layout::Stack<Child_t>& stack, std::size_t index)
    -> sl::Slot<void()>
{
    return link_lifetimes([&stack, index] { stack.delete_page(index); }, stack);
}

template <typename Child_t>
auto insert_page(layout::Stack<Child_t>& stack)
    -> sl::Slot<void(std::size_t, std::unique_ptr<Widget>)>
{
    return link_lifetimes(
        [&stack](auto index, auto widget) {
            stack.insert_page(index, std::move(widget));
        },
        stack);
}

template <typename Child_t>
auto insert_page(layout::Stack<Child_t>& stack, std::size_t index)
    -> sl::Slot<void(std::unique_ptr<Widget>)>
{
    return link_lifetimes(
        [&stack, index](auto widget) {
            stack.insert_page(index, std::move(widget));
        },
        stack);
}

}  // namespace ox::slot
#endif  // TERMOX_WIDGET_LAYOUTS_STACK_HPP
