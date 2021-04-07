#ifndef TERMOX_WIDGET_SIZE_POLICY_HPP
#define TERMOX_WIDGET_SIZE_POLICY_HPP
#include <limits>
#include <utility>

#include <signals_light/signal.hpp>

namespace ox {

/// Defines how a Layout should resize a Widget in one length Dimension.
class Size_policy {
   private:
    struct Data {
        int hint            = 0;
        int min             = 0;
        int max             = maximum_max;
        double stretch      = 1.;
        bool can_ignore_min = true;
    } data_;

   public:
    /// Emitted on any changes to the Size_policy.
    sl::Signal<void()> policy_updated;

    /// Largest possible value for max().
    static auto constexpr maximum_max =
        std::numeric_limits<decltype(data_.max)>::max();

   public:
    /// Set the size hint, used as the initial value in calculations.
    void hint(int value)
    {
        data_.hint = value;
        this->policy_updated();
    }

    /// Return the size hint currently being used.
    [[nodiscard]] auto hint() const -> int { return data_.hint; }

    /// Set the minimum length that the owning Widget should be.
    void min(int value)
    {
        data_.min = value;
        if (data_.max < data_.min)
            data_.max = data_.min;
        this->policy_updated();
    }

    /// Return the minimum length currently set.
    [[nodiscard]] auto min() const -> int { return data_.min; }

    /// Set the maximum length/height that the owning Widget can be.
    void max(int value)
    {
        data_.max = value;
        if (data_.min > data_.max)
            data_.min = data_.max;
        this->policy_updated();
    }

    /// Return the maximum length currently set.
    [[nodiscard]] auto max() const -> int { return data_.max; }

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
    [[nodiscard]] auto stretch() const -> double { return data_.stretch; }

    /// Set if min can be ignored for the last displayed widget in a layout.
    void can_ignore_min(bool enable) { data_.can_ignore_min = enable; }

    /// Return if min can be ignored for the last displayed widget in a layout.
    [[nodiscard]] auto can_ignore_min() const -> bool
    {
        return data_.can_ignore_min;
    }

    /* _Helper Methods_ */
    /// Fixed: \p hint is the only acceptable size.
    void fixed(int hint)
    {
        data_.hint    = hint;
        data_.min     = hint;
        data_.max     = hint;
        data_.stretch = 1.;
        this->policy_updated();
    }

    /// Minimum: \p hint is the minimum acceptable size, may be larger.
    void minimum(int hint)
    {
        data_.hint = hint;
        this->min(hint);
    }

    /// Maximum: \p hint is the maximum acceptable size, may be smaller.
    void maximum(int hint)
    {
        data_.hint = hint;
        this->max(hint);
    }

    /// Preferred: \p hint is preferred, though it can be any size.
    void preferred(int hint)
    {
        data_.hint = hint;
        if (hint < data_.min)
            data_.min = hint;
        if (hint > data_.max)
            data_.max = hint;
        this->policy_updated();
    }

    /// Expanding: \p hint is preferred, but it will expand to use extra space.
    void expanding(int hint)
    {
        data_.stretch = 100'000.;
        data_.hint    = hint;
        if (hint < data_.min)
            data_.min = hint;
        if (hint > data_.max)
            data_.max = hint;
        this->policy_updated();
    }

    /// Minimum Expanding: \p hint is minimum, it will expand into unused space.
    void minimum_expanding(int hint)
    {
        data_.stretch = 100'000.;
        data_.hint    = hint;
        this->min(hint);
    }

    /// Ignored: Stretch is the only consideration.
    void ignored()
    {
        data_.hint = 0;
        data_.min  = 0;
        data_.max  = maximum_max;
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
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Fixed_height : Widget_t {
    template <typename... Args>
    Fixed_height(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.fixed(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Fixed_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.fixed(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Fixed_width : Widget_t {
    template <typename... Args>
    Fixed_width(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.fixed(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Fixed_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.fixed(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Minimum_height : Widget_t {
    template <typename... Args>
    Minimum_height(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.minimum(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Minimum_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.minimum(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Minimum_width : Widget_t {
    template <typename... Args>
    Minimum_width(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.minimum(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Minimum_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.minimum(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Maximum_height : Widget_t {
    template <typename... Args>
    Maximum_height(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.maximum(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Maximum_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.maximum(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Maximum_width : Widget_t {
    template <typename... Args>
    Maximum_width(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.maximum(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Maximum_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.maximum(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Preferred_height : Widget_t {
    template <typename... Args>
    Preferred_height(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.preferred(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Preferred_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.preferred(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Preferred_width : Widget_t {
    template <typename... Args>
    Preferred_width(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.preferred(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Preferred_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.preferred(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Expanding_height : Widget_t {
    template <typename... Args>
    Expanding_height(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.expanding(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Expanding_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.expanding(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Expanding_width : Widget_t {
    template <typename... Args>
    Expanding_width(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.expanding(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Expanding_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.expanding(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Minimum_expanding_height : Widget_t {
    template <typename... Args>
    Minimum_expanding_height(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.minimum_expanding(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Minimum_expanding_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.minimum_expanding(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Minimum_expanding_width : Widget_t {
    template <typename... Args>
    Minimum_expanding_width(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.minimum_expanding(Hint);
    }

    template <typename = typename Widget_t::Parameters>
    Minimum_expanding_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.minimum_expanding(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <typename Widget_t>
struct Ignored_height : Widget_t {
    template <typename... Args>
    Ignored_height(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.ignored();
    }

    template <typename = typename Widget_t::Parameters>
    Ignored_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.ignored();
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <typename Widget_t>
struct Ignored_width : Widget_t {
    template <typename... Args>
    Ignored_width(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.ignored();
    }

    template <typename = typename Widget_t::Parameters>
    Ignored_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.ignored();
    }
};
}  // namespace ox
#endif  // TERMOX_WIDGET_SIZE_POLICY_HPP
