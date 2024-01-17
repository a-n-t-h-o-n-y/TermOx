# Selectable Widget

[`<termox/widget/widgets/selectable.hpp>`](../../../include/termox/widget/widgets/selectable.hpp)

Wraps a Widget type to provide select() and unselect() methods to change the
display, so the wrapped Widget is 'Selectable'. The default implementation
applies the Inverse trait to the entire widget on select(), and uninverts on
unselect().

```cpp
template <typename Widget_t,
          typename Select_method   = void,
          typename Unselect_method = void>
class Selectable : public Widget_t {
   public:
    // Provide two methods to modify Widget_t. Signature: void(Widget_t&);
    Selectable(Select_method s, Unselect_method u);

    Selectable(Select_method s, Unselect_method u, typename Widget_t::Parameters p);

    // Change visual to mark as selected.
    void select();

    // Change visual to mark as unselected.
    void unselect();
};
```
