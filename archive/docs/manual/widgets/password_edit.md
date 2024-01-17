# Password Edit Widget

[`<termox/widget/widgets/password_edit.hpp>`](../../../include/termox/widget/widgets/password_edit.hpp)

A `Line_edit` that veils the input characters so the contents are hidden. Does
not provide a validator, bad chars in a password should be caught after
submission; Does not provide Alignment choice, always left aligned; Does not
provide focus in action.

```cpp
class Password_edit : public Line_edit {
   public:
    struct Parameters {
        Glyph veil         = U'*';
        bool show_contents = false;
    };

   public:
    Password_edit(Glyph veil = U'*', bool show_contents = false);

    Password_edit(Parameters);

    // Set the Glyph that is show in place of the actual input Glyphs.
    void set_veil(Glyph x);

    // Return the currently set veil.
    auto veil() const -> Glyph;

    // Display each input char instead of veil if \p enable is true.
    void show_contents(bool enable);

    // Return true if input chars are not hidden.
    auto shows_contents() const -> bool;

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
```
