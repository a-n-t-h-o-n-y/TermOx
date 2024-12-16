#pragma once

#include <optional>
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
    Brush on_press_brush;
    Brush on_hover_brush;

    sl::Signal<void()> on_press;
    sl::Signal<void()> on_release;

   public:
    struct Init {
        std::string text = "";
        Brush brush = {};
        std::optional<Brush> on_press_brush = std::nullopt;
        std::optional<Brush> on_hover_brush = std::nullopt;
    } static const init;

    Button(Init state = init);

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