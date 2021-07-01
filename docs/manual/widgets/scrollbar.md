# Scrollbar Widget

[`<termox/widget/widgets/scrollbar.hpp>`](../../../include/termox/widget/widgets/scrollbar.hpp)

A Scrollbar Widget, displaying progress through some structure, and user input
control of that progress.  Provides an `increment`, `decrement`, and
`new_position` Signals to notify of changes. The size parameter tells the
scrollbar how many units of scroll there are to the scructure it is linked with,
and the current position the scrollbar is at is emitted with the `new_position`
signal. The `link` free function will connect a Scrollbar with either a
`Linear_layout` derived type, or a `Textbox` Widget.

```cpp
template <template <typename> typename Layout_t>
class Scrollbar : public Layout_t<Widget> {
   public:
    struct Parameters {
        std::size_t size     = 0;
        std::size_t position = invalid_position;
    };

   private:
    class Middle : public Widget {
       public:
        using Parameters = Scrollbar::Parameters;

        static auto constexpr invalid_position = -1uL;

       public:
        Middle(std::size_t size = 0, std::size_t position = 0);

        Middle(Parameters);

        void set_position(std::size_t p);

        void set_size(std::size_t s);

        void set_bar_fg(Color c);

        void set_bar_bg(Color c);

        // Finds the scroll position from a physical point on the bar.
        auto find_position_from_point(Point p) -> std::size_t;
    };

   public:
    static auto constexpr invalid_position = -1uL;

    Button& decrement_btn =
        this->template make_child<Button>(Glyph_string{top_symbol_});

    Middle& middle = this->template make_child<Middle>({0, invalid_position});

    Button& increment_btn =
        this->template make_child<Button>(Glyph_string{bottom_symbol_});

    sl::Signal<void(std::size_t)> new_position;
    sl::Signal<void()> decremented;
    sl::Signal<void()> incremented;

   public:
    Scrollbar(std::size_t size     = 0,
              std::size_t position = invalid_position);

    Scrollbar(Parameters);

    auto get_size() const -> std::size_t;

    void set_size(std::size_t s);

    void increment_size();

    void decrement_size();

    auto get_position() const -> std::size_t;

    void set_position(std::size_t p);

    void decrement_position();

    void increment_position();

    /// Returns true if button was a scoll wheel button
    auto handle_wheel(Mouse::Button button) -> bool;
};

// Links a Scrollbar to a Linear_layout type, changing the child offset on scroll.
template <template <typename> typename Layout_t,
          typename Child,
          typename Parameters>
void link(Scrollbar<Layout_t>& scrollbar,
          layout::detail::Linear_layout<Child, Parameters>& layout,
          bool hijack_scroll = true);

// Links a Scrollbar to a Textbox, scrolling lines of text on scroll.
template <template <typename> typename Layout_t>
void link(Scrollbar<Layout_t>& scrollbar, Textbox& textbox);
```
