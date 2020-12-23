#ifndef TERMOX_WIDGET_LAYOUT_HPP
#define TERMOX_WIDGET_LAYOUT_HPP
#include <algorithm>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include <termox/common/transform_view.hpp>
#include <termox/system/event.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/widget.hpp>

namespace ox {
struct Area;
struct Point;
}  // namespace ox

namespace ox::layout {

/// Provided as a uniform interface for arranging child Widgets.
/** Layout is limited to holding Child objects, which much be Widget or some
 *  derived type. */
template <typename Child = Widget>
class Layout : public Widget {
   public:
    using Child_t = Child;

    static_assert(std::is_base_of_v<Widget, Child_t>,
                  "Layout: Child type must be a Widget type.");

   private:
    template <typename UnaryPredicate>
    using enable_if_invocable = std::enable_if_t<
        std::is_invocable_v<UnaryPredicate,
                            std::add_lvalue_reference_t<Child_t>>,
        int>;

   public:
    template <typename... Widgets>
    Layout(std::unique_ptr<Widgets>... children)
    {
        (this->append_child(std::move(children)), ...);
    }

   public:
    /// Return a View of all children.
    auto get_children()
    {
        auto constexpr downcast = [](auto& widg_ptr) -> Child_t& {
            return static_cast<Child_t&>(*widg_ptr);
        };
        return Transform_view(children_, downcast);
    }

    /// Return a const View of all children.
    auto get_children() const
    {
        auto constexpr downcast = [](auto const& widg_ptr) -> Child_t const& {
            return static_cast<Child_t const&>(*widg_ptr);
        };
        return Transform_view(children_, downcast);
    }

    /// Inserts \p w at \p index, sending child added event to *this.
    /** Widget_t must be a Child_t or derived object. Inserts at end of
     *  container if \p index is out of range.  Returns a reference to the
     *  inserted Widget. No checks for nullptr. */
    template <typename Widget_t>
    auto insert_child(std::unique_ptr<Widget_t> w, std::size_t index)
        -> Widget_t&
    {
        static_assert(std::is_base_of_v<Child_t, Widget_t>,
                      "Layout::insert: Widget_t must be a Child_t type");
        if (index > this->child_count())
            index = this->child_count();
        auto& inserted = *w;
        children_.emplace(this->iter_at(index), std::move(w));
        inserted.set_parent(this);
        inserted.enable(this->is_enabled());
        System::post_event(Child_added_event{*this, inserted});
        return inserted;
    }

    /// Move \p w to the end of the child container. Forwards to insert_child()
    /** Returns a reference to the inserted Widget. */
    template <typename Widget_t>
    auto append_child(std::unique_ptr<Widget_t> w) -> Widget_t&
    {
        return this->insert_child(std::move(w), this->child_count());
    }

    /// Create a Widget and append it to the child container.
    /** Return a reference to this newly created Widget. */
    template <typename Widget_t = Child_t, typename... Args>
    auto make_child(Args&&... args) -> Widget_t&
    {
        return this->append_child(
            std::make_unique<Widget_t>(std::forward<Args>(args)...));
    }

    // TODO Remove this
    /// Helper so Parameters type does not have to be specified at call site.
    template <typename Widget_t = Child_t,
              typename SFINAE   = typename Widget_t::Parameters>
    auto make_child(typename Widget_t::Parameters p) -> Widget_t&
    {
        return this->append_child(std::make_unique<Widget_t>(std::move(p)));
    }

    /// Removes and returns the child pointed to by \p child.
    /** Returns nullptr if \p child isn't a child of *this. */
    auto remove_child(Child_t const* child) -> std::unique_ptr<Widget>
    {
        auto const at = this->find_iter(child);
        if (at == std::end(children_))
            return nullptr;
        auto removed = this->iter_remove(at);
        this->uninitialize(*removed);
        return removed;
    }

