#include <zzz/test.hpp>

#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <numeric>
#include <random>

// The calculation API deliberately has no widgets, coroutines, or terminal sessions.
#include <ox/core/layout.hpp>

using namespace ox;

TEST(layout_empty_and_zero_space_are_defined)
{
    ASSERT(distribute_length({}, 100).empty());
    ASSERT(layout_row({}, {100, 20}).empty());
    auto const policies = std::array{SizePolicy::fixed(4), SizePolicy::min(2)};
    ASSERT((distribute_length(policies, 0) == std::vector<int>{0, 0}));
    ASSERT((layout_column(policies, {10, 0}) ==
            std::vector<Rect>{{{0, 0}, {10, 0}}, {{0, 0}, {10, 0}}}));
}

TEST(layout_exact_minimums_fill_the_available_length)
{
    auto const policies =
        std::array{SizePolicy::fixed(2), SizePolicy::fixed(3), SizePolicy::min(4)};
    ASSERT((distribute_length(policies, 9) == std::vector<int>{2, 3, 4}));
    ASSERT((distribute_length(std::array{SizePolicy::fixed(1)}, 1) ==
            std::vector<int>{1}));
}

TEST(layout_insufficient_space_shrinks_proportionally_with_stable_rounding)
{
    auto const policies =
        std::array{SizePolicy::fixed(2), SizePolicy::fixed(6), SizePolicy::fixed(0)};
    ASSERT((distribute_length(policies, 4) == std::vector<int>{1, 3, 0}));
    ASSERT((distribute_length(policies, 3) == std::vector<int>{1, 2, 0}));
    auto const equal =
        std::array{SizePolicy::fixed(2), SizePolicy::fixed(2), SizePolicy::fixed(2)};
    ASSERT((distribute_length(equal, 4) == std::vector<int>{2, 1, 1}));
    ASSERT((distribute_length(equal, 1) == std::vector<int>{1, 0, 0}));
}

TEST(layout_growth_uses_weights_and_largest_remainders)
{
    auto const weighted = std::array{SizePolicy::flex(1), SizePolicy::flex(3)};
    ASSERT((distribute_length(weighted, 10) == std::vector<int>{3, 7}));
    ASSERT((distribute_length(weighted, 3) == std::vector<int>{1, 2}));
    auto const equal =
        std::array{SizePolicy::flex(), SizePolicy::flex(), SizePolicy::flex()};
    ASSERT((distribute_length(equal, 5) == std::vector<int>{2, 2, 1}));
    auto const minima = std::array{SizePolicy{2, 10, 1}, SizePolicy{4, 12, 3}};
    ASSERT((distribute_length(minima, 10) == std::vector<int>{3, 7}));
}

TEST(layout_growth_redistributes_after_caps_and_leaves_excess_unused)
{
    auto const policies =
        std::array{SizePolicy{0, 1, 9}, SizePolicy{0, 3, 3}, SizePolicy{0, 20, 1}};
    ASSERT((distribute_length(policies, 12) == std::vector<int>{1, 3, 8}));
    ASSERT((distribute_length(policies, 30) == std::vector<int>{1, 3, 20}));
    auto const fractional_cap = std::array{SizePolicy{0, 2, 2}, SizePolicy{0, 10, 1}};
    ASSERT((distribute_length(fractional_cap, 4) == std::vector<int>{2, 2}));
}

TEST(layout_zero_weight_never_grows_even_during_rounding)
{
    auto const policies =
        std::array{SizePolicy{2, 100, 0}, SizePolicy{1, 3, 1}, SizePolicy{0, 100, 0}};
    ASSERT((distribute_length(policies, 30) == std::vector<int>{2, 3, 0}));
    ASSERT((distribute_length(std::array{SizePolicy::flex(0), SizePolicy::min(0)}, 1) ==
            std::vector<int>{0, 1}));
    ASSERT((distribute_length(std::array{SizePolicy{2, 100, 0}}, 40) ==
            std::vector<int>{2}));
}

