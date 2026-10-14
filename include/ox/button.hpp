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
    std::chrono::milliseconds fade_in = std::chrono::milliseconds{200};
    std::chrono::milliseconds fade_out = std::chrono::milliseconds{400};
};

/// Activation, label, and decoration composed without another widget.
class Button : public Widget {
   public:
    using Widget::handle;

    using PaintFn = std::function<void(Canvas)>;
    using Decoration = std::variant<PaintFn, Fade>;

    struct Options {
        Label::Options label = {};
        Decoration decoration = [](Canvas) {};
        StylePatch pressed_style = {};
        StylePatch focused_style = {};
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

   public:
    Label::Options label;
    StylePatch pressed_style;
    StylePatch focused_style;
    Decoration decoration;
    bool pressed = false;
    bool hovered = false;
    float fade_percent = 0.f;
    sl::Signal<void()> on_press;
    sl::Signal<void()> on_release;

   public:
    Button(Options x = init);

   public:
    auto paint(Canvas) -> Cursor override;

    auto handle(KeyEvent const&) -> EventResponse override;

    auto handle(MousePress const&) -> EventResponse override;

    auto handle(MouseRelease const&) -> EventResponse override;

    auto focus_in() -> EventResponse override;

    auto focus_out() -> EventResponse override;

    auto mouse_enter() -> EventResponse override;

    auto mouse_leave() -> EventResponse override;

    auto tick() -> EventResponse;

   private:
    static constexpr auto timer_period_ms = 10;
    std::optional<Timer> timer_;
    void animate();
};

/**
 * Blend two RGB colors together.
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