# Line Widgets

[`<termox/widget/widgets/line.hpp>`](../../../include/termox/widget/widgets/line.hpp)

A Vertical or Horizontal single width/height line divider Widget.

```cpp
template <template <typename> typename Layout_t>
class Line : public Widget {
   public:
    Line();
};

using VLine = Line<layout::Vertical>;
using HLine = Line<layout::Horizontal>;
```
