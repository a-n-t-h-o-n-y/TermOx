#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <string>

#include <signals_light/signal.hpp>

#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

// TODO Implement Align::Right

class LineEdit : public Widget {
   public:
    sl::Signal<void(std::string const&)> on_enter;

   public:
    std::string text;
    Brush text_brush;
    enum class Align : bool { Left } align;
    Color background;
    std::string ghost_text;  // visible if text is empty
    Brush ghost_text_brush;
    std::optional<std::function<bool(char)>> validator;

   public:
    LineEdit(std::string text = {},
             Brush text_brush = {},
             Align align = Align::Left,
             Color background = XColor::Default,
             std::string ghost_text = {},
             Brush ghost_text_brush = {.foreground = XColor::BrightBlack},
             std::optional<std::function<bool(char)>> validator = std::nullopt);

   public:
    void key_press(Key k) override;

    void mouse_press(Mouse m) override;

    void paint(Canvas c) override;

   private:
    void increment_cursor(int amount);

   private:
    std::size_t cursor_index_ = 0;  // valid range: [0, text.size()]
    std::size_t anchor_index_ = 0;  // valid range: [0, text.size()]
};

}  // namespace ox