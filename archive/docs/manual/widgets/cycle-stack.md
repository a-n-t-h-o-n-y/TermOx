# Cycle Stack Widget

[`<termox/widget/widgets/cycle_stack.hpp>`](../../../include/termox/widget/widgets/cycle_stack.hpp)

A `layout::Stack` with an interface to cycle through each Widget in the stack.

```cpp
template <typename Child = Widget>
class Cycle_stack : public VPair<detail::CS_top_row, layout::Stack<Child>> {
   public:
    using Parameters = typename layout::Stack<Child>::Parameters;

    detail::CS_top_row& top_row = this->first;
    layout::Stack<Child>& stack = this->second;

   public:
    Cycle_stack(Parameters parameters);

    template <typename... Widget_t>
    Cycle_stack(std::pair<Glyph_string, std::unique_ptr<Widget_t>>... children);

    // Construct a new Widget_t object and add it to the end of the Stack.
    /* Returns a reference to this newly created page. \p title is passed to
     * the Cycle_box to display when this page is active. */
    template <typename Widget_t = Child, typename... Args>
    auto make_page(Glyph_string title, Args&&... args) -> Widget_t&;

    // Append a page to the Stack.
    /* \p title is passed to the Cycle_box associated with this page. */
    auto append_page(Glyph_string title, std::unique_ptr<Child> widget) -> Child&;
};

namespace ox::detail {

// User interface to cycle through the pages of the Stack.
class CS_top_row : public HTuple<Button, Cycle_box, Button> {
   public:
    Button& left_btn     = this->get<0>();
    Cycle_box& cycle_box = this->get<1>();
    Button& right_btn    = this->get<2>();

   public:
    CS_top_row();
};

}  // namespace ox::detail

```
