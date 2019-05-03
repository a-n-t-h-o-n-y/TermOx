#ifndef CPPURSES_WIDGET_LAYOUTS_STACK_HPP
#define CPPURSES_WIDGET_LAYOUTS_STACK_HPP
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include <signals/signals.hpp>

#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>

namespace cppurses {
class Widget;
namespace layout {

/// A Layout enabling only a single Widget at a time.
/** A Stack is made up of pages, which are child Widgets that can be displayed
 *  one at a time within the Stack. The active page determines which child
 *  Widget is currently displayed. */
class Stack : public layout::Horizontal {
   public:
    /// Set child Widget to be enabled/visible via its index into child vector.
    /** The index is typically the same order as child Widgets were added.*/
    void set_active_page(std::size_t index);

    /// Set whether Focus is given to a child Widget when it becomes active.
    /** Enabled by default. */
    void give_focus_on_change(bool sets_focus = true);

    /// Construct and append a page to the Stack.
    /** This will construct a child Widget of type T, using \p args passed to
     *  T's constructor, and then automatically disable it. Returns a reference
     *  to the created child Widget. */
    template <typename T, typename... Args>
    T& make_page(Args&&... args);

    /// Add an existing Widget as a page to the end of the Stack.
    void append_page(std::unique_ptr<Widget> widget);

    /// Insert a Widget at \p index.
    /** No-op if \p index is larger than children.get().size(). */
    void insert_page(std::size_t index, std::unique_ptr<Widget> widget);

    /// Remove a page from the list, by \p index value, and delete it.
    /** No-op if index is not valid. Sets active page to nullptr if current
     *  active page is being deleted. */
    void delete_page(std::size_t index);

    /// Remove a page from the list, by \p index value, and return it.
    /** Useful if you need to move a page into another Widget. Use
     *  Stack::delete_page() if you want to remove a page and destroy it.
     *  Letting the returned Widget destroy itself will potentially leave
     *  dangling pointers in the event system. No-op if index is not valid. Sets
     *  active page to nullptr if active page is being removed. */
    std::unique_ptr<Widget> remove_page(std::size_t index);

    /// Remove and delete all pages.
    void clear();

    /// Return the number of pages contained within.
    std::size_t size() const;

    /// Return a pointer to the currently active page, or nullptr if none.
    Widget* active_page() const;

    /// Return the index of the currently active page.
    std::size_t active_page_index() const;

    /// Post an Enable_event or Disable_event to this, and all descendants.
    /** Does not post an event to any inactive pages. */
    void enable(bool enable = true,
                bool post_child_polished_event = true) override;

    /// Emitted whhen the active page is changed, sends the new index along.
    sig::Signal<void(std::size_t)> page_changed;

   private:
    Widget* active_page_{nullptr};
    bool sets_focus_{true};
};

// - - - - - - - - - - - - Template Implementations - - - - - - - - - - - - - -

template <typename T, typename... Args>
T& Stack::make_page(Args&&... args) {
    this->append_page(std::make_unique<T>(std::forward<Args>(args)...));
    return static_cast<T&>(*this->children.get().back());
}

}  // namespace layout

// - - - - - - - - - - - - - - - - Slots - - - - - - - - - - - - - - - - - - - -
namespace slot {

sig::Slot<void(std::size_t)> set_active_page(layout::Stack& stack);
sig::Slot<void()> set_active_page(layout::Stack& stack, std::size_t index);

sig::Slot<void(std::size_t)> delete_page(layout::Stack& stack);
sig::Slot<void()> delete_page(layout::Stack& stack, std::size_t index);

sig::Slot<void(std::size_t, std::unique_ptr<Widget>)> insert_page(
    layout::Stack& stack);
sig::Slot<void(std::unique_ptr<Widget>)> insert_page(layout::Stack& stack,
                                                     std::size_t index);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_LAYOUTS_STACK_HPP
