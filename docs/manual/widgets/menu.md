# Menu Widget

[`<termox/widget/widgets/menu.hpp>`](../../../include/termox/widget/widgets/menu.hpp)

A vertical list of scrollable, user selectable labels.

```cpp
class Menu : public VPair<Menu_list, Widget> {
   public:
    Menu();

    // Append item to the end of list, displayed with \p label.
    /* Returns a Signal ref which will be called when this item is selected. */
    auto append_item(Glyph_string label) -> sl::Signal<void()>&;

    // Insert item at \p index into the Menu_list, displayed with \p label.
    /* Returns a Signal ref which will be called when this item is selected. */
    auto insert_item(Glyph_string label, std::size_t index) -> sl::Signal<void()>&;

    // Remove item a \p index in the Menu_list, no-op if \p index is invalid.
    void remove_item(std::size_t index);
};

class Menu_list : public Passive<layout::Selecting<layout::Vertical<Menu_item>>> {
   public:
    Menu_list();

    // Append item to the end of list, displayed with \p label.
    /* Returns a Signal ref which will be called when this item is selected. */
    auto append_item(Glyph_string label) -> sl::Signal<void()>&;

    // Insert item at \p index into the Menu_list, displayed with \p label.
    /* Returns a Signal ref which will be called when this item is selected. */
    auto insert_item(Glyph_string label, std::size_t index) -> sl::Signal<void()>&;

    // Remove item a \p index in the Menu_list, no-op if \p index is invalid.
    void remove_item(std::size_t index);
};

class Menu_item : public Selectable<HLabel> {
   public:
    sl::Signal<void()> selected;

    Menu_item(Glyph_string label);
};
```
