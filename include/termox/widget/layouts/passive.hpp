#ifndef TERMOX_WIDGET_LAYOUTS_PASSIVE_HPP
#define TERMOX_WIDGET_LAYOUTS_PASSIVE_HPP
#include <memory>
#include <utility>

#include <termox/widget/detail/border_offset.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Make any Linear_layout type passive.
/** A Passive Layout will always have the size policy that is the sum of its
 *  children's size hints. */
template <typename Layout_t>
class Passive : public Layout_t {
   private:
    using Base_t                      = Layout_t;
    static auto constexpr is_vertical = layout::is_vertical_v<Base_t>;

   public:
    template <typename... Args>
    Passive(Args&&... args) : Base_t{std::forward<Args>(args)...}
    {
        this->set_length(
            sum_hints(this->get_children(), this->get_child_offset()));
    }

   protected:
    auto child_added_event(ox::Widget& child) -> bool override
    {
        this->set_length(
            sum_hints(this->get_children(), this->get_child_offset()));
        return Base_t::child_added_event(child);
    }

    auto child_removed_event(ox::Widget& child) -> bool override
    {
        this->set_length(
            sum_hints(this->get_children(), this->get_child_offset()));
        return Base_t::child_removed_event(child);
    }

    auto child_polished_event(ox::Widget& child) -> bool override
    {
        this->set_length(
            sum_hints(this->get_children(), this->get_child_offset()));
        return Base_t::child_polished_event(child);
    }

   private:
    /// Return the sum of all hints in the inherited from Layout_t direction.
    static auto sum_hints(decltype(std::declval<Layout_t>().get_children())
                              const& children,
                          std::size_t begin_offset) -> int
    {
        auto get_hint = [](Widget const& child) {
            if constexpr (is_vertical)
                return child.height_policy.hint();
            else
                return child.width_policy.hint();
        };

        auto sum = 0;
        for (auto i = begin_offset; i < children.size(); ++i)
            sum += get_hint(children[i]);
        return sum;
    }

    void set_length(int length)
    {
        // TODO this can be removed once Borders are their own thing.
        if constexpr (is_vertical) {
            length += detail::Border_offset::north_enabled(*this) ? 1 : 0;
            length += detail::Border_offset::south_enabled(*this) ? 1 : 0;
        }
        else {
            length += detail::Border_offset::east_enabled(*this) ? 1 : 0;
            length += detail::Border_offset::west_enabled(*this) ? 1 : 0;
        }
        if constexpr (is_vertical)
            *this | pipe::fixed_height(length);
        else
            *this | pipe::fixed_width(length);
    }
};

/// Helper function to create an instance.
template <typename Layout_t, typename... Args>
[[nodiscard]] inline auto passive(Args&&... args)
    -> std::unique_ptr<Passive<Layout_t>>
{
    return std::make_unique<Passive<Layout_t>>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_LAYOUTS_PASSIVE_HPP
