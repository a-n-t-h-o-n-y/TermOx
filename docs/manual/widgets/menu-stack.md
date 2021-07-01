# Menu Stack Widget

[`<termox/widget/widgets/menu_stack.hpp>`](../../../include/termox/widget/widgets/menu_stack.hpp)

A Stack layout with a Menu to switch between pages. Menu Widget is not counted
as a Stack index, pages start from index 0.

```cpp
class Menu_stack : public layout::Stack<Widget> {
   public:
    // Construct an empty Menu_stack.
    Menu_stack();

    // Construct and append a page to the Stack.
    /* This will construct a child Widget of type Widget_t, using \p args
     * passed to Widget_t's constructor, and then automatically disable it.
     * Creates a menu item with \p title. Returns a reference to the created
     * child Widget.*/
    template <typename Widget_t, typename... Args>
    auto make_page(Glyph_string title, Args&&... args) -> Widget_t&;

    // Add an existing Widget as a page to the end of the Stack.
    template <typename Widget_t>
    void append_page(Glyph_string title, std::unique_ptr<Widget_t> w_ptr);

    // Insert a Widget at \p index.
    /* No-op if \p index is larger than Widget::child_count() - 1. */
    void insert_page(Glyph_string title, std::unique_ptr<Widget> widget, std::size_t index);

    // Remove a page from the Stack, by \p index value, and delete it.
    /* No-op if index is not valid. Sets active page to the menu if current
     * active page is being deleted. */
    void delete_page(std::size_t index);

    // Remove a page from the Stack, by \p index value, and return it.
    /* Useful if you need to move a page into another Widget. Use delete_page()
     * if you want to remove a page and destroy it. No-op if index is not
     * valid. Sets active page to menu if active page is being removed. */
    auto remove_page(std::size_t index) -> std::unique_ptr<Widget>;

    // Remove and delete all pages except menu.
    void clear();

    // Return the number of pages in this Stack, not including the Menu Widget.
    auto size() const -> std::size_t;

    // Return reference to the Menu Widget at the front of the Stack.
    auto menu() -> Menu&;

    // Return const reference to the Menu Widget at the front of the Stack.
    auto menu() const -> Menu const&;

    // Set the active page to the menu, useful to control returning to menu.
    void goto_menu();

    // Set the active page, first page is index 0, etc...
    void set_active_page(std::size_t index);
};
```
