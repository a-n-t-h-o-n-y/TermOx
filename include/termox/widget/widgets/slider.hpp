#ifndef TERMOX_WIDGET_WIDGETS_SLIDER_HPP
#define TERMOX_WIDGET_WIDGETS_SLIDER_HPP
#include <memory>

#include <signals_light/signal.hpp>

#include <termox/painter/painter.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

/// Fixed range sliding display fixed at one end of the Widget.
/** Range can contain negative values. */
template <template <typename> typename Layout_t, bool Inverted>
class Slider : public Widget {
   public:
    /// [low, high] inclusive range of values.
    struct Range {
        int low;
        int high;
    };

    struct Parameters {
        Range range;
        int initial;
    };

   public:
    /// Emitted when the [low, high] range is modified.
    sl::Signal<void(Range)> range_changed;

    /// Emitted on user input and passes along the new current value set.
    sl::Signal<void(int)> value_changed;

   public:
    /// Create new Slider.
    /** Throws std::invalid_argument if \p range's low is greater than its high.
     *  Throws std::out_of_range if \p initial is outside of \p range. */
    Slider(Range range, int initial);

    /// Create new Slider.
    /** Throws std::invalid_argument if \p range's low is greater than its high.
     *  Throws std::out_of_range if \p initial is outside of \p range. */
    Slider(Parameters p);

   public:
    /// Set a new Range for this Slider.
    /** Throws std::invalid_argument if \p range's low is greater than its high.
     *  The current value is clamped to the new range, if now outside of it. */
    void set_range(Range x);

    /// Return the currently set Range. Emits range_changed Signal.
    [[nodiscard]] auto range() const noexcept -> Range;

    /// Set the current value of the Slider. Emits value_changed Signal.
    /** Throws std::out_of_range if outside of the currently set Range. */
    void set_value(int x);

    /// Return the currently set value.
    auto value() const noexcept -> int;

   protected:
    auto mouse_press_event(Mouse const& m) -> bool override;

    auto mouse_move_event(Mouse const& m) -> bool override;

    auto mouse_wheel_event(Mouse const& m) -> bool override;

    auto key_press_event(Key k) -> bool override;

    auto paint_event(Painter& p) -> bool override;

   private:
    void increment(int amount = 1);

    void decrement(int amount = 1);

    /// Return the value that should be at the given display Point.
    [[nodiscard]] auto value_from_point(Point p) const -> int;

    /// Return some percentage of the current range for long increment.
    [[nodiscard]] auto extra() const -> int;

   private:
    static auto constexpr is_vertical = layout::is_vertical_v<Layout_t<Widget>>;

    Range range_;
    int value_;

   private:
    /// Return the number of whole values in the currently set Range.
    [[nodiscard]] static auto length(Range range) -> int;

    [[nodiscard]] static auto make_point(int length, int breadth) -> Point;

    [[nodiscard]] static auto make_area(int length_dim, int breadth_dim)
        -> Area;
};

/// Helper function to create a Slider instance.
template <template <typename> typename Layout_t, bool Inverted>
[[nodiscard]] auto slider(typename Slider<Layout_t, Inverted>::Range range,
                          int initial)
    -> std::unique_ptr<Slider<Layout_t, Inverted>>;

/// Helper function to create a Slider instance.
template <template <typename> typename Layout_t, bool Inverted>
[[nodiscard]] auto slider(
    typename Slider<Layout_t, Inverted>::Parameters parameters)
    -> std::unique_ptr<Slider<Layout_t, Inverted>>;

using Slider_top = Slider<layout::Vertical, false>;

/// Helper function to create a Slider_top instance.
[[nodiscard]] auto slider_top(Slider_top::Range range, int initial)
    -> std::unique_ptr<Slider_top>;

/// Helper function to create a Slider_top instance.
[[nodiscard]] auto slider_top(Slider_top::Parameters parameters)
    -> std::unique_ptr<Slider_top>;

using Slider_bottom = Slider<layout::Vertical, true>;

/// Helper function to create a Slider_bottom instance.
[[nodiscard]] auto slider_bottom(Slider_bottom::Range range, int initial)
    -> std::unique_ptr<Slider_bottom>;

/// Helper function to create a Slider_bottom instance.
[[nodiscard]] auto slider_bottom(Slider_bottom::Parameters parameters)
    -> std::unique_ptr<Slider_bottom>;

using Slider_left = Slider<layout::Horizontal, false>;

/// Helper function to create a Slider_left instance.
[[nodiscard]] auto slider_left(Slider_left::Range range, int initial)
    -> std::unique_ptr<Slider_left>;

/// Helper function to create a Slider_left instance.
[[nodiscard]] auto slider_left(Slider_left::Parameters parameters)
    -> std::unique_ptr<Slider_left>;

using Slider_right = Slider<layout::Horizontal, true>;

/// Helper function to create a Slider_right instance.
[[nodiscard]] auto slider_right(Slider_right::Range range, int initial)
    -> std::unique_ptr<Slider_right>;

/// Helper function to create a Slider_right instance.
[[nodiscard]] auto slider_right(Slider_right::Parameters parameters)
    -> std::unique_ptr<Slider_right>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_SLIDER_HPP
