#ifndef TERMOX_WIDGET_LAYOUTS_DETAIL_SHARED_SPACE_HPP
#define TERMOX_WIDGET_LAYOUTS_DETAIL_SHARED_SPACE_HPP
#include <cstddef>
#include <vector>

#include <termox/widget/size_policy.hpp>
#include <termox/widget/widget.hpp>

#include "layout_span.hpp"

namespace ox::layout::detail {

/// Divides up space between child Widgets where all Widgets share the length.
template <typename Parameters>
class Shared_space {
   private:
    using Length_list   = std::vector<int>;
    using Position_list = std::vector<int>;

   public:
    [[nodiscard]] auto calculate_lengths(Widget& parent) -> Length_list
    {
        // Disperse initial min() space to each child Widget.
        auto children = [&parent, this] {
            auto const temp = parent.get_children();
            return detail::Layout_span{
                std::next(std::begin(temp), offset_), std::end(temp),
                typename Parameters::Primary::get_length{}(parent),
                [](Widget const& w) -> Size_policy const& {
                    return typename Parameters::Primary::get_policy{}(w);
                }};
        }();

        // Have you gone over or under the avaliable space?
        auto const difference = find_length_difference(parent, children);

        if (difference > 0)
            this->disperse(children, difference);
        else if (difference < 0)
            this->reclaim(children, -1 * difference);
        return children.get_results();
    }

    /// Returns local primary dimension positions, starting at zero.
    [[nodiscard]] auto calculate_positions(Length_list const& lengths)
        -> Position_list
    {
        auto result = Position_list{};
        result.reserve(lengths.size());
        auto running_total = 0;
        for (auto length : lengths) {
            result.push_back(running_total);
            running_total += length;
        }
        return result;
    }

    /// Return the child Widget offset, the first widget included in the layout.
    [[nodiscard]] auto get_offset() const -> std::size_t { return offset_; }

    /// Sets the child Widget offset, does not do bounds checking.
    void set_offset(std::size_t index) { offset_ = index; }

   private:
    std::size_t offset_ = 0;

   private:
    template <typename Children_span>
    void disperse(Children_span& children, int surplus)
    {
        auto given_away = -1;
        while (given_away != 0) {
            given_away = 0;
            for (auto iter = children.begin_max(); iter != children.end();
                 ++iter) {
                auto const& policy    = iter.get_policy();
                auto const max_length = policy.max();
                auto const stretch_ratio =
                    policy.stretch() / children.total_stretch();
                auto to_add = int(stretch_ratio * surplus);
                if ((iter->length + to_add) > max_length)
                    to_add = max_length - iter->length;
                iter->length += to_add;
                given_away += to_add;
            }
            surplus -= given_away;
        }
        this->disperse_leftovers(children, surplus);
    }

    /// Stretch ratio can't split small surplus values into values > 1.0
    template <typename Children_span>
    void disperse_leftovers(Children_span& children, int surplus)
    {
        while (children.size() != 0 && surplus != 0) {
            for (auto iter = children.begin_max();
                 iter != children.end() && surplus != 0; ++iter) {
                iter->length += 1;
                --surplus;
            }
        }
    }

    template <typename Children_span>
    void reclaim(Children_span& children, int deficit)
    {
        auto taken_back = -1;
        while (taken_back != 0) {
            taken_back = 0;
            for (auto iter = children.begin_min(); iter != children.end();
                 ++iter) {
                auto const& policy    = iter.get_policy();
                auto const min_length = static_cast<int>(policy.min());
                auto const inverse_stretch_ratio =
                    (1. / policy.stretch()) / children.total_inverse_stretch();
                auto to_sub = static_cast<int>(inverse_stretch_ratio * deficit);
                if ((static_cast<int>(iter->length) - to_sub) < min_length)
                    to_sub = static_cast<int>(iter->length) - min_length;
                iter->length -= to_sub;
                taken_back += to_sub;
            }
            deficit -= taken_back;
        }
        this->reclaim_leftovers(children, deficit);
    }

    /// Stretch ratio can't split small deficit values into values > 1.0
    template <typename Children_span>
    void reclaim_leftovers(Children_span& children, int deficit)
    {
        while (children.size() != 0 && deficit != 0) {
            for (auto iter = children.begin_min();
                 iter != children.end() && deficit != 0; ++iter, --deficit) {
                iter->length -= 1;
            }
        }
    }

    template <typename Children_span>
    [[nodiscard]] static auto find_length_difference(Widget const& parent,
                                                     Children_span const& span)
        -> int
    {
        auto const parent_length =
            typename Parameters::Primary::get_length{}(parent);
        auto const children_entire_length = span.entire_length();
        return parent_length - children_entire_length;
    }
};

}  // namespace ox::layout::detail
#endif  // TERMOX_WIDGET_LAYOUTS_DETAIL_SHARED_SPACE_HPP
