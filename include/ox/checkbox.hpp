#pragma once

#include <vector>

#include <signals_light/signal.hpp>

#include <ox/widget.hpp>

namespace ox {

class CheckBox : public Widget {
   public:
    using Widget::handle;
    struct Options {
        bool checked = false;
        std::vector<Glyph> checked_glyphs = U"[X]"_gs;
        std::vector<Glyph> unchecked_glyphs = U"[ ]"_gs;
        StylePatch focused_style = {.background = {},
                                    .foreground = {},
                                    .enabled = Trait::Bold,
                                    .disabled = {}};
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy size_policy = SizePolicy::fixed(4);
    } static const init;

    bool checked;
    std::vector<Glyph> checked_glyphs;
    std::vector<Glyph> unchecked_glyphs;
    StylePatch focused_style;
    sl::Signal<void(bool)> on_toggle;

    CheckBox(Options x = init);
    /** Commands notify only on change and request repaint when attached. */
    auto set_checked(bool value) -> bool;
    auto toggle() -> bool;
    auto handle(KeyEvent const& event) -> EventResponse override;
    auto handle(MousePress const& event) -> EventResponse override;
    auto paint(Canvas canvas) -> Cursor override;
};
}  // namespace ox
