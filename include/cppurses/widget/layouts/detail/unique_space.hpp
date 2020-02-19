#ifndef CPPURSES_WIDGET_LAYOUTS_DETAIL_UNIQUE_SPACE_HPP
#define CPPURSES_WIDGET_LAYOUTS_DETAIL_UNIQUE_SPACE_HPP
#include <cstddef>
#include <vector>

#include <cppurses/widget/widget.hpp>

namespace cppurses::layout::detail {

/// Gives space to child Widgets where each gets full use of the length.
template <typename Parameters>
class Unique_space {
   private:
    using Length_list   = std::vector<std::size_t>;
    using Position_list = std::vector<std::size_t>;

   public:
    auto calculate_lengths(Widget& parent) -> Length_list
    {
        auto result = Length_list{};
        result.reserve(parent.child_count());
        auto const limit = parameters_.secondary.get_length(parent);
        for (auto const& child : parent.get_children()) {
            auto const& policy = parameters_.secondary.get_policy(child);
            if (limit > policy.max())
                result.push_back(policy.max());
            else if (limit < policy.min())
                result.push_back(0);
            else
                result.push_back(limit);
        }
        return result;
    }

    auto calculate_positions(Length_list const& lengths) -> Position_list
    {
        return Position_list(lengths.size(), 0uL);
    }

   private:
    inline static auto const parameters_ = Parameters{};
};

}  // namespace cppurses::layout::detail
#endif  // CPPURSES_WIDGET_LAYOUTS_DETAIL_UNIQUE_SPACE_HPP
