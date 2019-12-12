#ifndef CPPURSES_WIDGET_CHILDREN_HPP
#define CPPURSES_WIDGET_CHILDREN_HPP
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

// #include <cppurses/system/events/child_event.hpp>
// #include <cppurses/system/system.hpp>

namespace cppurses {
namespace detail {

/// Holds all data on children of a given Widget.
class Children {
    using List_t = std::vector<std::unique_ptr<Widget>>;
    List_t children_;
    Widget* self_;  // The parent of the children

    /// Provides Widget_t const& access to underlying child objects.
    template <typename Widget_t>
    class Const_range {
        static_assert(std::is_base_of<Widget, Widget_t>::value,
                      "Widget_t must be a Widget type.");
        List_t& children_;

        class Iterator {
            List_t::const_iterator iter_;

           public:
            Iterator(List_t::const_iterator iter) : iter_{iter} {}
            auto operator*() const -> Widget_t const&
            {
                return static_cast<Widget_t const&>(**iter_);
            }
            auto operator++() -> Iterator&
            {
                ++iter_;
                return *this;
            }
            auto operator==(Iterator other) const -> bool
            {
                return this->iter_ == other.iter_;
            }
        };

       public:
        Const_range(List_t& children) : children_{children} {}
        auto begin() const -> Iterator { return {std::begin(children_)}; }
        auto end() const -> Iterator { return {std::end(children_)}; }
    };

    /// Provides Widget_t& access to underlying child objects.
    template <typename Widget_t>
    class Range {
        static_assert(std::is_base_of<Widget, Widget_t>::value,
                      "Widget_t must be a Widget type.");
        List_t& children_;

        class Iterator {
            List_t::iterator iter_;

           public:
            Iterator(List_t::iterator iter) : iter_{iter} {}
            auto operator*() const -> Widget_t&
            {
                return static_cast<Widget_t&>(**iter_);
            }
            auto operator++() -> Iterator&
            {
                ++iter_;
                return *this;
            }
            auto operator==(Iterator other) const -> bool
            {
                return this->iter_ == other.iter_;
            }
        };

       public:
        Range(List_t& children) : children_{children} {}
        auto begin() const -> Iterator { return {std::begin(children_)}; }
        auto end() const -> Iterator { return {std::end(children_)}; }
    };

    template <typename Child_t>
    constexpr void assert_is_widget()
    {
        static_assert(std::is_base_of<Widget, Child_t>::value,
                      "Child_t must be a Widget type");
    }

   public:
    Children(Widget* self) : self_{self} {}

    /// Insert \p child directly before \p index in underlying vector.
    template <typename Child_t>
    void insert(std::unique_ptr<Child_t> child, std::size_t index)
    {
        assert_is_widget<Child_t>();
        if (child == nullptr)
            return;
        child->set_parent(self_);
        auto const iter =
            children_.emplace(std::cbegin(children_) + index, std::move(child));
        (*iter)->enable(self_->enabled());
        System::post_event<Child_added_event>(*self_, (*iter)->get());
    }

    // Is not templated, only return a Widget because you can't cast a unique
    // pointer up an inheritance hierarchy, at least with limited testing.
    // auto remove(Widget* child) -> std::unique_ptr<Widget>;

    // auto has(Widget* child) const -> bool;
    // {
    //     // find in children_, static cast to widget_t before returning;
    // }

    // auto has_descendant(Widget* descendant) const -> bool;

    // Past the first generation you are not guaranteeing that they are Child_t
    // auto get_descendants() const -> std::vector<Widget*>;
    // {
    //     // create a new vector with static casts of the children_ data.get()
    // }

    template <typename Widget_t = Widget>
    auto get_list() -> Range<Widget_t>
    {
        return Range<Widget_t>{children_};
    }

    template <typename Widget_t = Widget>
    auto get_list() const -> Const_range<Widget_t>
    {
        return Const_range<Widget_t>{children_};
    }
};

}  // namespace detail
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_CHILDREN_HPP
