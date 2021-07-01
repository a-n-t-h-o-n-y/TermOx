# Titlebar Widget

[`<termox/widget/widgets/titlebar.hpp>`](../../../include/termox/widget/widgets/titlebar.hpp)

Provides a centered title and flush-right exit button. The exit button will call
System::quit() when pressed. Titlebar is fixed at a height of 1.

```cpp
class Titlebar : public HPair<HLabel, Button> {
   public:
    struct Parameters { Glyph_string title_text = U""; };

    HLabel& title       = this->first;
    Button& exit_button = this->second;

   public:
    // Construct a Titlebar with centered \p title.
    Titlebar(Glyph_string title_text = U"");

    Titlebar(Parameters);
};
```
