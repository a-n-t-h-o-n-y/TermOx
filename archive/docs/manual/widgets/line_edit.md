# Line Edit Widget

[`<termox/widget/widgets/line_edit.hpp>`](../../../include/termox/widget/widgets/line_edit.hpp)

A single height horizontal line for text editing, with an optional validator to
supress input for particular `char32_t` characters.

```cpp
class Line_edit : public detail::Textline_base {
   public:
    using Validator_t = std::function<bool(char32_t)>;

    struct Parameters {
        Glyph_string initial_text            = U"";
        Align alignment                      = Align::Left;
        Action focus_in_action               = Action::None;
        std::optional<Validator_t> validator = std::nullopt;
    };

    // Emitted on Enter key press, sends along the current contents.
    sl::Signal<void(std::string const&)> submitted;

    // Emitted on every printable key press, sends along the current contents.
    sl::Signal<void(std::string const&)> text_modified;

   public:
    Line_edit(Glyph_string initial_text            = U"",
              Align alignment                      = Align::Left,
              Action focus_in_action               = Action::None,
              std::optional<Validator_t> validator = std::nullopt);

    Line_edit(Parameters);

    // Set the input validator, allowing or disallowing specific char values.
    /* Invalid character input will result in no input. */
    void set_validator(std::optional<Validator_t> x);

    // Turn off the validator so all text input is valid.
    void disable_validator();

    // Return the current validator function object, or nullopt if not set.
    auto validator() const -> std::optional<Validator_t> const&;
};
```
