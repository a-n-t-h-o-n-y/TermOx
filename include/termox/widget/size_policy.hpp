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
    void hint(int value);

    /// Return the size hint currently being used.
    [[nodiscard]] auto hint() const -> int;

    /// Set the minimum length that the owning Widget should be.
    void min(int value);

    /// Return the minimum length currently set.
    [[nodiscard]] auto min() const -> int;

    /// Set the maximum length/height that the owning Widget can be.
    void max(int value);

    /// Return the maximum length currently set.
    [[nodiscard]] auto max() const -> int;

    /// Set the stretch value, used to divide up space between sibling Widgets.
    /** A ratio of stretch over siblings' stretch sum is used to give space. */
    void stretch(double value);

    /// Return the stretch value currently being used.
    [[nodiscard]] auto stretch() const -> double;

    /// Set if min can be ignored for the last displayed widget in a layout.
    void can_ignore_min(bool enable);

    /// Return if min can be ignored for the last displayed widget in a layout.
    [[nodiscard]] auto can_ignore_min() const -> bool;

    /* _Helper Methods_ */
    /// Fixed: \p hint is the only acceptable size.
    void fixed(int hint);

    /// Minimum: \p hint is the minimum acceptable size, may be larger.
    void minimum(int hint);

    /// Maximum: \p hint is the maximum acceptable size, may be smaller.
    void maximum(int hint);

    /// Preferred: \p hint is preferred, though it can be any size.
    void preferred(int hint);

    /// Expanding: \p hint is preferred, but it will expand to use extra space.
    void expanding(int hint);

    /// Minimum Expanding: \p hint is minimum, it will expand into unused space.
    void minimum_expanding(int hint);

    /// Ignored: Stretch is the only consideration.
    void ignored();

   public:
    Size_policy() = default;

    /// Does not copy the Signal, so no slots are connected on copy init.
    Size_policy(Size_policy const& x);

    /// Does not move the Signal, so no slots are connected on move init.
    Size_policy(Size_policy&& x);

    /// Specifically does not copy the Signal, so Widget is still notified.
    auto operator=(Size_policy const& x) -> Size_policy&;
    auto operator=(Size_policy&& x) -> Size_policy& = delete;

    friend auto operator==(Size_policy const& a, Size_policy const& b) -> bool;
    friend auto operator!=(Size_policy const& a, Size_policy const& b) -> bool;
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Fixed_height : Widget_t {
    template <typename... Args>
    explicit Fixed_height(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.fixed(Hint);
    }

    explicit Fixed_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.fixed(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Fixed_width : Widget_t {
    template <typename... Args>
    explicit Fixed_width(Args&&... args) : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.fixed(Hint);
    }

    explicit Fixed_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.fixed(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Minimum_height : Widget_t {
    template <typename... Args>
    explicit Minimum_height(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.minimum(Hint);
    }

    explicit Minimum_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.minimum(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Minimum_width : Widget_t {
    template <typename... Args>
    explicit Minimum_width(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.minimum(Hint);
    }

    explicit Minimum_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.minimum(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Maximum_height : Widget_t {
    template <typename... Args>
    explicit Maximum_height(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.maximum(Hint);
    }

    explicit Maximum_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.maximum(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Maximum_width : Widget_t {
    template <typename... Args>
    explicit Maximum_width(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.maximum(Hint);
    }

    explicit Maximum_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.maximum(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Preferred_height : Widget_t {
    template <typename... Args>
    explicit Preferred_height(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.preferred(Hint);
    }

    explicit Preferred_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.preferred(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Preferred_width : Widget_t {
    template <typename... Args>
    explicit Preferred_width(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.preferred(Hint);
    }

    explicit Preferred_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.preferred(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Expanding_height : Widget_t {
    template <typename... Args>
    explicit Expanding_height(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.expanding(Hint);
    }

    explicit Expanding_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.expanding(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Expanding_width : Widget_t {
    template <typename... Args>
    explicit Expanding_width(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.expanding(Hint);
    }

    explicit Expanding_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.expanding(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <int Hint, typename Widget_t>
struct Minimum_expanding_height : Widget_t {
    template <typename... Args>
    explicit Minimum_expanding_height(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.minimum_expanding(Hint);
    }

    explicit Minimum_expanding_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.minimum_expanding(Hint);
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <int Hint, typename Widget_t>
struct Minimum_expanding_width : Widget_t {
    template <typename... Args>
    explicit Minimum_expanding_width(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.minimum_expanding(Hint);
    }

    explicit Minimum_expanding_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.minimum_expanding(Hint);
    }
};

/// Wrapper type to set the height Size_policy at construction.
template <typename Widget_t>
struct Ignored_height : Widget_t {
    template <typename... Args>
    explicit Ignored_height(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->height_policy.ignored();
    }

    explicit Ignored_height(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->height_policy.ignored();
    }
};

/// Wrapper type to set the width Size_policy at construction.
template <typename Widget_t>
struct Ignored_width : Widget_t {
    template <typename... Args>
    explicit Ignored_width(Args&&... args)
        : Widget_t{std::forward<Args>(args)...}
    {
        this->width_policy.ignored();
    }

    explicit Ignored_width(typename Widget_t::Parameters parameters)
        : Widget_t{std::move(parameters)}
    {
        this->width_policy.ignored();
    }
};

/// Return true if all members make sense in relation to eachother.
[[nodiscard]] auto is_valid(Size_policy const& p) -> bool;

}  // namespace ox
#endif  // TERMOX_WIDGET_SIZE_POLICY_HPP
