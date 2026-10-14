#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <functional>
#include <string>
#include <variant>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>
#include <ox/label.hpp>
#include <ox/timer.hpp>
#include <ox/widget.hpp>

namespace ox {

/// Emits signals on left mouse button press and release and enter key press.
class BasicButton : public Widget {
   public:
    using Widget::handle;

    sl::Signal<void()> on_press;
    sl::Signal<void()> on_release;

   public:
    BasicButton(FocusPolicy fp = FocusPolicy::Strong,
                SizePolicy sp = SizePolicy::flex());

   public:
    auto handle(KeyEvent const&) -> EventResponse override;

    auto handle(MousePress const&) -> EventResponse override;

    auto handle(MouseRelease const&) -> EventResponse override;
};

/// Decoration that fades in and out with mouse hover.
struct Fade {
    /// (Canvas, percent) where percent is fade amount from [0, 1].
    std::function<void(Canvas, float)> paint_fn;
    std::chrono::milliseconds fade_in = std::chrono::milliseconds{200};
    std::chrono::milliseconds fade_out = std::chrono::milliseconds{400};
};

/// BasicButton with a Label and optional Decoration.
class Button : public BasicButton {
   public:
    using Widget::handle;

    using PaintFn = std::function<void(Canvas)>;
    using Decoration = std::variant<PaintFn, Fade>;

    struct Options {
        Label::Options label = {};
        Decoration decoration = [](Canvas) {};
        std::function<void(Label&)> press_mod = [](Label&) {};
        std::function<void(Label&)> focus_mod = [](Label&) {};
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

   public:
    Label::Options label;
    std::function<void(Label&)> press_mod;
    std::function<void(Label&)> focus_mod;

   public:
    Button(Options x = init);

   public:
    auto paint(Canvas) -> Cursor override;

    auto handle(MousePress const&) -> EventResponse override;

    auto handle(MouseRelease const&) -> EventResponse override;

    auto focus_in() -> EventResponse override;

    auto focus_out() -> EventResponse override;

    auto mouse_enter() -> EventResponse override;

    auto mouse_leave() -> EventResponse override;

    auto tick() -> EventResponse;

   private:
    static constexpr auto timer_period_ms = 10;

    struct FadeInternal {
        Fade fade;
        int direction = +1;  // +1 or -1
        float percent = 0.f;
        Timer timer;
    };
    using DecorationInternal = std::variant<PaintFn, FadeInternal>;

    DecorationInternal decoration_;
    bool pressed_ = false;
    bool in_focus_ = false;

   private:
    /// Start the fade in animation, if applicable.
    void start_select();

    /// Start the fade out animation, if applicable.
    void end_select();
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