#ifndef CPPURSES_WIDGET_SIZE_POLICY_HPP
#define CPPURSES_WIDGET_SIZE_POLICY_HPP
#include <cstddef>
#include <limits>

namespace cppurses {
class Widget;

/// Defines how a Layout should resize a Widget in one length Dimension.
class Size_policy {
   public:
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

    /// Constructs a size policy with \p owner.
    /** \p owner is needed to notify its parent whenever a value has been
     *  changed, this is done by posting a Child_polished_event. */
    explicit Size_policy(Widget* owner);

    /// Set the Type of the Size_policy.
    void type(Size_policy::Type type);

    /// Retrieve the type of Size_policy currently being used.
    Size_policy::Type type() const;

    /// Sets the stretch factor.
    /** Used to fit adjacent Widgets within a length. The stretch is used to
     *  compute a percentage of length the Widget should receive by dividing it
     *  by the total stretch of all Widgets in the layout. */
    void stretch(std::size_t value);

    /// Retrieve the stretch factor currently being used.
    std::size_t stretch() const;

    /// Set the size hint, used in accordance to the Type enum.
    void hint(std::size_t value);

    /// Retrieve the size hint currently being used.
    std::size_t hint() const;

    /// Set the minimum length that the owning Widget should be.
    void min(std::size_t value);

    /// Retrieve the minimum length currently set.
    std::size_t min() const;

    /// Set the maximum length that the owning Widget should be.
    void max(std::size_t value);

    /// Retrieve the maximum length currently set.
    std::size_t max() const;

   private:
    Size_policy::Type type_{Type::Ignored};
    std::size_t stretch_{1};
    std::size_t hint_{0};
    std::size_t min_{0};
    std::size_t max_{std::numeric_limits<std::size_t>::max()};
    Widget* owner_;

    /// Posts a Child_polished_event to the parent of owner_.
    void notify_parent() const;
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_SIZE_POLICY_HPP
