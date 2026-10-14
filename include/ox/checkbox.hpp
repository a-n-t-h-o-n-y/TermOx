#pragma once

#include <vector>

#include <signals_light/signal.hpp>

#include <ox/label.hpp>
#include <ox/widget.hpp>

namespace ox {

class CheckBox : public Widget {
   public:
    using Widget::handle;
    struct Options {
        bool checked = false;
        Label::Options label = {};
        std::vector<Glyph> checked_glyphs = U"[X]"_gs;
        std::vector<Glyph> unchecked_glyphs = U"[ ]"_gs;
        StylePatch focused_style = {.background = {},
                                    .foreground = {},
                                    .enabled = Trait::Bold,
                                    .disabled = {}};
        FocusPolicy focus_policy = FocusPolicy::Strong;
        SizePolicy width_policy = SizePolicy::flex();
        SizePolicy height_policy = SizePolicy::fixed(1);
    } static const init;

    Label::Options label;
    auto checked() const -> bool { return checked_; }
    std::vector<Glyph> checked_glyphs;
    std::vector<Glyph> unchecked_glyphs;
    StylePatch focused_style;
    sl::Signal<void(bool)> on_change;

    CheckBox(Options x = init);
    /** Commands notify only on change and request repaint when attached. */
    auto set_checked(bool value) -> bool;
    auto toggle() -> bool;
    auto handle(KeyEvent const& event) -> EventResponse override;
    auto handle(MousePress const& event) -> EventResponse override;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas canvas) -> Cursor override;

   private:
    bool checked_;
    mutable TextLayout measure_layout_;
};
}  // namespace ox
