#include <termox/widget/size_policy.hpp>

#include <algorithm>
#include <utility>

namespace ox {

Size_policy::Size_policy(int hint,
                         int min,
                         int max,
                         double stretch,
                         bool can_ignore_min)
    : data_{hint, min, max, stretch, can_ignore_min}
{}

void Size_policy::hint(int value)
{
    data_.hint = value;
    policy_updated.emit();
}

auto Size_policy::hint() const -> int { return data_.hint; }

void Size_policy::min(int value)
{
    data_.min = value;
    policy_updated.emit();
}

auto Size_policy::min() const -> int { return data_.min; }

void Size_policy::max(int value)
{
    data_.max = value;
    policy_updated.emit();
}

auto Size_policy::max() const -> int { return data_.max; }

void Size_policy::stretch(double value)
{
    data_.stretch = value;
    policy_updated.emit();
}

auto Size_policy::stretch() const -> double { return data_.stretch; }

void Size_policy::can_ignore_min(bool enable) { data_.can_ignore_min = enable; }

auto Size_policy::can_ignore_min() const -> bool
{
    return data_.can_ignore_min;
}

auto Size_policy::fixed(int hint) -> Size_policy
{
    // {hint, min, max}
    return Size_policy{hint, hint, hint};
}

auto Size_policy::minimum(int hint) -> Size_policy
{
    // {hint, min}
    return Size_policy{hint, hint};
}

auto Size_policy::maximum(int hint) -> Size_policy
{
    // {hint, min, max}
    return Size_policy{hint, 0, hint};
}

auto Size_policy::preferred(int hint) -> Size_policy
{
    // {hint}
    return Size_policy{hint};
}

auto Size_policy::expanding(int hint) -> Size_policy
{
    // {hint, min, max, stretch}
    return Size_policy{hint, 0, maximum_max, 100'000};
}

auto Size_policy::minimum_expanding(int hint) -> Size_policy
{
    // {hint, min, max, stretch}
    return Size_policy{hint, hint, maximum_max, 100'000};
}

auto Size_policy::ignored() -> Size_policy
{
    // {hint, min, max}
    return Size_policy{0, 0, maximum_max};
}

Size_policy::Size_policy(Size_policy const& x) : data_{x.data_} {}

Size_policy::Size_policy(Size_policy&& x) : data_{std::move(x.data_)} {}

auto Size_policy::operator=(Size_policy const& x) -> Size_policy&
{
    data_ = x.data_;
    policy_updated.emit();
    return *this;
}

auto Size_policy::operator=(Size_policy&& x) -> Size_policy&
{
    data_ = std::move(x.data_);
    policy_updated.emit();
    return *this;
}

auto operator==(Size_policy const& a, Size_policy const& b) -> bool
{
    return std::tie(a.data_.hint, a.data_.min, a.data_.max, a.data_.stretch,
                    a.data_.can_ignore_min) ==
           std::tie(b.data_.hint, b.data_.min, b.data_.max, b.data_.stretch,
                    b.data_.can_ignore_min);
}

auto operator!=(Size_policy const& a, Size_policy const& b) -> bool
{
    return !(a == b);
}

auto is_valid(Size_policy const& p) -> bool
{
    return p.min() <= p.max() && p.hint() >= p.min() && p.hint() <= p.max() &&
           p.stretch() > 0.;
}

}  // namespace ox
