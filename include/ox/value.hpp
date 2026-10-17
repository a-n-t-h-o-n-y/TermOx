/** @file
 * Sliders, progress bars, and committed numeric values.
 */
#pragma once
#include <chrono>
#include <ox/checkbox.hpp>
#include <ox/lineedit.hpp>
#include <ox/timer.hpp>
#include <ox/value_range.hpp>
namespace ox {
/** Fractional block fill, clipped to the supplied canvas. Fractions clamp to [0,1]. */
void paint_meter(Canvas,
                 double fraction,
                 Style fill_style,
                 Style track_style,
                 bool vertical = false,
                 bool ascii = false);
/** Reject nonfinite or reversed bounds and nonpositive/nonfinite step with
 * std::invalid_argument.
 */
void validate(ValueRange);
/** Validate a finite value and range, then clamp without snapping to step. */
auto clamp_value(double, ValueRange) -> double;
/** Return a clamped normalized fraction; a zero-width range maps to one (complete). */
auto value_fraction(double, ValueRange) -> double;
/** Interactive clamped numeric value with keyboard stepping and captured pointer
 * dragging; on_change reports committed values immediately.
 */
class Slider : public Widget {
   public:
    using Widget::handle;
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Finite numeric bounds and positive step used to clamp and edit the value. */
        ValueRange range = {};
        /** Initial committed numeric value, clamped to range. */
        double value = 0;
        /** Draw a vertical slider when true; horizontal is the default. */
        bool vertical = false;
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Style patch for the inactive track behind the value or thumb. */
        StylePatch track_style = {};
        /** Style patch for the active or completed portion of the track. */
        StylePatch fill_style = {};
        /** Style patch for the slider thumb. */
        StylePatch thumb_style = {};
        /** Style patch applied while the control has focus. */
        StylePatch focused_style = {.background = {},
                                    .foreground = {},
                                    .enabled = Trait::Bold,
                                    .disabled = {}};
        /** Draw the active portion of the slider track. */
        bool show_fill = true;
        /** Draw the slider thumb at the committed value. */
        bool show_thumb = true;
    };
    /** Draw a vertical slider when true; horizontal is the default. */
    bool vertical;
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Style patch for the inactive track behind the value or thumb. */
    StylePatch track_style;
    /** Style patch for the active or completed portion of the track. */
    StylePatch fill_style;
    /** Style patch for the slider thumb. */
    StylePatch thumb_style;
    /** Style patch applied while the control has focus. */
    StylePatch focused_style;
    /** Draw the active portion of the slider track. */
    bool show_fill;
    /** Draw the slider thumb at the committed value. */
    bool show_thumb;
    /** Emits the new committed value only on change; presentation frames do not delay
     * the model.
     */
    sl::Signal<void(double)> on_change;
    /** Create a horizontal slider at zero in the default 0..100 range. */
    Slider();
    /** Validate and own the initial range/value and appearance; values clamp without
     * step quantization.
     */
    explicit Slider(Options);
    /** Transfer widget identity, retained state, and signal subscriptions; tracked
     * references follow the move.
     */
    Slider(Slider&&);
    /** Replace this widget's identity and retained state with the source; previous
     * lifetime views of this identity expire.
     */
    auto operator=(Slider&&) -> Slider&;
    /** Return the inclusive committed range and keyboard increment. */
    auto range() const -> ValueRange { return range_; }
    /** Return the committed numeric value; presentation animation does not delay it. */
    auto value() const -> double { return value_; }
    /** Validate and clamp a finite value; request repaint and emit on_change only on
     * value change.
     */
    auto set_value(double) -> bool;
    /** Validate and replace the range, clamp the current value, and repaint; notify
     * on_change only if the value changes.
     */
    auto set_range(ValueRange) -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
    /** Step with Right/Up or Left/Down; PageUp/PageDown use ten steps.
     * Home/End select the range endpoints. Releases and modified keys are ignored.
     */
    auto handle(KeyEvent const&) -> EventResponse override;
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseMove const&) -> EventResponse override;
    auto handle(MouseRelease const&) -> EventResponse override;
    auto pointer_cancel() -> EventResponse override;
    auto focus_out() -> EventResponse override { return pointer_cancel(); }

   private:
    ValueRange range_;
    double value_;
    bool dragging_ = false;
    auto point_value(Point) const -> double;
};
/** Read-only committed numeric progress with optional animated presentation and
 * percentage text.
 */
