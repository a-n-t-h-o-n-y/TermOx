# Confirm Button Widget

[`<termox/widget/widgets/confirm_button.hpp>`](../../../include/termox/widget/widgets/confirm_button.hpp)

Button with a two-step press process, confirming the initial click with via a
second display.

```cpp
class Confirm_button : public STuple<Button, detail::Confirm_page> {
   public:
    struct Parameters {
        Glyph_string label        = U"";
        Glyph_string confirm_text = U"Confirm";
    };

    // Emitted once confirmation has been made.
    sl::Signal<void()> pressed;

    Button& main_btn                   = this->get<0>();
    detail::Confirm_page& confirm_page = this->get<1>();

   public:
    // Construct a Button with \p label and corresponding \p confirm_text.
    Confirm_button(Glyph_string label        = U"",
                   Glyph_string confirm_text = U"Confirm");

    Confirm_button(Parameters p);
};

namespace ox::detail {

class Confirm_page : public HArray<Button, 2> {
   public:
    struct Parameters { Glyph_string confirm_text; };

    Button& confirm_btn = this->get<0>();
    Button& exit_btn    = this->get<1>();

   public:
    Confirm_page(Glyph_string confirm_text);
    Confirm_page(Parameters);
};

}  // namespace ox::detail
```
