#ifndef CPPURSES_WIDGET_WIDGETS_MENU_STACK_HPP
#define CPPURSES_WIDGET_WIDGETS_MENU_STACK_HPP
#include <cstddef>
#include <memory>
#include <utility>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/stack.hpp>

namespace cppurses {
class Widget;
class Menu;

/// A Stack layout with a Menu to switch between pages.
/** Menu Widget is not counted as a Stack index, pages start from index 0. */
class Menu_stack : public layout::Stack {
    Menu& menu_;

   public:
    /// Construct an empty Menu_stack with \p title passed to Menu constructor.
    Menu_stack(Glyph_string title = "");

    /// Construct and append a page to the Stack.
    /** This will construct a child Widget of type T, using \p args passed to
     *  T's constructor, and then automatically disable it. Creates a menu item
     *  with \p title. Returns a reference to the created child Widget.*/
    template <typename T, typename... Args>
    T& make_page(Glyph_string title, Args&&... args);

    /// Add an existing Widget as a page to the end of the Stack.
    void append_page(Glyph_string title, std::unique_ptr<Widget> widget);

    /// Insert a Widget at \p index.
    /** No-op if \p index is larger than children.get().size() - 1. */
    void insert_page(Glyph_string title,
                     std::size_t index,
                     std::unique_ptr<Widget> widget);

    /// Remove a page from the Stack, by \p index value, and delete it.
    /** No-op if index is not valid. Sets active page to the menu if current
     *  active page is being deleted. */
    void delete_page(std::size_t index);

    /// Remove a page from the Stack, by \p index value, and return it.
    /** Useful if you need to move a page into another Widget. Use delete_page()
     *  if you want to remove a page and destroy it. No-op if index is not
     *  valid. Sets active page to menu if active page is being removed. */
    std::unique_ptr<Widget> remove_page(std::size_t index);

    /// Remove and delete all pages except menu.
    void clear();

    /// Return the number of pages in this Stack, not including the Menu Widget.
    std::size_t size() const { return this->Stack::size() - 1; }

    /// Return reference to the Menu Widget at the front of the Stack.
    Menu& menu() { return menu_; }

    /// Return const reference to the Menu Widget at the front of the Stack.
    const Menu& menu() const { return menu_; }

    /// Set the active page to the menu, useful to control returning to menu.
    void goto_menu();

    /// Set the active page, first page is index 0, etc...
    void set_active_page(std::size_t index);

   private:
    /// Remove item at \p index from menu, where index 0 is the menu.
    /** Reset menu as active page if the current active page is being removed.*/
    void remove_from_menu(std::size_t index);

    /// Insert new menu item and connect its signal to set index as active page.
    /** Used with menu as index 0. */
    void connect_to_menu(Glyph_string title, std::size_t index);

   protected:
    bool focus_in_event() override;
};

template <typename T, typename... Args>
T& Menu_stack::make_page(Glyph_string title, Args&&... args) {
    auto& ret = this->Stack::make_page<T>(std::forward<Args>(args)...);
    connect_to_menu(std::move(title), this->Stack::size() - 1);
    return ret;
}
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_MENU_STACK_HPP
