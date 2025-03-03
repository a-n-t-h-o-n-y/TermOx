#include <ox/button.hpp>

#include <algorithm>
#include <functional>
#include <utility>

namespace ox {

Button::Options const Button::init = {};

Button::Button(Options x)
    : Widget{FocusPolicy::Strong, SizePolicy::flex()},
      label{std::move(x.label)},
      style{std::move(x.style)}
{}

auto Button::paint(Canvas c) -> void
{
    using LabelFn = std::function<void(Label&)>;
    using LabelAnimationFn = std::function<void(Label&, float)>;
    using CanvasFn = std::function<void(Canvas&)>;
    using CanvasAnimationFn = std::function<void(Canvas&, float)>;

    auto local_label = Label{label};

    if (in_focus_ && std::holds_alternative<LabelFn>(style.in_focus)) {
        auto& fn = std::get<LabelFn>(style.in_focus);
        if (fn) {
            fn(local_label);
        }
    }

    if (hovered_ && std::holds_alternative<LabelFn>(style.hover)) {
        auto& fn = std::get<LabelFn>(style.hover);
        if (fn) {
            fn(local_label);
        }
    }
    else if (percent_hovered_ != 0.f &&
             std::holds_alternative<LabelAnimationFn>(style.hover)) {
        auto& fn = std::get<LabelAnimationFn>(style.hover);
        if (fn) {
            fn(local_label, percent_hovered_);
        }
    }

    if (pressed_ && std::holds_alternative<LabelFn>(style.pressed)) {
        auto& fn = std::get<LabelFn>(style.pressed);
        if (fn) {
            fn(local_label);
        }
    }

    local_label.paint(c);

    if (style.decoration) {
        style.decoration(c);
    }

    // Modify Canvas depending on state.
    if (in_focus_ && std::holds_alternative<CanvasFn>(style.in_focus)) {
        auto& fn = std::get<CanvasFn>(style.in_focus);
        if (fn) {
            fn(c);
        }
    }

    if (hovered_ && std::holds_alternative<CanvasFn>(style.hover)) {
        auto& fn = std::get<CanvasFn>(style.hover);
        if (fn) {
            fn(c);
        }
    }
    else if (percent_hovered_ != 0.f &&
             std::holds_alternative<CanvasAnimationFn>(style.hover)) {
        auto& fn = std::get<CanvasAnimationFn>(style.hover);
        if (fn) {
            fn(c, percent_hovered_);
        }
    }

    if (pressed_ && std::holds_alternative<CanvasFn>(style.pressed)) {
        auto& fn = std::get<CanvasFn>(style.pressed);
        if (fn) {
            fn(c);
        }
    }
}

auto Button::mouse_press(Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) {
        pressed_ = true;
        on_press();
    }
}

auto Button::mouse_release(Mouse m) -> void
{
    if (m.button == Mouse::Button::Left) {
        pressed_ = false;
        on_release();
    }
}

auto Button::mouse_enter() -> void
{
    using CanvasAnimationFn = std::function<void(Canvas&, float)>;

    // TODO these two can be represented with a single item. maybe bool and use a
    // ternary op to determine if +1 or -1.
    hovered_ = true;
    hover_direction_ = 1;

    // TODO fix Timer and TimerThread so the thread is reused.
    if (not timer_.is_running() &&
        std::holds_alternative<CanvasAnimationFn>(style.hover)) {
        timer_.start();
    }
}

auto Button::mouse_leave() -> void
{
    hovered_ = false;
    hover_direction_ = -1;
}

auto Button::focus_in() -> void { in_focus_ = true; }

auto Button::focus_out() -> void { in_focus_ = false; }

auto Button::timer(int id) -> void
{
    if (id == timer_.id()) {
        percent_hovered_ =
            std::clamp(0.f, percent_hovered_ + 0.1f * (float)hover_direction_, 1.f);
        if (percent_hovered_ == 0.f) {
            timer_.stop();
        }
    }
}

// TODO move to Painter?
auto half_frame_1(Color fg) -> std::function<void(Canvas&)>
{
    return [fg](Canvas& c) {
        // line across bottom
        for (auto x = 1; x < c.size.width; ++x) {
            auto& g = c[{
                .x = x,
                .y = std::max(c.size.height - 1, 0),
            }];
            g.symbol = U'─';
            g.brush.foreground = fg;
        }

        // line on right side
        for (auto y = 1; y < c.size.height; ++y) {
            auto& g = c[{
                .x = std::max(c.size.width - 1, 0),
                .y = y,
            }];
            g.symbol = U'│';
            g.brush.foreground = fg;
        }

        auto& bottom_right = c[{
            .x = std::max(c.size.width - 1, 0),
            .y = std::max(c.size.height - 1, 0),
        }];
        bottom_right.symbol = U'╯';
        bottom_right.brush.foreground = fg;
    };
}

void four_corners_(Canvas& c, Color fg)
{
    auto& top_left = c[{.x = 0, .y = 0}];
    top_left.symbol = U'┌';
    top_left.brush.foreground = fg;

    auto& top_right = c[{
        .x = std::max(c.size.width - 1, 0),
        .y = 0,
    }];
    top_right.symbol = U'┐';
    top_right.brush.foreground = fg;

    auto& bottom_left = c[{
        .x = 0,
        .y = std::max(c.size.height - 1, 0),
    }];
    bottom_left.symbol = U'└';
    bottom_left.brush.foreground = fg;

    auto& bottom_right = c[{
        .x = std::max(c.size.width - 1, 0),
        .y = std::max(c.size.height - 1, 0),
    }];
    bottom_right.symbol = U'┘';
    bottom_right.brush.foreground = fg;
}

auto four_corners(Color fg) -> std::function<void(Canvas&)>
{
    return [fg](Canvas& c) { four_corners_(c, fg); };
}

}  // namespace ox