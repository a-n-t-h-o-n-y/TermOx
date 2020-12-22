#ifndef TERMOX_WIDGET_SIZE_POLICY_HPP
#define TERMOX_WIDGET_SIZE_POLICY_HPP
#include <cstddef>
#include <limits>
#include <utility>

#include <signals_light/signal.hpp>

namespace ox {

/// Defines how a Layout should resize a Widget in one length Dimension.
class Size_policy {
   public:
    /// Emitted on any changes to the Size_policy.
    sl::Signal<void()> policy_updated;

   public:
    /// Set the size hint, used as the initial value in calculations.
    void hint(std::size_t value)
    {
        data_.hint = value;
        this->policy_updated();
    }

    /// Return the size hint currently being used.
    auto hint() const -> std::size_t { return data_.hint; }

    /// Set the minimum length that the owning Widget should be.
    void min(std::size_t value)
    {
        data_.min = value;
        if (data_.max < data_.min)
            data_.max = data_.min;
        this->policy_updated();
    }

    /// Return the minimum length currently set.
    auto min() const -> std::size_t { return data_.min; }

    /// Set the maximum length/height that the owning Widget can be.
    void max(std::size_t value)
    {
        data_.max = value;
        if (data_.min > data_.max)
            data_.min = data_.max;
        this->policy_updated();
    }

    /// Return the maximum length currently set.
    auto max() const -> std::size_t { return data_.max; }

    /// Set the stretch value, used to divide up space between sibling Widgets.
    /** A ratio of stretch over siblings' stretch sum is used to give space. */
    void stretch(double value)
    {
        if (value <= 0.)
            return;
        data_.stretch = value;
        this->policy_updated();
    }

    /// Return the stretch value currently being used.
    auto stretch() const -> double { return data_.stretch; }

    /// Set if min can be ignored for the last displayed widget in a layout.
    void can_ignore_min(bool enable) { data_.can_ignore_min = enable; }

    /// Return if min can be ignored for the last displayed widget in a layout.
    auto can_ignore_min() const -> bool { return data_.can_ignore_min; }

    /// Passive: Takes the sum of child Widget's hints as its fixed length.
    /** Assumes each child has a fixed() Size_policy. */
    void passive(bool x)
    {
        data_.passive = x;
        this->policy_updated();
    }

    /// Return true if Size_policy is passive.
    auto is_passive() const -> bool { return data_.passive; }

    /* _Helper Methods_ */
    /// Fixed: \p hint is the only acceptable size.
    void fixed(std::size_t hint)
    {
        data_.hint    = hint;
        data_.min     = hint;
        data_.max     = hint;
        data_.stretch = 1.;
        data_.passive = false;
        this->policy_updated();
    }

    /// Minimum: \p hint is the minimum acceptable size, may be larger.
    void minimum(std::size_t hint)
    {
        data_.hint    = hint;
        data_.passive = false;
        this->min(hint);
    }

    /// Maximum: \p hint is the maximum acceptable size, may be smaller.
    void maximum(std::size_t hint)
    {
        data_.hint    = hint;
        data_.passive = false;
        this->max(hint);
    }

    /// Preferred: \p hint is preferred, though it can be any size.
    void preferred(std::size_t hint)
    {
        data_.hint = hint;
        if (hint < data_.min)
            data_.min = hint;
        if (hint > data_.max)
            data_.max = hint;
        data_.passive = false;
        this->policy_updated();
    }

    /// Expanding: \p hint is preferred, but it will expand to use extra space.
    void expanding(std::size_t hint)
    {
        data_.stretch = 100'000.;
        data_.hint    = hint;
        if (hint < data_.min)
            data_.min = hint;
        if (hint > data_.max)
            data_.max = hint;
        data_.passive = false;
        this->policy_updated();
    }

    /// Minimum Expanding: \p hint is minimum, it will expand into unused space.
    void minimum_expanding(std::size_t hint)
    {
        data_.stretch = 100'000.;
        data_.hint    = hint;
        data_.passive = false;
        this->min(hint);
    }

    /// Ignored: Stretch is the only consideration.
    void ignored()
    {
        data_.hint    = 0;
        data_.min     = 0;
        data_.max     = max_max_;
        data_.passive = false;
        this->policy_updated();
    }

   public:
    Size_policy() = default;

    Size_policy(Size_policy const& x) = delete;
    Size_policy(Size_policy&& x)      = delete;

    /// Specifically does not copy the Signal, so Widget is still notified.
    auto operator=(Size_policy const& x) -> Size_policy&
    {
        this->data_ = x.data_;
        this->policy_updated();
        return *this;
    }

    /// Specifically does not copy the Signal, so Widget is still notified.
    auto operator=(Size_policy&& x) -> Size_policy&
    {
        this->data_ = std::move(x.data_);
        this->policy_updated();
        return *this;
    }

   private:
    struct Data {
        std::size_t hint    = 0;
        std::size_t min     = 0;
        std::size_t max     = max_max_;
        double stretch      = 1.;
        bool can_ignore_min = true;
        bool passive        = false;
    } data_;

    static auto constexpr max_max_ =
        std::numeric_limits<decltype(data_.max)>::max();
};

/// Implementation Helper
enum class Policy_direction { Vertical, Horizontal };

}  // namespace ox
#endif  // TERMOX_WIDGET_SIZE_POLICY_HPP
