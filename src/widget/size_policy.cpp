#include <termox/widget/size_policy.hpp>

#include <algorithm>
#include <utility>

namespace ox {

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

void Size_policy::fixed(int hint)
{
    data_.hint    = hint;
    data_.min     = hint;
    data_.max     = hint;
    data_.stretch = 1.;
    policy_updated.emit();
}

void Size_policy::minimum(int hint)
{
    data_.hint = hint;
    data_.min  = hint;
    policy_updated.emit();
}

void Size_policy::maximum(int hint)
{
    data_.hint = hint;
    data_.max  = hint;
    policy_updated.emit();
}

void Size_policy::preferred(int hint)
{
    data_.hint = hint;
    data_.min  = 0;
    data_.max  = maximum_max;
    policy_updated.emit();
}

void Size_policy::expanding(int hint)
{
    data_.stretch = 100'000.;
    data_.hint    = hint;
    data_.min     = 0;
    data_.max     = maximum_max;
    policy_updated.emit();
}

void Size_policy::minimum_expanding(int hint)
{
    data_.stretch = 100'000.;
    data_.hint    = hint;
    data_.min     = hint;
}

void Size_policy::ignored()
{
    data_.hint = 0;
    data_.min  = 0;
    data_.max  = maximum_max;
    policy_updated.emit();
}

Size_policy::Size_policy(Size_policy const& x) : data_{x.data_} {}

Size_policy::Size_policy(Size_policy&& x) : data_{std::move(x.data_)} {}

auto Size_policy::operator=(Size_policy const& x) -> Size_policy&
{
    data_ = x.data_;
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
