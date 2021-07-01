# Matrix View Widget

[`<termox/widget/widgets/matrix_view.hpp>`](../../../include/termox/widget/widgets/matrix_view.hpp)

Displays a [`Glyph_matrix`](../../../include/termox/painter/glyph_matrix.hpp)
object.

```cpp
class Matrix_view : public Widget {
   public:
    struct Parameters { Glyph_matrix matrix; };

    Glyph_matrix matrix;

   public:
    Matrix_view(Glyph_matrix matrix_);

    Matrix_view(Parameters);
};
```
