#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <string>

#include <signals_light/signal.hpp>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

// TODO Implement Align::Right and throw on Align::Center

class LineEdit : public Widget {
   public:
    struct Options {
        std::string text = "";
        Style text_style = {};
        Align align = Align::Left;
        Color background = default_color;
        std::string ghost_text = "";
        Style ghost_text_style = {.foreground = colors::bright_black};
        std::optional<std::function<bool(char32_t)>> validator = std::nullopt;
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

   public:
    std::string text;
    Style text_style;
    Align align;
    Color background;
    std::string ghost_text;  // Visible if text is empty.
    Style ghost_text_style;
    std::optional<std::function<bool(char32_t)>> validator;

    sl::Signal<void(std::string const&)> on_enter;

    LineEdit(Options x = init);

   public:
    auto key_press(KeyEvent k) -> EventResponse override;

    auto paste(esc::PasteChunk const& chunk) -> EventResponse override;

    auto mouse_press(MousePress m) -> EventResponse override;

    void paint(Canvas c) override;

   private:
    void update_scroll_offset();

   private:
    std::string paste_buffer_;
    std::size_t cursor_index_ = 0;  // valid range: [0, number of Unicode scalars]
    std::size_t anchor_index_ = 0;  // valid range: [0, number of Unicode scalars]
};

}  // namespace ox