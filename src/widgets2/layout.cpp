#include <ox/widgets2/layout.hpp>

namespace {
;
;
}  // namespace

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

}  // namespace ox