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
    // Does not copy/replace the policy_updated Signal
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
        data_.type = Type::Fixed;
        data_.hint = hint;
        this->policy_updated();
    }

    /// Set the type to Minimum with size hint of \p hint.
    void minimum(std::size_t hint)
    {
        data_.type = Type::Minimum;
        data_.hint = hint;
        this->policy_updated();
    }

    /// Set the type to Maximum with size hint of \p hint.
    void maximum(std::size_t hint)
    {
        data_.type = Type::Maximum;
        data_.hint = hint;
        this->policy_updated();
    }

    /// Set the type to Preferred with size hint of \p hint.
    void preferred(std::size_t hint)
    {
        data_.type = Type::Preferred;
        data_.hint = hint;
        this->policy_updated();
    }

    /// Set the type to Expanding with size hint of \p hint.
    void expanding(std::size_t hint)
    {
        data_.type = Type::Expanding;
        data_.hint = hint;
        this->policy_updated();
    }

    /// Set the type to MinimumExpanding with size hint of \p hint.
    void minimumExpanding(std::size_t hint)
    {
        data_.type = Type::MinimumExpanding;
        data_.hint = hint;
        this->policy_updated();
    }

    /// Set the Type of the Size_policy.
    void type(Size_policy::Type type)
    {
        data_.type = type;
        this->policy_updated();
    }

    /// Return the type of Size_policy currently being used.
    auto type() const -> Size_policy::Type { return data_.type; }

    /// Set the type to Ignored.
    void ignored() { this->type(Type::Ignored); }

    /// Set the stretch factor.
    /** Used to fit adjacent Widgets within a length. The stretch is used to
     *  compute a percentage of length the Widget should receive by dividing it
     *  by the total stretch of all Widgets in the layout. */
    void stretch(std::size_t value)
    {
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
        this->policy_updated();
    }

    /// Return the minimum length currently set.
    auto min_size() const -> std::size_t { return data_.min; }

    /// Set the maximum length/height that the owning Widget can be.
    void max_size(std::size_t value)
    {
        data_.max = value;
        this->policy_updated();
    }

    /// Return the maximum length currently set.
    auto max_size() const -> std::size_t { return data_.max; }

   private:
    struct Data {
        Size_policy::Type type = Type::Ignored;
        std::size_t stretch    = 1;
        std::size_t hint       = 0;
        std::size_t min        = 0;
        std::size_t max        = std::numeric_limits<std::size_t>::max();
    } data_;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_SIZE_POLICY_HPP