class ProgressBar : public Widget {
   public:
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Finite numeric bounds and positive step used to clamp and edit the value. */
        ValueRange range = {};
        /** Initial committed numeric value, clamped to range. */
        double value = 0;
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Overlay the normalized integer percentage on the progress fill. */
        bool show_percentage = true;
        /** Style patch for the inactive track behind the value or thumb. */
        StylePatch track_style = {};
        /** Style patch for the active or completed portion of the track. */
        StylePatch fill_style = {};
        /** Style patch applied to the progress percentage label. */
        StylePatch label_style = {};
    };
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Overlay the normalized integer percentage on the progress fill. */
    bool show_percentage;
    /** Style patch for the inactive track behind the value or thumb. */
    StylePatch track_style;
    /** Style patch for the active or completed portion of the track. */
    StylePatch fill_style;
    /** Style patch applied to the progress percentage label. */
    StylePatch label_style;
    /** Emits the new committed value only on change; presentation frames do not delay
     * the model.
     */
    sl::Signal<void(double)> on_change;
    /** Create zero progress in the default 0..100 range. */
    ProgressBar();
    /** Validate and own the initial range, value, and fill/label presentation. */
    explicit ProgressBar(Options);
    /** Return the inclusive committed range and keyboard increment. */
    auto range() const -> ValueRange { return range_; }
    /** Return the committed numeric value; presentation animation does not delay it. */
    auto value() const -> double { return value_; }
    /** Validate and clamp a finite value; request repaint and emit on_change only on
     * value change.
     */
    auto set_value(double) -> bool;
    /** Validate and replace the range, clamp the current value, and repaint; notify
     * on_change only if the value changes.
     */
    auto set_range(ValueRange) -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;

   private:
    ValueRange range_;
    double value_;
    Animation display_{*this};
};
/** Animated status indicator. No timer runs while stopped; timers follow widget moves.
 */
class Spinner : public Widget {
   public:
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Nonempty sequence of printable single-line spinner frames. */
        std::vector<std::string> frames = {"⠋", "⠙", "⠹", "⠸", "⠼",
                                           "⠴", "⠦", "⠧", "⠇", "⠏"};
        /** Positive steady-clock interval between spinner frame advances. */
        std::chrono::milliseconds interval{80};
        /** Initial spinner-running state; motion policy also controls whether its timer
         * is scheduled.
         */
        bool running = true;
        /** Base drawing style; explicit colors take precedence over theme defaults. */
        Style style = {};
        /** Style patch for running spinner presentation. */
        StylePatch running_style = {};
        /** Style patch for stopped spinner presentation. */
        StylePatch stopped_style = {};
    };
    /** Base drawing style; explicit colors take precedence over theme defaults. */
    Style style;
    /** Style patch for running spinner presentation. */
    StylePatch running_style;
    /** Style patch for stopped spinner presentation. */
    StylePatch stopped_style;
    /** Emits the requested running state only when it changes; individual frames emit no
     * model change.
     */
    sl::Signal<void(bool)> on_change;
    /** Create a running spinner using the default frame sequence and 80 ms interval. */
    Spinner();
    /** Validate nonempty printable frames and positive interval; retain a widget timer
     * controlled by running and motion policy.
     */
    explicit Spinner(Options);
    /** Return the requested running state, independent of whether motion policy
     * currently schedules its timer.
     */
    auto is_running() const -> bool;
    /** Change the running request and timer scheduling; repaint and emit on_change only
     * when the flag changes.
     */
    auto set_running(bool) -> bool;
    /** Validate a nonempty frame sequence, reset to its first frame, and request
     * layout/repaint.
     */
    void set_frames(std::vector<std::string>);
    /** Borrow spinner frames until set_frames() or destruction. */
    auto frames() const -> std::span<std::string const> { return frames_; }
    /** Advance one frame while running and return repaint effects; no model change
     * signal is emitted per frame.
     */
    auto tick() -> EventResponse;
    void theme_changed() override;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;

   private:
    bool running_ = true;
    std::vector<std::string> frames_;
    std::size_t frame_ = 0;
    Timer timer_;
    TextLayout layout_;
};
/** Same boolean contract and activation bindings as CheckBox, with switch presentation.
 */
class Switch : public CheckBox {
   public:
    /** Create a default off-state switch with CheckBox's input and change-notification
     * contract.
     */
    Switch();
    /** Construct from checkbox options and replace default indicator glyphs with switch
     * presentation.
     */
    explicit Switch(CheckBox::Options);
};
}  // namespace ox

#include <ox/numeric_edit.hpp>
