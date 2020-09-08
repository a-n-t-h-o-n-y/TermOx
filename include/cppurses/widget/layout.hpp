#ifndef CPPURSES_WIDGET_LAYOUT_HPP
#define CPPURSES_WIDGET_LAYOUT_HPP
#include <algorithm>
#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include <cppurses/widget/widget.hpp>

namespace cppurses {
struct Area;
struct Point;
namespace layout {

/// Provided as a uniform interface for arranging child Widgets.
/** Layout is limited to holding Child_t objects, which much be Widget or some
 *  derived type. */
template <typename Child = Widget>
class Layout : public Widget {
   public:
    using Child_t = Child;

   public:
    template <typename... Widgets>
    Layout(std::unique_ptr<Widgets>... children)
    {
        (this->append(std::move(children)), ...);
    }

   public:
    /// Return a Widget::Range<Child_t> of all children.
    auto get_children()
    {
        return Children::View<Child_t, Widget::Children>{children_};
    }

    /// Return a Widget::Const_range<Child_t> of all children.
    auto get_children() const
    {
        return Children::View<Child_t, Widget::Children const>{children_};
    }

    /// Find a child widget by name, returns nullptr if not found.
    auto find(std::string const& name) -> Child_t*
    {
        return this->get_children().find(
            [&](auto const& w) { return w.name() == name; });
    }

    /// Create a Widget and append it to the list of children.
    /** Return a reference to this newly created Widget. */
    template <typename Widget_t = Child_t, typename... Args>
    auto make_child(Args&&... args) -> Widget_t&
    {
        static_assert(std::is_base_of_v<Child_t, Widget_t>,
                      "Layout::make_child: Widget_t must be a Child_t type");
        return this->append(
            std::make_unique<Widget_t>(std::forward<Args>(args)...));
    }

    /// Helper so Parameters type does not have to be specified at call site.
    template <typename Widget_t,
              typename SFINAE = typename Widget_t::Parameters>
    auto make_child(typename Widget_t::Parameters p) -> Widget_t&
    {
        static_assert(std::is_base_of_v<Child_t, Widget_t>,
                      "Layout::make_child: Widget_t must be a Child_t type");
        return this->append(std::make_unique<Widget_t>(std::move(p)));
    }

    template <typename Widget_t>
    auto insert(std::unique_ptr<Widget_t> child, std::size_t index) -> Widget_t&
    {
        return this->Widget::children_.insert(std::move(child), index);
    }

    /// Move \p child to end of underlying vector.
    template <typename Widget_t>
    auto append(std::unique_ptr<Widget_t> child) -> Widget_t&
    {
        return this->insert(std::move(child), this->child_count());
    }

    auto remove(Widget const* child) -> std::unique_ptr<Widget>
    {
        return this->Widget::children_.remove(child);
    }

    auto remove(std::size_t index) -> std::unique_ptr<Widget>
    {
        return this->Widget::children_.remove(index);
    }

    /// Erase first element that satisfies \p pred. Return true if erase happens
    template <typename Unary_predicate_t,
              std::enable_if_t<
                  std::is_invocable_v<Unary_predicate_t,
                                      std::add_lvalue_reference_t<Child_t>>,
                  int> = 0>
    auto erase(Unary_predicate_t&& pred) -> bool
    {
        auto child =
            this->get_children().find(std::forward<Unary_predicate_t>(pred));
        if (child == nullptr)
            return false;
        this->erase(child);
        return true;
    }

    void erase(Widget const* child) { this->Widget::children_.erase(child); }

    void erase(std::size_t index) { this->Widget::children_.erase(index); }

    void clear() { this->Widget::children_.clear(); }

    void swap(std::size_t index_a, std::size_t index_b)
    {
        this->Widget::children_.swap(index_a, index_b);
    }

    // TODO
    // void move(std::size_t initial_index, std::size_t end_index);

   protected:
    // TODO Remove this method
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
};

}  // namespace layout
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_LAYOUT_HPP
