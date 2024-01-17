# Label Widget

[`<termox/widget/widgets/label.hpp>`](../../../include/termox/widget/widgets/label.hpp)

## `Label`

A single line of text with alignment, non-editable. Can be either Vertical or
Horizontal by passing in a Layout type.

```cpp
template <template <typename> typename Layout_t>
class Label : public Widget {
   public:
    struct Parameters {
        Glyph_string text      = Glyph_string{U""};
        Align alignment        = Align::Left;
        int extra_left         = 0;
        int extra_right        = 0;
        Growth growth_strategy = Growth::Static;
    };

   public:
    // Create a new Label Widget.
    /* \param text            The initial text used for the display.
     * \param alignment       The placement of the text on the Label.
     * \param extra_left      Align::Center takes this into account.
     * \param extra_right     Align::Center takes this into account
     * \param growth_strategy If the Label is resized to fit the text or not.
     */
    Label(Glyph_string text      = U"",
          Align alignment        = Align::Left,
          int extra_left         = 0,
          int extra_right        = 0,
          Growth growth_strategy = Growth::Static);

    // Construct directly with Parameters object.
    Label(Parameters);

   public:
    // Set text contents of Label and update display.
    void set_text(Glyph_string text);

    // Return the text given to set_text().
    auto text() const -> Glyph_string const&;

    // Set text alignment of Label and update display.
    void set_alignment(Align x);

    // Return the Align given to set_alignment().
    auto alignment() const -> Align;

    // Inform Label about space to left of Label for centered text offset.
    void set_extra_left(int x);

    // Return the amount given to set_extra_left().
    auto extra_left() const -> int;

    // Inform Label about space to right of Label for centered text offset.
    void set_extra_right(int x);

    // Return the amount given to set_extra_right().
    auto extra_right() const -> int;

    // Enable/Disable Dynamic size, where the Label's size is the text length.
    void set_growth_strategy(Growth type);

    // Return the value given to set_growth_strategy().
    auto growth_strategy() const -> Growth;
};

using HLabel = Label<layout::Horizontal>;
using VLabel = Label<layout::Vertical>;
```

## `Label_wrapper`

```cpp
template <typename Widget_t>
using HLabel_left = Label_left<layout::Horizontal, Widget_t>;

template <typename Widget_t>
using VLabel_left = Label_left<layout::Vertical, Widget_t>;

template <typename Widget_t>
using HLabel_right = Label_right<layout::Horizontal, Widget_t>;

template <typename Widget_t>
using VLabel_right = Label_right<layout::Vertical, Widget_t>;

template <typename Widget_t>
using VLabel_top = Label_top<layout::Vertical, Widget_t>;

template <typename Widget_t>
using HLabel_top = Label_top<layout::Horizontal, Widget_t>;

template <typename Widget_t>
using VLabel_bottom = Label_bottom<layout::Vertical, Widget_t>;

template <typename Widget_t>
using HLabel_bottom = Label_bottom<layout::Horizontal, Widget_t>;

// Implementations -------------------------------------------------------------

template <template <typename> typename Label_layout_t, typename Widget_t>
using Label_left =
    detail::Label_wrapper<Label_layout_t, Widget_t, layout::Horizontal, false>;

template <template <typename> typename Label_layout_t, typename Widget_t>
using Label_right =
    detail::Label_wrapper<Label_layout_t, Widget_t, layout::Horizontal, true>;

template <template <typename> typename Label_layout_t, typename Widget_t>
using Label_top =
    detail::Label_wrapper<Label_layout_t, Widget_t, layout::Vertical, false>;

template <template <typename> typename Label_layout_t, typename Widget_t>
using Label_bottom =
    detail::Label_wrapper<Label_layout_t, Widget_t, layout::Vertical, true>;

namespace ox::detail {

// Wraps a Widget_t object with a label.
/* Wrapper_layout is the layout of *this, if label_last is true, the Label is
 * after the wrapped Widget_t. */
template <template <typename> typename Label_layout,
          typename Widget_t,
          template <typename>
          typename Wrapper_layout,
          bool label_last>
class Label_wrapper : public Wrapper_layout<Widget> {
   public:
    struct Parameters {
        Glyph_string text      = Glyph_string{U""};
        Align alignment        = label_last ? Align::Left : Align::Right;
        int extra_left         = 0;
        int extra_right        = 0;
        Growth growth_strategy = Growth::Static;
    };

    Label_t& label;
    Widget& padding = this->template make_child() | padding_policy();
    Widget_t& wrapped;

   public:
    // Construct a new Label and wrapped Widget_t.
    /* Only takes Label constructor args, if you need to pass in args to the
     * wrapped Widget_t, then use the Label::Parameters overload. */
    Label_wrapper(Glyph_string text      = U"",
                  Align alignment        = Align::Left,
                  int extra_left         = 0,
                  int extra_right        = 0,
                  Growth growth_strategy = Growth::Static);

    // Constructs Label with given parameters, and Widget_t with args...
    template <typename... Args>
    Label_wrapper(Parameters p, Args&&... args);
};

}  // namespace ox::detail
```
