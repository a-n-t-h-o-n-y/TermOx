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
#include <ox/widget.hpp>

namespace ox {

/// Emits signals on left mouse button press and release and enter key press.
class BasicButton : public Widget {
   public:
    sl::Signal<void()> on_press;
    sl::Signal<void()> on_release;

   public:
    BasicButton(FocusPolicy fp = FocusPolicy::Strong,
                SizePolicy sp = SizePolicy::flex());

   public:
    void key_press(Key) override;

    void mouse_press(Mouse) override;

    void mouse_release(Mouse) override;
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
    using PaintFn = std::function<void(Canvas)>;
    using Decoration = std::variant<PaintFn, Fade>;

    struct Options {
        Label::Options label = {};
        Decoration decoration = [](Canvas) {};
        std::function<void(Label&)> pressed_mod = [](Label&) {};
        std::function<void(Label&)> focused_mod = [](Label&) {};
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

   public:
    Label::Options label;
    std::function<void(Label&)> pressed_mod;
    std::function<void(Label&)> focused_mod;

   public:
    Button(Options x = init);

   public:
    void paint(Canvas) override;

    void mouse_press(Mouse) override;

    void mouse_release(Mouse) override;

    void focus_in() override;

    void focus_out() override;

    void mouse_enter() override;

    void mouse_leave() override;

    void timer(int id) override;

   private:
    static constexpr auto timer_period_ms = 30;

    struct FadeInternal {
        Fade fade;
        int direction = +1;  // +1 or -1
        float percent = 0.f;
        Timer timer = Timer{std::chrono::milliseconds{timer_period_ms}, false};
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
 * Blend two TrueColor colors together.
 * @param percent [0, 1] blending amount. 0 is all one, 1 is all two.
 */
[[nodiscard]] auto gradient_blend(TrueColor one, TrueColor two, float percent)
    -> TrueColor;

/**
 * Builds a void(Canvas, float) function from a Shape object and gradient.
 * @details Shape must be able to be used with put(...). Useful for Fade decorations.
 */
[[nodiscard]] auto shape_gradient(auto const& shape, TrueColor one, TrueColor two)
    -> std::function<void(Canvas, float)>
{
    return [=](Canvas c, float percent) {
        auto const fg = gradient_blend(one, two, percent);
        put(c, shape, fg);
    };
}

}  // namespace ox