#include <algorithm>
#include <termox/common/unique_queue.hpp>

#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <random>

#include <catch2/catch.hpp>

namespace {
struct Element {
    int i;
};

constexpr auto operator<(Element const& a, Element const& b) -> bool
{
    return a.i < b.i;
}

constexpr auto operator==(Element const& a, Element const& b) -> bool
{
    return a.i == b.i;
}

auto generate_elements(int count) -> std::vector<Element>
{
    auto result = std::vector<Element>{};
    result.reserve(count);
    for (auto i = 0; i < count; ++i)
        result.push_back({i});
    return result;
}

/// Appends \p count number of elements from \p elements to \p queue, randomly.
template <typename T>
void append_elements_randomly(ox::Unique_queue<T>& queue,
                              std::vector<T> const& elements,
                              int count)
{
    assert(!elements.empty());
    auto gen = std::mt19937{std::random_device{}()};
    auto dist =
        std::uniform_int_distribution<std::size_t>{0, elements.size() - 1};
    for (auto i = 0; i < count; ++i)
        queue.append(elements[dist(gen)]);
}

/// Modifying! Return true is all elements are unique.
/** Assumes value_type has .i member for comparisons. */
template <typename Container>
[[nodiscard]] auto is_unique(Container& c) -> bool
{
    std::sort(std::begin(c), std::end(c));
    return std::adjacent_find(std::cbegin(c), std::cend(c)) == std::cend(c);
}

}  // namespace

TEST_CASE("Unique elements after Compress", "[Unique_queue]")
{
    auto queue    = ox::Unique_queue<Element>{};
    auto elements = generate_elements(100);
    append_elements_randomly(queue, elements, 10'000);

    queue.compress();
    CHECK(is_unique(queue));
}

TEST_CASE("Ordering is kept stable after append", "[Unique_queue]")
{
    auto queue    = ox::Unique_queue<Element>{};
    auto elements = generate_elements(8);

    int a = 0;
    int b = 1;
    int c = 2;
    int d = 3;
    int e = 4;
    int f = 5;
    int g = 6;
    int h = 7;

    queue.append({g});
    queue.append({a});
    queue.append({c});
    queue.append({g});
    queue.append({f});
    queue.append({e});
    queue.append({e});
    queue.append({f});
    queue.append({e});
    queue.append({a});
    queue.append({f});
    queue.append({g});
    queue.append({e});
    queue.append({e});
    queue.append({d});
    queue.append({e});
    queue.append({d});
    queue.append({d});
    queue.append({b});
    queue.append({f});
    queue.append({d});
    queue.append({e});
    queue.append({a});
    queue.append({h});
    queue.append({e});
    queue.append({f});
    queue.append({g});
    queue.append({a});
    queue.append({h});
    queue.append({a});
    queue.append({g});

    queue.compress();
    REQUIRE(queue.size() == 8);
    auto const final_order = {c, b, d, e, f, h, a, g};
    auto order_iter        = std::begin(final_order);
    for (auto e : queue) {
        CHECK(*order_iter == e.i);
        ++order_iter;
    }
}
