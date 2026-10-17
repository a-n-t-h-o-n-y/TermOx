/** @file
 * Boolean checkbox state and change notifications.
 */
#pragma once

#include <vector>

#include <signals_light/signal.hpp>

#include <ox/label.hpp>
#include <ox/widget.hpp>

namespace ox {

/** Boolean choice toggled by an unmodified Enter/Space press or left mouse press. State
 * commands emit on_change only when changed.
 */
class CheckBox : public Widget {
   public:
    using Widget::handle;
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Initial boolean state; constructors do not emit change notifications. */
        bool checked = false;
        /** Text and style displayed beside the state indicator. */
        Label::Options label = {};
        /** Glyph sequence used to draw the checked indicator in plain styling. */
        std::vector<Glyph> checked_glyphs = U"[X]"_gs;
        /** Glyph sequence used to draw the unchecked indicator in plain styling. */
        std::vector<Glyph> unchecked_glyphs = U"[ ]"_gs;
        /** Style patch applied while the control has focus. */
        StylePatch focused_style = {.background = {},
                                    .foreground = {},
                                    .enabled = Trait::Bold,
                                    .disabled = {}};
        /** Style patch for the control's selection or state indicator. */
        StylePatch indicator_style = {};
        /** Style patch for checked-state presentation. */
        StylePatch checked_style = {};
        /** Style patch for unchecked-state presentation. */
        StylePatch unchecked_style = {};
        /** Keyboard- and pointer-focus eligibility for this control. */
        FocusPolicy focus_policy = FocusPolicy::Strong;
        /** Horizontal size bounds and growth weight. */
        SizePolicy width_policy = SizePolicy::flex();
        /** Vertical size bounds and growth weight. */
        SizePolicy height_policy = SizePolicy::fixed(1);
    } static const init; ///< Default options used when no explicit configuration is supplied.

    /** Text and style displayed beside the state indicator. */
    Label::Options label;
    /** Return the committed boolean value. */
    auto checked() const -> bool { return checked_; }
    /** Glyph sequence used to draw the checked indicator in plain styling. */
    std::vector<Glyph> checked_glyphs;
    /** Glyph sequence used to draw the unchecked indicator in plain styling. */
    std::vector<Glyph> unchecked_glyphs;
    /** Style patch applied while the control has focus. */
    StylePatch focused_style;
    /** Style patch for the control's selection or state indicator. */
    StylePatch indicator_style;
    /** Style patch for checked-state presentation. */
    StylePatch checked_style;
    /** Style patch for unchecked-state presentation. */
    StylePatch unchecked_style;
    /** Emits the new boolean state only when set_checked() or toggle() changes it. */
    sl::Signal<void(bool)> on_change;

    /** Create a checkbox with the supplied initial state and label; do not emit
     * on_change during construction.
     */
    CheckBox(Options x = init);
    /** Commands notify only on change and request repaint when attached. */
    auto set_checked(bool value) -> bool;
    /** Invert the committed value through set_checked(), emitting the new value and
     * requesting repaint.
     */
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
