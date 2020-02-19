#ifndef CPPURSES_WIDGET_LAYOUTS_DETAIL_SHARED_SPACE_HPP
#define CPPURSES_WIDGET_LAYOUTS_DETAIL_SHARED_SPACE_HPP
#include <cstddef>
#include <vector>

#include <cppurses/widget/size_policy.hpp>
#include <cppurses/widget/widget.hpp>

#include "layout_range.hpp"

namespace cppurses::layout::detail {

/// Divides up space between child Widgets where all Widgets share the length.
template <typename Parameters>
class Shared_space {
   private:
    using Length_list   = std::vector<std::size_t>;
    using Position_list = std::vector<std::size_t>;

   public:
    auto calculate_lengths(Widget& parent) -> Length_list
    {
        auto children = detail::Layout_range{
            parent.get_children(), parameters_.primary.get_length(parent),
            [](Widget const& w) -> Size_policy const& {
                return parameters_.primary.get_policy(w);
            }};

        auto const difference = this->find_length_difference(parent, children);

        if (difference > 0)
            this->disperse(children, difference);
        else if (difference < 0)
            this->reclaim(children, difference);

        return children.get_each_length();
    }

    /// Returns local primary dimension positions, starting at zero.
    auto calculate_positions(Length_list const& lengths) -> Position_list
    {
        auto result = Position_list{};
        result.reserve(lengths.size());
        auto running_total = 0uL;
        for (auto length : lengths) {
            result.push_back(running_total);
            running_total += length;
        }
        return result;
    }

   private:
    inline static auto const parameters_ = Parameters{};

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
                auto const max_length = policy.max_size();
                auto const stretch_ratio =
                    static_cast<double>(policy.stretch()) /
                    children.total_stretch();
                auto to_add = static_cast<std::size_t>(stretch_ratio * surplus);
                if ((iter->length + to_add) > max_length)
                    to_add = max_length - iter->length;
                iter->length += to_add;
                given_away += to_add;
            }
            surplus -= given_away;
        }
        // TODO
        // Give away leftovers that don't divide evenly into remaining widgets
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
                auto const min_length = policy.min_size();
                auto const inverse_stretch_ratio =
                    static_cast<double>(policy.stretch()) /
                    children.total_inverse_stretch();
                auto to_sub =
                    static_cast<std::size_t>(inverse_stretch_ratio * deficit);
                if ((static_cast<int>(iter->length) - to_sub) < min_length)
                    to_sub = static_cast<int>(iter->length) - min_length;
                iter->length -= to_sub;
                taken_back += to_sub;
            }
            deficit -= taken_back;
        }
        // TODO
        // reclaim leftovers that don't divide evenly into remaining widgets
    }

    template <typename Children_span>
    static auto find_length_difference(Widget const& parent,
                                       Children_span const& span) -> int
    {
        auto const parent_length =
            static_cast<int>(Parameters{}.primary.get_length(parent));
        auto const children_entire_length =
            static_cast<int>(span.entire_length());
        return parent_length - children_entire_length;
    }
};

}  // namespace cppurses::layout::detail
#endif  // CPPURSES_WIDGET_LAYOUTS_DETAIL_SHARED_SPACE_HPP
