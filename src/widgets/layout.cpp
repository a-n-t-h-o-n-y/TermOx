#include <ox/widgets/layout.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <numeric>
#include <utility>
#include <vector>

#include <ox/widgets/border.hpp>
#include <ox/widgets/focus.hpp>

namespace {
using namespace ox;

/**
 * Calculate the length of each child in the given total space based on the its
 * SizePolicy and the total space available.
 *
 * @param children The children to distribute the space between.
 * @param total The total space to distribute.
 * @return A vector of the lengths of each child.
 */
[[nodiscard]] auto distribute_length(std::vector<SizePolicy>& size_policies,
                                     int total) -> std::vector<int>
{
    assert(total >= 0);

    for (auto const& policy : size_policies) {
        assert(policy.minimum >= 0);
        assert(policy.maximum >= policy.minimum);
        assert(policy.flexibility >= 0);
    }

    auto exact_amounts = std::vector<float>(size_policies.size(), 0.f);
    auto total_allocated = 0.f;

    // Distribute minimums first
    for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
        exact_amounts[i] = (float)size_policies[i].minimum;
        total_allocated += exact_amounts[i];
    }

    // TODO
    // I do not believe this removes space from widgets if the sum of mins is greater
    // than the total space available.
    // You could have a check here.
    // if (total_allocated >= total) {
    //     auto results = std::vector<int>(size_policies.size(), 0);

    //     // floor values to ints.
    //     std::ranges::copy(exact_amounts, results.begin());
    //     return results;
    // }

    // Distribute flex space
    auto remaining_space = (float)total - total_allocated;
    while (remaining_space > 0) {
        auto const total_flex = [&] {
            auto x = 0.f;
            for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
                auto const& policy = size_policies[i];
                if (exact_amounts[i] < (float)policy.maximum) {
                    x += policy.flexibility;
                }
            }
            return x;
        }();

        if (total_flex == 0.f) {
            break;
        }

        auto space_distributed_this_round = 0.f;
        for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
            auto const& policy = size_policies[i];
            if (exact_amounts[i] < (float)policy.maximum) {
                float additional_space =
                    std::min(policy.flexibility / total_flex * (float)remaining_space,
                             (float)policy.maximum - exact_amounts[i]);
                exact_amounts[i] += additional_space;
                space_distributed_this_round += additional_space;
            }
        }
        remaining_space -= space_distributed_this_round;
        if (space_distributed_this_round == 0) {
            break;
        }
    }
    auto results = std::vector<int>(size_policies.size(), 0);

    // floor values to ints.
    std::ranges::copy(exact_amounts, results.begin());

    // Distribute remaining space from left from flooring.
    int remaining = total - std::accumulate(std::begin(results), std::end(results), 0);
    while (remaining > 0) {
        auto space_distributed_this_round = 0;
        for (auto i = std::size_t{0}; i < size_policies.size(); ++i) {
            if (results[i] < size_policies[i].maximum) {
                results[i] += 1;
                remaining -= 1;
                ++space_distributed_this_round;
                if (remaining == 0) {
                    break;
                }
            }
        }
        if (space_distributed_this_round == 0) {
            break;
        }
    }
    return results;
}

}  // namespace

// -------------------------------------------------------------------------------------

namespace ox {

auto SizePolicy::fixed(int size) -> SizePolicy
{
    return {.minimum = size, .maximum = size, .flexibility = 0.f};
}

auto SizePolicy::flex(float flex) -> SizePolicy
{
    return {
        .minimum = 0, .maximum = std::numeric_limits<int>::max(), .flexibility = flex};
}

auto SizePolicy::bounded(int min, int max) -> SizePolicy
{
    return {.minimum = min, .maximum = max, .flexibility = 1.f};
}

auto SizePolicy::min(int min) -> SizePolicy
{
    return {
        .minimum = min, .maximum = std::numeric_limits<int>::max(), .flexibility = 1.f};
}

auto SizePolicy::max(int max) -> SizePolicy
{
    return {.minimum = 0, .maximum = max, .flexibility = 1.f};
}

// -------------------------------------------------------------------------------------

auto remove(LinearLayout& layout, Widget const& w) -> Widget
{
    auto const iter = std::ranges::find(layout.children, &w,
                                        [](Widget const& child) { return &child; });

    if (iter == std::end(layout.children)) {
        throw std::out_of_range{"remove: Widget not found in layout"};
    }
    auto const index = std::distance(std::begin(layout.children), iter);
    layout.size_policies.erase(std::next(std::begin(layout.size_policies), index));
    auto removed = std::move(*iter);
    layout.children.erase(iter);
    return removed;
}

auto remove_at(LinearLayout& layout, std::size_t index) -> Widget
{
    if (index >= layout.children.size()) {
        throw std::out_of_range{"remove_at: index out of range"};
    }

    layout.size_policies.erase(
        std::next(std::begin(layout.size_policies), (std::ptrdiff_t)index));
    auto iter = std::next(std::begin(layout.children), (std::ptrdiff_t)index);
    auto removed = std::move(*iter);
    layout.children.erase(iter);
    return removed;
}

auto remove_all(LinearLayout& layout) -> std::vector<Widget>
{
    auto removed = std::move(layout.children);
    layout.children.clear();
    layout.size_policies.clear();
    return removed;
}

// -------------------------------------------------------------------------------------

auto resize(HLayout& layout, Area a) -> void
{
    auto x = 0;
    auto const widths = ::distribute_length(layout.size_policies, a.width);
    for (auto i = std::size_t{0}; i < layout.children.size(); ++i) {
        auto& child = layout.children[i];
        child.at = {x, 0};
        child.size = {widths[i], a.height};
        resize(child, child.size);
        x += widths[i];
    }
}

auto append_divider(HLayout& layout, Glyph line) -> Divider&
{
    return layout.append(Divider{line}, SizePolicy::fixed(1));
}

// -------------------------------------------------------------------------------------

auto resize(VLayout& layout, Area a) -> void
{
    auto y = 0;
    auto const heights = ::distribute_length(layout.size_policies, a.height);
    for (auto i = std::size_t{0}; i < layout.children.size(); ++i) {
        auto& child = layout.children[i];
        child.at = {0, y};
        child.size = {a.width, heights[i]};
        resize(child, child.size);
        y += heights[i];
    }
}

auto append_divider(VLayout& layout, Glyph line) -> Divider&
{
    return layout.append(Divider{line}, SizePolicy::fixed(1));
}

}  // namespace ox