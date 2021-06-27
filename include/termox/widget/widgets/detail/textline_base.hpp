#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_BASE_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_BASE_HPP
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/widget.hpp>
#include <termox/widget/widgets/detail/textline_core.hpp>

namespace ox::detail {

/// Implements single line text Cursor navigation and deletion, no text input.
/** Line_edit and Number_edit inherit from this. */
class Textline_base : public ox::Widget {
   public:
    enum class Action { None, Clear };

    struct Parameters {
        ox::Glyph_string initial = U"";
        ox::Align align          = ox::Align::Left;
        Action focus_in_action   = Action::None;
    };

   public:
    Textline_base(ox::Glyph_string initial = U"",
                  ox::Align align          = ox::Align::Left,
                  Action focus_in_action   = Action::None);

    Textline_base(Parameters p);

   public:
    /// Clear and reset the text being displayed.
    void set_text(ox::Glyph_string x);

    /// Return the full text contents.
    [[nodiscard]] auto text() const noexcept -> ox::Glyph_string const&;

    /// Set either Left or Right Alignment, asserts with other alignments.
    void set_alignment(ox::Align x);

    /// Return the current alignment.
    [[nodiscard]] auto alignment() const noexcept -> ox::Align;

    /// Set the Action that occurs on focus in events.
    void set_focus_in_action(Action x);

    /// Clear the text on focus in event, restoring if nothing edited.
    void clear_on_focus();

    /// Does nothing on focus in event.
    void nothing_on_focus();

    /// Return the currently used focus in event Action.
    [[nodiscard]] auto focus_in_action() const noexcept -> Action;

    /// Set the Cursor to the Glyph in text at index \p.
    void set_cursor_to_index(int i);

   protected:
    Textline_core core_;

   protected:
    auto paint_event(ox::Painter& p) -> bool override;

    auto key_press_event(ox::Key k) -> bool override;

    auto mouse_press_event(ox::Mouse const& m) -> bool override;

    auto resize_event(ox::Area new_size, ox::Area old_size) -> bool override;

    auto focus_in_event() -> bool override;

    auto focus_out_event() -> bool override;

   private:
    Action focus_in_action_;
    Glyph_string stored_text_;  // stored on focus_in
};

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_BASE_HPP
