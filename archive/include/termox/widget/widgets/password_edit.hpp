#ifndef TERMOX_WIDGET_WIDGETS_PASSWORD_EDIT_HPP
#define TERMOX_WIDGET_WIDGETS_PASSWORD_EDIT_HPP
#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/widget/widgets/line_edit.hpp>

namespace ox {

/// A Line_edit that veils the input characters so the contents are hidden.
/** Does not provide a validator, bad chars in a password should be caught after
 *  submission; Does not provide Alignment choice, always left aligned; Does not
 *  provide focus in action. */
class Password_edit : public Line_edit {
   public:
    struct Parameters {
        Glyph veil         = U'*';
        bool show_contents = false;
    };

   public:
    explicit Password_edit(Glyph veil = U'*', bool show_contents = false);

    explicit Password_edit(Parameters p);

   public:
    /// Set the Glyph that is show in place of the actual input Glyphs.
    void set_veil(Glyph x);

    /// Return the currently set veil.
    [[nodiscard]] auto veil() const -> Glyph;

    /// Display each input char instead of veil if \p enable is true.
    void show_contents(bool enable);

    /// Return true if input chars are not hidden.
    [[nodiscard]] auto shows_contents() const -> bool;

   protected:
    auto paint_event(Painter& p) -> bool;

   private:
    Glyph veil_         = U'*';
    bool show_contents_ = false;

   private:
    using Line_edit::disable_validator;
    using Line_edit::set_validator;
    using Line_edit::validator;
    using Textline_base::alignment;
    using Textline_base::clear_on_focus;
    using Textline_base::focus_in_action;
    using Textline_base::nothing_on_focus;
    using Textline_base::set_alignment;
    using Textline_base::set_focus_in_action;
};

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_PASSWORD_EDIT_HPP
