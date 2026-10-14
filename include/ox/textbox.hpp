#pragma once

#include <span>
#include <utility>
#include <vector>

#include <ox/align.hpp>
#include <ox/core/core.hpp>
#include <ox/widget.hpp>

namespace ox {
class ScrollBar;

/**
 * A box of text.
 * @details Set FocusPolicy::None to disable editing.
 */
class TextBox : public Widget {
   public:
    using Widget::handle;

    struct Options {
        std::string text = {};  // UTF8 encoded
        TextWrap wrap = TextWrap::Word;
        Align align = Align::Left;
        Style style = {};
        StylePatch selection_style = {.background = {},
                                      .foreground = {},
                                      .enabled = Trait::Inverse,
                                      .disabled = {}};
        FocusPolicy focus_policy = FocusPolicy::None;
        SizePolicy size_policy = SizePolicy::flex();
    } static const init;

   public:
    TextEdit editor;
    auto text() const -> std::string const& { return editor.text(); }
    void set_text(std::string_view text);
    TextWrap wrap;
    Align align;
    Style style;
    StylePatch selection_style;

    /**
     * Emitted when the top display row or row count changes. For ScrollBar.
     * void(int index, int size)
     */
    sl::Signal<void(int, int)> on_scroll;

    TextBox(Options x = init);

   public:
    /**
     * Set the line number for the top display line.
     */
    auto set_scroll_offset(int position) -> bool;

    /**
     * Return the number of lines that can be scrolled to.
     */
    [[nodiscard]]
    auto get_scroll_length() const -> int;

   public:
    auto paint(Canvas c) -> Cursor override;

    auto handle(KeyEvent const& k) -> EventResponse override;

    auto handle(esc::PasteChunk const& chunk) -> EventResponse override;

    auto handle(MousePress const& m) -> EventResponse override;

    auto handle(MouseWheel const& m) -> EventResponse override;

   private:
    mutable TextLayout layout_;
    Point scroll_ = {};
    int notified_offset_ = -1;
    int notified_length_ = -1;

    void update_layout() const;
    void notify_scroll();
};

/**
 * Link a ScrollBar to a TextBox, the ScrollBar will control the TextBox, and the
 * TextBox will update the ScrollBar.
 */
void link(TextBox& tb, ScrollBar& sb);

}  // namespace ox