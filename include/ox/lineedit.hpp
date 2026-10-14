#pragma once

#include <cstddef>
#include <functional>
#include <string>

#include <signals_light/signal.hpp>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {

class LineEdit : public Widget {
   public:
    using Widget::handle;

    struct Options {
        std::string text = "";
        Style text_style = {};
        Align align = Align::Left;
        StylePatch selection_style = {.background = {},
                                      .foreground = {},
                                      .enabled = Trait::Inverse,
                                      .disabled = {}};
        Color background = default_color;
        std::string ghost_text = "";
        Style ghost_text_style = {.foreground = colors::bright_black};
        std::function<bool(char32_t)> validator = {};
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

   public:
    TextEdit editor;
    auto text() const -> std::string const& { return editor.text(); }
    void set_text(std::string_view text);
    Style text_style;
    Align align;
    StylePatch selection_style;
    Color background;
    std::string ghost_text;  // Visible if text is empty.
    Style ghost_text_style;
    std::function<bool(char32_t)> validator;

    sl::Signal<void(std::string const&)> on_enter;

    LineEdit(Options x = init);

   public:
    auto handle(KeyEvent const& k) -> EventResponse override;

    auto handle(esc::PasteChunk const& chunk) -> EventResponse override;

    auto handle(MousePress const& m) -> EventResponse override;

    auto paint(Canvas c) -> Cursor override;

   private:
    TextLayout layout_;
    TextLayout ghost_layout_;
    Point scroll_ = {};

    void update_layout();
};

}  // namespace ox
