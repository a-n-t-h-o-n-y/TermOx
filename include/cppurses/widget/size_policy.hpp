#ifndef CPPURSES_WIDGET_SIZE_POLICY_HPP
#define CPPURSES_WIDGET_SIZE_POLICY_HPP
#include <cstddef>
#include <limits>
#include <utility>

#include <signals/signal.hpp>

namespace cppurses {

/// Defines how a Layout should resize a Widget in one length Dimension.
class Size_policy {
   public:
    /// Connect to this to be notified of changes to the size policy
    sig::Signal<void()> policy_updated;

    /// Descriptions of how the size hint should be used.
    /** Fixed: hint is the only acceptable size. */
    /** Minimum: hint is the minimum acceptable size, may be larger. */
    /** Maximum: hint is the maximum acceptable size, may be smaller. */
    /** Preferred: hint is preferred, though it can be any size. */
    /** Expanding: hint is preferred, but it will expand to use extra space. */
    /** MinimumExpanding: hint is minimum, it will expand into unused space. */
    /** Ignored: hint is ignored and stretch factor is the only consideration */
    enum Type {
        Fixed,
        Minimum,
        Maximum,
        Preferred,
        Expanding,
        MinimumExpanding,
        Ignored
    };

   public:
    // None of these copies/replaces the policy_updated Signal.
    // Signal is attached to the Widget, not the Size_policy.
    Size_policy() = default;
    Size_policy(Size_policy const& x) : data_{x.data_} {}
    Size_policy(Size_policy&& x) : data_{std::move(x.data_)} {}

    auto operator=(Size_policy const& x) -> Size_policy&
    {
        this->data_ = x.data_;
        return *this;
    }

    auto operator=(Size_policy&& x) -> Size_policy&
    {
        this->data_ = std::move(x.data_);
        return *this;
    }

    /// Set the type to Fixed with size hint of \p hint.
    void fixed(std::size_t hint)
    {
        data_.type    = Type::Fixed;
        data_.hint    = hint;
        data_.min     = hint;
        data_.max     = hint;
        data_.stretch = 1;
        this->policy_updated();
    }

    /// Set the type to Minimum with size hint of \p hint.
    void minimum(std::size_t hint)
    {
        data_.type = Type::Minimum;
        data_.hint = hint;
        data_.min  = hint;  // TODO set_min() does the check
        if (data_.max < data_.min)
            data_.max = data_.min;
        this->policy_updated();
    }

    /// Set the type to Maximum with size hint of \p hint.
    void maximum(std::size_t hint)
    {
        data_.type = Type::Maximum;
        data_.hint = hint;
        data_.max  = hint;  // TODO this->set_max() which does the check
        if (data_.min > data_.max)
            data_.min = data_.max;
        this->policy_updated();
    }

    /// Set the type to Preferred with size hint of \p hint.
    void preferred(std::size_t hint)
    {
        data_.type = Type::Preferred;
        data_.hint = hint;
        if (data_.hint < data_.min)
            data_.min = data_.hint;
        if (data_.hint > data_.max)
            data_.max = data_.hint;
        this->policy_updated();
    }

    /// Set the type to Expanding with size hint of \p hint.
    void expanding(std::size_t hint)
    {
        data_.type    = Type::Expanding;
        data_.stretch = std::numeric_limits<decltype(data_.stretch)>::max();
        data_.hint    = hint;
        if (data_.hint < data_.min)
            data_.min = data_.hint;
        if (data_.hint > data_.max)
            data_.max = data_.hint;
        this->policy_updated();
    }

    /// Set the type to MinimumExpanding with size hint of \p hint.
    // TODO remove this or make it underscore separated..
    void minimumExpanding(std::size_t hint)
    {
        data_.type    = Type::MinimumExpanding;
        data_.stretch = std::numeric_limits<decltype(data_.stretch)>::max();
        data_.hint    = hint;
        data_.min     = hint;  // TODO set_min_size()
        this->policy_updated();
    }

    /// Set the type to Ignored.
    void ignored()
    {
        data_.hint = 0;
        data_.min  = 0;
        data_.max  = std::numeric_limits<decltype(data_.max)>::max();
        data_.type = Type::Ignored;
        this->policy_updated();
    }

    /// Return the type of Size_policy currently being used.
    auto type() const -> Size_policy::Type { return data_.type; }

    /// Set the stretch factor.
    /** Used to fit adjacent Widgets within a length. The stretch is used to
     *  compute a percentage of length the Widget should receive by dividing it
     *  by the total stretch of all Widgets in the layout. */
    void stretch(std::size_t value)
    {
        // TODO cannot be less than zero or zero, for when type is double
        // Have to make sure it cannot be zero, used in division. I guess double
        // division by zero is not a crash. It is undefined... okay.
        if (value == 0)
            return;
        data_.stretch = value;
        this->policy_updated();
    }

    /// Return the stretch factor currently being used.
    auto stretch() const -> std::size_t { return data_.stretch; }

    /// Set the size hint, used in accordance to the Type enum.
    void hint(std::size_t value)
    {
        data_.hint = value;
        this->policy_updated();
    }

    /// Return the size hint currently being used.
    auto hint() const -> std::size_t { return data_.hint; }

    /// Set the minimum length that the owning Widget should be.
    void min_size(std::size_t value)
    {
        data_.min = value;
        if (data_.max < data_.min)
            data_.max = data_.min;
        this->policy_updated();
    }

    /// Return the minimum length currently set.
    auto min_size() const -> std::size_t { return data_.min; }

    /// Set the maximum length/height that the owning Widget can be.
    void max_size(std::size_t value)
    {
        data_.max = value;
        if (data_.min > data_.max)
            data_.min = data_.max;
        this->policy_updated();
    }

    /// Return the maximum length currently set.
    auto max_size() const -> std::size_t { return data_.max; }

   private:
    struct Data {
        Size_policy::Type type = Type::Ignored;
        std::size_t stretch    = 1; // TODO should be double, calculations need it
        std::size_t hint       = 0;
        std::size_t min        = 0;
        std::size_t max        = std::numeric_limits<std::size_t>::max();
    } data_;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_SIZE_POLICY_HPP
