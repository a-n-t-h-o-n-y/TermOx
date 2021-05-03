#ifndef TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_BASE_HPP
#define TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_BASE_HPP
#include <utility>

#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/align.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/pipe.hpp>
#include <termox/widget/point.hpp>
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
                  Action focus_in_action   = Action::None)
        : core_{std::move(initial), 0, align}, focus_in_action_{focus_in_action}
    {
        using namespace ox::pipe;
        *this | fixed_height(1) | show_cursor() | strong_focus();
    }

    Textline_base(Parameters p)
        : Textline_base{std::move(p.initial), p.align, p.focus_in_action}
    {}

   public:
    /// Clear and reset the text being displayed.
    void set_text(ox::Glyph_string x)
    {
        core_.set_text(std::move(x));
        this->update();
    }

    /// Return the full text contents.
    [[nodiscard]] auto text() const noexcept -> ox::Glyph_string const&
    {
        return core_.text();
    }

    /// Set either Left or Right Alignment, asserts with other alignments.
    void set_alignment(ox::Align x)
    {
        core_.set_alignment(x);
        this->update();
    }

    /// Return the current alignment.
    [[nodiscard]] auto alignment() const noexcept -> ox::Align
    {
        return core_.alignment();
    }

    /// Set the Action that occurs on focus in events.
    void set_focus_in_action(Action x) { focus_in_action_ = x; }

    /// Clear the text on focus in event, restoring if nothing edited.
    void clear_on_focus() { focus_in_action_ = Action::Clear; }

    /// Does nothing on focus in event.
    void nothing_on_focus() { focus_in_action_ = Action::None; }

    /// Return the currently used focus in event Action.
    [[nodiscard]] auto focus_in_action() const noexcept -> Action
    {
        return focus_in_action_;
    }

    /// Set the Cursor to the Glyph in text at index \p.
    void set_cursor_to_index(int i)
    {
        core_.set_cursor_to_index(i);
        this->update();
    }

   protected:
    Textline_core core_;

   protected:
    auto paint_event(ox::Painter& p) -> bool override
    {
        auto const str = core_.display_substr();
        switch (core_.alignment()) {
            case ox::Align::Left:
                p.put(str, {0, 0});
                this->cursor.set_position({core_.cursor_position(), 0});
                break;
            case ox::Align::Right: {
                p.put(str, {this->width() - str.size(), 0});
                this->cursor.set_position(
                    {this->width() - str.size() + core_.cursor_position(), 0});
                break;
            }
            default: break;
        }
        return Widget::paint_event(p);
    }

    auto key_press_event(ox::Key k) -> bool override
    {
        switch (k) {
            using ox::Key;
            case Key::Arrow_left: core_.decrement_cursor(); break;
            case Key::Arrow_right: core_.increment_cursor(); break;
            case Key::Delete: core_.erase_at_cursor(); break;
            case Key::Backspace: core_.erase_before_cursor(); break;
            case Key::Home: core_.cursor_home(); break;
            case Key::End: core_.cursor_end(); break;
            default: break;
        }
        this->update();
        return Widget::key_press_event(k);
    }

    auto mouse_press_event(ox::Mouse const& m) -> bool override
    {
        core_.move_cursor_to_display(m.at.x);
        this->update();
        return Widget::mouse_press_event(m);
    }

    auto resize_event(ox::Area new_size, ox::Area old_size) -> bool override
    {
        core_.set_display_length(new_size.width);
        this->update();
        return Widget::resize_event(new_size, old_size);
    }

    auto focus_in_event() -> bool override
    {
        if (focus_in_action_ == Action::Clear) {
            stored_text_ = core_.text();
            core_.clear();
            this->update();
        }
        return Widget::focus_in_event();
    }

    auto focus_out_event() -> bool override
    {
        if (focus_in_action_ == Action::Clear) {
            if (this->core_.text().empty())
                this->set_text(stored_text_);
        }
        return Widget::focus_out_event();
    }

   private:
    Action focus_in_action_;
    Glyph_string stored_text_;  // stored on focus_in
};

}  // namespace ox::detail
#endif  // TERMOX_WIDGET_WIDGETS_DETAIL_TEXTLINE_BASE_HPP