TEST(layout_rejects_invalid_input_including_at_zero_available_space)
{
    ASSERT_THROWS(distribute_length({}, -1), std::invalid_argument);
    auto const bad =
        std::array{SizePolicy{-1, 2, 1}, SizePolicy{3, 2, 1}, SizePolicy{0, 2, -1},
                   SizePolicy{0, 2, std::numeric_limits<float>::infinity()},
                   SizePolicy{0, 2, std::numeric_limits<float>::quiet_NaN()}};
    for (auto policy : bad) {
        ASSERT_THROWS(distribute_length(std::span{&policy, 1}, 0),
                      std::invalid_argument);
        ASSERT_THROWS(distribute_length(std::span{&policy, 1}, 10),
                      std::invalid_argument);
    }
    ASSERT_THROWS((layout_row({}, {1, -1})), std::invalid_argument);
    ASSERT_THROWS((layout_column({}, {-1, 1})), std::invalid_argument);
    ASSERT_THROWS((centered_rect({1, 1}, {-1, 0})), std::invalid_argument);
    ASSERT_THROWS((centered_rect({-1, 1}, {0, 0})), std::invalid_argument);
}

TEST(layout_handles_extreme_lengths_without_per_cell_work_or_overflow)
{
    auto const limit = std::numeric_limits<int>::max();
    auto const fixed = std::array{SizePolicy::fixed(limit), SizePolicy::fixed(limit)};
    ASSERT((distribute_length(fixed, 3) == std::vector<int>{2, 1}));
    ASSERT((distribute_length(fixed, limit) ==
            std::vector<int>{limit / 2 + 1, limit / 2}));
    auto const flex = std::array{SizePolicy::flex(), SizePolicy::flex()};
    ASSERT(
        (distribute_length(flex, limit) == std::vector<int>{limit / 2 + 1, limit / 2}));
    auto const rects = layout_row(flex, {limit, limit});
    ASSERT((rects[1] == Rect{{limit / 2 + 1, 0}, {limit / 2, limit}}));
}

TEST(layout_tiny_weights_survive_saturation_of_large_weights)
{
    auto const policies =
        std::array{SizePolicy{0, 2, std::numeric_limits<float>::max()},
                   SizePolicy{0, 100, std::numeric_limits<float>::denorm_min()}};
    ASSERT((distribute_length(policies, 100) == std::vector<int>{2, 98}));
    auto const rescaled = std::array{SizePolicy::flex(0.25f), SizePolicy::flex(0.75f)};
    ASSERT((distribute_length(rescaled, 10) == std::vector<int>{3, 7}));
}

TEST(layout_rectangles_compose_on_both_axes)
{
    auto const policies =
        std::array{SizePolicy::fixed(2), SizePolicy::max(4), SizePolicy::flex()};
    ASSERT((layout_row(policies, {12, 3}) ==
            std::vector<Rect>{{{0, 0}, {2, 3}}, {{2, 0}, {4, 3}}, {{6, 0}, {6, 3}}}));
    ASSERT((layout_column(policies, {3, 12}) ==
            std::vector<Rect>{{{0, 0}, {3, 2}}, {{0, 2}, {3, 4}}, {{0, 6}, {3, 6}}}));
    ASSERT((centered_rect({10, 8}, {3, 3}) == Rect{{3, 2}, {3, 3}}));
    ASSERT((centered_rect({2, 1}, {3, 8}) == Rect{{0, 0}, {2, 1}}));
    ASSERT((centered_rect({0, 0}, {3, 8}) == Rect{{0, 0}, {0, 0}}));
}

TEST(layout_random_policies_respect_bounds_and_conserve_eligible_space)
{
    auto random = std::mt19937{17};
    for (int trial = 0; trial < 1500; ++trial) {
        auto policies = std::vector<SizePolicy>{};
        auto minimum = std::int64_t{0};
        auto eligible = std::int64_t{0};
        auto const count = random() % 20;
        for (unsigned i = 0; i < count; ++i) {
            auto const low = static_cast<int>(random() % 20);
            auto const high = low + static_cast<int>(random() % 40);
            auto const weight = static_cast<float>(random() % 8);
            policies.push_back({low, high, weight});
            minimum += low;
            eligible += weight == 0 ? low : high;
        }
        auto const available = static_cast<int>(random() % 1000);
        auto const lengths = distribute_length(policies, available);
        ASSERT(lengths.size() == policies.size());
        ASSERT(std::accumulate(lengths.begin(), lengths.end(), std::int64_t{0}) ==
               std::min(std::int64_t{available}, eligible));
        ASSERT(distribute_length(policies, available) == lengths);
        for (auto i = std::size_t{0}; i < lengths.size(); ++i) {
            ASSERT(lengths[i] >= 0);
            ASSERT(lengths[i] <= policies[i].maximum);
            if (minimum <= available) {
                ASSERT(lengths[i] >= policies[i].minimum);
                if (policies[i].flexibility == 0) {
                    ASSERT(lengths[i] == policies[i].minimum);
                }
            }
            else {
                ASSERT(lengths[i] <= policies[i].minimum);
            }
        }
    }
}