    /// Removes and returns the first child where predicate(child) returns true.
    /** If no child is found, returns nullptr. */
    template <typename UnaryPredicate,
              typename = enable_if_invocable<UnaryPredicate>>
    auto remove_child_if(UnaryPredicate&& predicate) -> std::unique_ptr<Widget>
    {
        Widget* found =
            this->find_child_if(std::forward<UnaryPredicate>(predicate));
        return this->remove_child(found);
    }

    /// Removes and returns the child at \p index in the child container.
    /** Returns nullptr if \p index is out of range. */
    auto remove_child_at(std::size_t index) -> std::unique_ptr<Widget>
    {
        if (index >= this->child_count())
            return nullptr;
        auto removed = this->iter_remove(this->iter_at(index));
        this->uninitialize(*removed);
        return removed;
    }

    /// Removes the child with given pointer and sends a Delete_event to it.
    /** Returns false if \p child is not found and deleted. */
    auto remove_and_delete_child(Child_t const* child) -> bool
    {
        auto removed = this->remove_child(child);
        if (removed == nullptr)
            return false;
        System::post_event(Delete_event{std::move(removed)});
        return true;
    }

    /// Erase first element that satisfies \p pred.
    /** Returns true if a child is found and deleted. */
    template <typename UnaryPredicate,
              typename = enable_if_invocable<UnaryPredicate>>
    auto remove_and_delete_child_if(UnaryPredicate&& predicate) -> bool
    {
        Widget* found =
            this->find_child_if(std::forward<UnaryPredicate>(predicate));
        if (found == nullptr)
            return false;
        this->remove_and_delete_child(found);
        return true;
    }

    /// Removes the child at \p index and sends a Delete_event to it.
    /** Returns false if \p index is out of range. */
    auto remove_and_delete_child_at(std::size_t index) -> bool
    {
        auto removed = this->remove_child_at(index);
        if (removed == nullptr)
            return false;
        System::post_event(Delete_event{std::move(removed)});
        return true;
    }

    /// Removes all children and sends Delete_events to each.
    void delete_all_children()
    {
        while (this->child_count() != 0uL)
            this->remove_and_delete_child_at(0uL);
    }

    /// Swap two child widgets, no index range check.
    void swap_children(std::size_t index_a, std::size_t index_b)
    {
        std::iter_swap(this->iter_at(index_a), this->iter_at(index_b));
        System::post_event(Child_polished_event{*this, *children_[index_b]});
        System::post_event(Child_polished_event{*this, *children_[index_a]});
    }

    /// Find first child satisfying \p predicate.
    /** \p predicate takes a Widget_t const reference and returns a bool.
     *  Returns nullptr if no child is found. */
    template <typename UnaryPredicate>
    auto find_child_if(UnaryPredicate&& predicate) const -> Child_t const*
    {
        auto const view = this->get_children();
        auto const found =
            std::find_if(std::cbegin(view), std::cend(view),
                         std::forward<UnaryPredicate>(predicate));
        return found == std::cend(view) ? nullptr : std::addressof(*found);
    }

    /// Find first child satisfying \p predicate.
    /** \p predicate takes a Widget_t const reference and returns a bool.
     *  Returns nullptr if no child is found. */
    template <typename UnaryPredicate>
    auto find_child_if(UnaryPredicate&& predicate) -> Child_t*
    {
        auto view = this->get_children();
        auto const found =
            std::find_if(std::begin(view), std::end(view),
                         std::forward<UnaryPredicate>(predicate));
        return found == std::end(view) ? nullptr : std::addressof(*found);
    }

    /// Find a child widget by name, returns nullptr if not found.
    auto find_child_by_name(std::string const& name) -> Child_t*
    {
        return this->find_child_if(
            [&](Child_t const& w) { return w.name() == name; });
    }

    /// Find a child widget by name, returns nullptr if not found.
    auto find_child_by_name(std::string const& name) const -> Child_t const*
    {
        return this->find_child_if(
            [&](Child_t const& w) { return w.name() == name; });
    }

