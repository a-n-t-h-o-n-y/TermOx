/** @file
 * Pointer and keyboard buttons with click and hover feedback.
 */
#pragma once

#include <algorithm>
#include <chrono>
#include <functional>
#include <optional>
#include <string>
#include <variant>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>
#include <ox/label.hpp>
#include <ox/timer.hpp>
#include <ox/widget.hpp>

namespace ox {

/// Decoration that fades in and out with mouse hover or keyboard focus.
struct Fade {
    /// (Canvas, percent) where percent is fade amount from [0, 1].
    std::function<void(Canvas, float)> paint_fn;
    /** Nonnegative duration of decoration fade-in. */
    std::chrono::milliseconds fade_in = std::chrono::milliseconds{200};
    /** Nonnegative duration of decoration fade-out. */
    std::chrono::milliseconds fade_out = std::chrono::milliseconds{400};
};

/// Activation, label, and decoration composed without another widget.
class Button : public Widget {
   public:
    using Widget::handle;

    /** Decoration callback borrowing the current paint canvas; it must not retain the
     * canvas.
     */
    using PaintFn = std::function<void(Canvas)>;
    /** Either an immediate paint callback or hover/focus fade presentation. */
    using Decoration = std::variant<PaintFn, Fade>;

    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Label text and style painted by the button; call refresh() after changing
         * text or layout properties.
         */
        Label::Options label = {};
        /** Optional paint callback or fade decoration drawn after the button label. */
        Decoration decoration = [](Canvas) {};
        /** Style patch while the button is armed and pressed. */
        StylePatch pressed_style = {};
        /** Style patch applied while the control has focus. */
        StylePatch focused_style = {};
        /** Style patch for pointer hover feedback. */
        StylePatch hover_style = {};
        /** Style patch for the control when it does not have focus. */
        StylePatch blurred_style = {};
        /** Draw focus endpoint marks when themed button styling is enabled. */
        bool show_focus_indicator = true;
        /** Keyboard- and pointer-focus eligibility for this control. */
        FocusPolicy focus_policy = FocusPolicy::Strong;
        /** Horizontal size bounds and growth weight. */
        SizePolicy width_policy = SizePolicy::flex();
        /** Vertical size bounds and growth weight. */
        SizePolicy height_policy = SizePolicy::fixed(1);
    } static const init; ///< Default options used when no explicit configuration is supplied.

   public:
    /** Label text and style painted by the button; call refresh() after changing text or
     * layout properties.
     */
    Label::Options label;
    /** Style patch while the button is armed and pressed. */
    StylePatch pressed_style;
    /** Style patch applied while the control has focus. */
    StylePatch focused_style;
    /** Style patch for pointer hover feedback. */
    StylePatch hover_style;
    /** Style patch for the control when it does not have focus. */
    StylePatch blurred_style;
    /** Draw focus endpoint marks when themed button styling is enabled. */
    bool show_focus_indicator;
    /** Optional paint callback or fade decoration drawn after the button label. */
    Decoration decoration;
    /** Return whether an armed pointer gesture is currently inside the button. */
    auto is_pressed() const -> bool { return pressed_; }
    /** Return whether the pointer lies within the button's hover ancestry. */
    auto is_hovered() const -> bool { return hovered_; }
    /** Emits for activate(), an activation key press, or a matching pointer release
     * inside the button.
     */
    sl::Signal<void()> on_activate;
    /** Current presentation feedback amount, from zero to one. */
    float fade_percent = 0.f;
    /** Emits when a left pointer press arms the button and captures the drag. */
    sl::Signal<void()> on_press;
    /** Emits for a matching armed pointer release, even when released outside;
     * on_activate follows only for an inside release.
     */
    sl::Signal<void()> on_release;

   public:
    /** Create a button from label, decoration, and appearance options; defaults to one
     * row and Strong focus.
     */
    Button(Options x = init);
    /** Transfer widget identity, retained state, and signal subscriptions; tracked
     * references follow the move.
     */
    Button(Button&& other);
    /** Replace this widget's identity and retained state with the source; previous
     * lifetime views of this identity expire.
     */
    auto operator=(Button&& other) -> Button&;

   public:
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    /** Emit on_activate and request repaint when effectively enabled; return false for a
     * disabled button.
     */
    auto activate() -> bool;
    auto paint(Canvas) -> Cursor override;

    /** Handle an unmodified Enter/Space press through activate(); releases and unrelated
     * keys are ignored.
     */
    auto handle(KeyEvent const&) -> EventResponse override;

    auto handle(MousePress const&) -> EventResponse override;

    auto handle(MouseRelease const&) -> EventResponse override;

    auto handle(MouseMove const&) -> EventResponse override;
    auto pointer_cancel() -> EventResponse override;

    auto focus_in() -> EventResponse override;

    auto focus_out() -> EventResponse override;

    auto mouse_enter() -> EventResponse override;

    auto mouse_leave() -> EventResponse override;

    /** Advance retained hover/focus feedback by one frame and return its repaint effects. */
    auto tick() -> EventResponse;

   private:
    bool pressed_ = false;
    bool hovered_ = false;
    bool armed_ = false;
    std::size_t gesture_ = 0;
    mutable TextLayout measure_layout_;
    Animation feedback_{*this};
    void animate();
};

/**
 * Blend two RGB colors together.
 * @param one The color returned at zero percent.
 * @param two The color returned at one hundred percent.
 * @param percent [0, 1] blending amount. 0 is all one, 1 is all two.
 */
[[nodiscard]] auto gradient_blend(RGB one, RGB two, float percent) -> RGB;

/**
 * Builds a void(Canvas, float) function from a Shape object and gradient.
 * @details Shape must be able to be used with put(Canvas, shape, fg). Useful for Fade
 * decorations.
 */
[[nodiscard]] auto shape_gradient(auto const& shape, RGB one, RGB two)
    -> std::function<void(Canvas, float)>
{
    return [=](Canvas c, float percent) {
        auto const fg = gradient_blend(one, two, percent);
        put(c, shape, fg);
    };
}

}  // namespace ox
