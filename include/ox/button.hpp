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

class Button : public Widget {
   public:
    using PaintFn = std::function<void(Canvas&)>;

    using ModifyFn =
        std::variant<std::function<void(Canvas&)>, std::function<void(Label&)>>;

    // TODO do you need a (canvas, label, float) version or does canvas provide enough
    // to handle any scenario there?
    using ModifyWithAnimationFn = std::variant<std::function<void(Canvas&, float)>,
                                               std::function<void(Label&, float)>,
                                               std::function<void(Label&)>,
                                               std::function<void(Canvas&)>>;
    struct Style {
        PaintFn decoration = [](Canvas&) {};
        ModifyFn in_focus = [](Label&) {};
        ModifyFn pressed = [](Label&) {};
        ModifyWithAnimationFn hover = [](Label&) {};
    };

    struct Options {
        Label::Options label = {};
        Style style = {};
    } static const init;

   public:
    Label::Options label;
    Style style;

    sl::Signal<void()> on_press;
    sl::Signal<void()> on_release;

    Button(Options x = init);

   public:
    // TODO enter key to emit
    auto paint(Canvas c) -> void override;

    auto mouse_press(Mouse m) -> void override;

    auto mouse_release(Mouse m) -> void override;

    auto mouse_enter() -> void override;

    auto mouse_leave() -> void override;

    auto focus_in() -> void override;

    auto focus_out() -> void override;

    auto timer(int id) -> void override;

   private:
    bool in_focus_ = false;
    bool pressed_ = false;
    bool hovered_ = false;
    int hover_direction_ = 1;  // +1 or -1
    float percent_hovered_ = 0.f;
    Timer timer_{std::chrono::milliseconds{30}, false};
};

[[nodiscard]] inline auto gradient_blend(TrueColor one, TrueColor two, float percent)
    -> TrueColor
{
    auto blend = [](std::uint8_t a, std::uint8_t b, float t) {
        return (std::uint8_t)std::clamp(std::lround(a + t * (b - a)), 0L, 255L);
    };

    return RGB{
        blend(one.red, two.red, percent),
        blend(one.green, two.green, percent),
        blend(one.blue, two.blue, percent),
    };
}

/**
 * Makes a callable that will apply the given drawing function with a gradient between
 * the two given colors.
 */
[[nodiscard]]
inline auto apply_gradient(std::function<void(Canvas&, Color)> const& drawing_fn,
                           TrueColor one,
                           TrueColor two) -> std::function<void(Canvas&, float)>
{
    return [drawing_fn, one, two](Canvas& c, float percent) {
        auto const fg = gradient_blend(one, two, percent);
        drawing_fn(c, fg);
    };
}

[[nodiscard]] auto half_frame_1(Color fg = XColor::Default)
    -> std::function<void(Canvas&)>;

void four_corners_(Canvas& c, Color fg = XColor::Default);

[[nodiscard]] auto four_corners(Color fg = XColor::Default)
    -> std::function<void(Canvas&)>;

// [[nodiscard]] auto four_corners_round(Color fg = XColor::Default)
//     -> std::function<void(Canvas&)>;

}  // namespace ox