    /// Finds the index of the given child pointer in the child container.
    /** Returns std::size_t(-1) if \p w is not a child of *this. */
    auto find_child_position(Child_t const* w) const -> std::size_t
    {
        auto const found = std::find_if(
            std::cbegin(children_), std::cend(children_),
            [w](std::unique_ptr<Widget> const& x) { return x.get() == w; });
        if (found == std::cend(children_))
            return -1uL;
        return std::distance(std::cbegin(children_), found);
    }

    /// Returns true if \p w is a child of *this.
    auto contains_child(Child_t const* w) const -> bool
    {
        return this->find_child(w) != nullptr;
    }

    /// Returns true if \p descendant is a child or some other child's child etc
    /** Not as efficient as it could be. */
    auto contains_descendant(Widget const* descendant) const -> bool
    {
        auto const d = this->get_descendants();
        return std::find(std::begin(d), std::end(d), descendant) != std::end(d);
    }

   protected:
    /// Clients override this to post Resize and Move events to children.
    /** This will be called each time the children Widgets possibly need to be
     *  rearranged. Triggered by Move_event, Resize_event, Child_added_event,
     *  Child_removed_event, Child_polished_event, and Enable_even\. */
    virtual void update_geometry() = 0;

    auto enable_event() -> bool override
    {
        this->update_geometry();
        return Widget::enable_event();
    }

    auto move_event(Point new_position, Point old_position) -> bool override
    {
        this->update_geometry();
        return Widget::move_event(new_position, old_position);
    }

    auto resize_event(Area new_size, Area old_size) -> bool override
    {
        this->update_geometry();
        return Widget::resize_event(new_size, old_size);
    }

    auto child_added_event(Widget& child) -> bool override
    {
        // Child_added_event can be sent even if receivier is disabled, and
        // update_geometry() is capable of enabling child widgets, so don't call
        if (this->is_enabled())
            this->update_geometry();
        return Widget::child_added_event(child);
    }

    auto child_removed_event(Widget& child) -> bool override
    {
        // Child_removed_event can be sent even if receivier is disabled, and
        // update_geometry() is capable of enabling child widgets, so don't call
        if (this->is_enabled())
            this->update_geometry();
        return Widget::child_removed_event(child);
    }

    auto child_polished_event(Widget& child) -> bool override
    {
        this->update_geometry();
        return Widget::child_polished_event(child);
    }

   protected:
    struct Dimensions {
        Widget* widget;
        std::size_t width;
        std::size_t height;
    };

    struct Dimensions_reference {
        Widget* widget;
        std::size_t* width;
        std::size_t* height;
    };

   private:
    /// Get the iterator pointing to the child at \p index into children_.
    auto iter_at(std::size_t index) -> Children_t::iterator
    {
        return std::next(std::begin(children_), index);
    }

    /// Get the iterator pointing to the child at \p index into children_.
    auto iter_at(std::size_t index) const -> Children_t::const_iterator
    {
        return std::next(std::cbegin(children_), index);
    }

    /// find_iter implementation.
    auto is_target(Child_t const* target)
    {
        return [target](std::unique_ptr<Widget> const& w) -> bool {
            return w.get() == target;
        };
    }

    /// Find the iterator pointing to \p w.
    auto find_iter(Child_t const* w) const -> Children_t::const_iterator
    {
        return std::find_if(std::cbegin(children_), std::cend(children_),
                            is_target(w));
    }

    /// Find the iterator pointing to \p w.
    auto find_iter(Child_t const* w) -> Children_t::iterator
    {
        return std::find_if(std::begin(children_), std::end(children_),
                            is_target(w));
    }

    /// Moves \p at out of the widget tree and erases the nullptr left behind.
    auto iter_remove(Children_t::iterator at) -> std::unique_ptr<Widget>
    {
        auto removed = std::move(*at);
        children_.erase(at);
        return removed;
    }

    /// Disable, post Child_remove_event to *this, and set parent to nullptr.
    void uninitialize(Widget& w)
    {
        w.disable();
        System::post_event(Child_removed_event{*this, w});
        w.set_parent(nullptr);
    }
};

}  // namespace ox::layout
#endif  // TERMOX_WIDGET_LAYOUT_HPP
