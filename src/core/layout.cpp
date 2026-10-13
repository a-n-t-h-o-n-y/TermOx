#include <ox/core/layout.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace ox {
namespace {

template <typename Remainder>
void round_lengths(std::vector<int>& lengths,
                   std::vector<std::pair<Remainder, std::size_t>>& remainders,
                   int remaining)
{
    std::ranges::sort(remainders, [](auto const& a, auto const& b) {
        return a.first != b.first ? a.first > b.first : a.second < b.second;
    });
    for (auto const& [remainder, index] : remainders) {
        if (remaining == 0) { break; }
        ++lengths[index];
        --remaining;
    }
}

}  // namespace

auto distribute_length(std::span<SizePolicy const> policies, int available)
    -> std::vector<int>
{
    if (available < 0) { throw std::invalid_argument{"Negative layout length"}; }
    auto minimum_total = std::int64_t{0};
    for (auto const& policy : policies) {
        if (policy.minimum < 0 || policy.maximum < policy.minimum ||
            !std::isfinite(policy.flexibility) || policy.flexibility < 0) {
            throw std::invalid_argument{"Invalid size policy"};
        }
        if (minimum_total > std::numeric_limits<std::int64_t>::max() - policy.minimum) {
            throw std::length_error{"Layout minimum total overflow"};
        }
        minimum_total += policy.minimum;
    }
    auto lengths = std::vector<int>(policies.size(), 0);
    if (available == 0) { return lengths; }
    if (minimum_total > available) {
        // Integer ratios keep shrinking exact even at INT_MAX dimensions.
        auto remainders = std::vector<std::pair<std::int64_t, std::size_t>>{};
        remainders.reserve(policies.size());
        auto remaining = available;
        for (auto i = std::size_t{0}; i < policies.size(); ++i) {
            auto const product = std::int64_t{available} * policies[i].minimum;
            lengths[i] = static_cast<int>(product / minimum_total);
            remaining -= lengths[i];
            if (policies[i].minimum > 0) {
                remainders.emplace_back(product % minimum_total, i);
            }
        }
        round_lengths(lengths, remainders, remaining);
        return lengths;
    }
    for (auto i = std::size_t{0}; i < policies.size(); ++i) {
        lengths[i] = policies[i].minimum;
    }
    auto remaining = available - static_cast<int>(minimum_total);
    if (remaining == 0) { return lengths; }

    // Visit growth caps in the order they would be reached. A suffix sum retains
    // tiny weights when large weights saturate, without subtractive cancellation.
    auto order = std::vector<std::size_t>{};
    order.reserve(policies.size());
    for (auto i = std::size_t{0}; i < policies.size(); ++i) {
        if (policies[i].flexibility > 0 && policies[i].maximum > lengths[i]) {
            order.push_back(i);
        }
    }
    auto capacity = [&](std::size_t i) { return policies[i].maximum - lengths[i]; };
    std::ranges::sort(order, [&](auto a, auto b) {
        auto const left =
            static_cast<long double>(capacity(a)) * policies[b].flexibility;
        auto const right =
            static_cast<long double>(capacity(b)) * policies[a].flexibility;
        return left != right ? left < right : a < b;
    });
    auto weights = std::vector<long double>(order.size() + 1, 0);
    for (auto i = order.size(); i > 0; --i) {
        weights[i - 1] = weights[i] + policies[order[i - 1]].flexibility;
    }
    for (auto i = std::size_t{0}; i < order.size(); ++i) {
        auto const index = order[i];
        auto const cap = capacity(index);
        if (static_cast<long double>(cap) * weights[i] <=
            static_cast<long double>(remaining) * policies[index].flexibility) {
            lengths[index] += cap;
            remaining -= cap;
            continue;
        }
        auto remainders = std::vector<std::pair<long double, std::size_t>>{};
        remainders.reserve(order.size() - i);
        auto unassigned = remaining;
        for (auto j = i; j < order.size(); ++j) {
            auto const child = order[j];
            auto const share = static_cast<long double>(remaining) *
                               policies[child].flexibility / weights[i];
            auto const growth = std::min(
                {static_cast<int>(std::floor(share)), capacity(child), unassigned});
            lengths[child] += growth;
            unassigned -= growth;
            if (lengths[child] < policies[child].maximum) {
                remainders.emplace_back(share - growth, child);
            }
        }
        round_lengths(lengths, remainders, unassigned);
        break;
    }
    return lengths;
}

auto layout_row(std::span<SizePolicy const> policies, Area available)
    -> std::vector<Rect>
{
    validate(available);
    auto const lengths = distribute_length(policies, available.width);
    auto result = std::vector<Rect>{};
    result.reserve(lengths.size());
    auto x = 0;
    for (auto length : lengths) {
        result.push_back({{x, 0}, {length, available.height}});
        x += length;
    }
    return result;
}

auto layout_column(std::span<SizePolicy const> policies, Area available)
    -> std::vector<Rect>
{
    auto result = layout_row(policies, {available.height, available.width});
    for (auto& rectangle : result) {
        std::swap(rectangle.at.x, rectangle.at.y);
        std::swap(rectangle.size.width, rectangle.size.height);
    }
    return result;
}

auto centered_rect(Area available, Area requested) -> Rect
{
    validate(available);
    validate(requested);
    auto const size = Area{std::min(available.width, requested.width),
                           std::min(available.height, requested.height)};
    return {{(available.width - size.width) / 2, (available.height - size.height) / 2},
            size};
}

}  // namespace ox
