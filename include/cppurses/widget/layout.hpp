#ifndef CPPURSES_WIDGET_LAYOUT_HPP
#define CPPURSES_WIDGET_LAYOUT_HPP
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
template <typename Child_t = Widget>
class Layout : public Widget {
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

   protected:
    /// Create a Widget and append it to the list of children.
    /** Return a reference to this newly created Widget. */
    template <typename Widget_t = Child_t, typename... Args>
    auto make_child(Args&&... args) -> Widget_t&
    {
        static_assert(std::is_base_of<Child_t, Widget_t>::value,
                      "Layout::make_child: Widget_t must be a Child_t type");
        return this->Widget::children_.append(
            std::make_unique<Widget_t>(std::forward<Args>(args)...));
    }

    auto insert_child(std::unique_ptr<Child_t> child_ptr, std::size_t index)
        -> Child_t&
    {
        return this->Widget::children_.insert(std::move(child_ptr), index);
    }

    auto append_child(std::unique_ptr<Child_t> child_ptr) -> Child_t&
    {
        return this->Widget::children_.append(std::move(child_ptr));
    }

    auto remove_child(Widget const* child_ptr) -> std::unique_ptr<Widget>
    {
        return this->Widget::children_.remove(child_ptr);
    }

    auto remove_child(std::size_t index) -> std::unique_ptr<Widget>
    {
        return this->Widget::children_.remove(index);
    }

    // void swap_children(std::size_t index_a, std::size_t index_b);

    // void move_child(std::size_t initial_index, std::size_t end_index);

    // TODO Remove this method
    /// Clients override this to post Resize and Move events to children.
    /** This will be called each time the children Widgets possibly need to be
     *  rearranged. Triggered by Move_event, Resize_event, Child_added_event,
     *  Child_removed_event, and Child_polished_event. */
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
        this->update_geometry();
        return Widget::child_added_event(child);
    }

    auto child_removed_event(Widget& child) -> bool override
    {
        this->update_geometry();
        return Widget::child_removed_event(child);
    }

    auto child_polished_event(Widget& child) -> bool override
    {
        this->update_geometry();
        return Widget::child_polished_event(child);
    }

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
