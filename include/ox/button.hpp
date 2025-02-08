#pragma once

#include <functional>
#include <string>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>
#include <ox/label.hpp>
#include <ox/widget.hpp>

namespace ox {

class Button : public Widget {
   public:
    Label label;

    Brush brush;
    std::function<Brush(Brush)> on_press_brush;
    std::function<Brush(Brush)> on_hover_brush;

    sl::Signal<void()> on_press;
    sl::Signal<void()> on_release;

   public:
    struct Options {
        std::string text = "";
        Brush brush = {};
        std::function<Brush(Brush)> on_press_brush = [](Brush b) { return b; };
        std::function<Brush(Brush)> on_hover_brush = [](Brush b) { return b; };
    } static const init;

    Button(Options x = init);

   public:
    // TODO enter key to emit
    // TODO on focus brush?
    auto paint(Canvas c) -> void override;

    auto mouse_press(Mouse m) -> void override;

    auto mouse_release(Mouse m) -> void override;

    auto mouse_enter() -> void override;

    auto mouse_leave() -> void override;
};

}  // namespace ox