#ifndef TERMOX_WIDGET_WIDGETS_MENU_STACK_HPP
#define TERMOX_WIDGET_WIDGETS_MENU_STACK_HPP
#include <cstddef>
#include <memory>
#include <utility>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/system.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/layouts/stack.hpp>
#include <termox/widget/widgets/menu.hpp>

namespace ox {

/// A Stack layout with a Menu to switch between pages.
/** Menu Widget is not counted as a Stack index, pages start from index 0. */
class Menu_stack : public layout::Stack<Widget> {
   private:
    static auto constexpr menu_index_ = 0uL;

   public:
    /// Construct an empty Menu_stack with \p title passed to Menu constructor.
    Menu_stack(Glyph_string title = "")
        : menu_{this->Stack::make_page<Menu>(std::move(title))}
    {
        this->Stack::set_active_page(menu_index_);
        this->focus_policy = Focus_policy::Direct;
    }

    /// Construct and append a page to the Stack.
    /** This will construct a child Widget of type Widget_t, using \p args
     *  passed to Widget_t's constructor, and then automatically disable it.
     *  Creates a menu item with \p title. Returns a reference to the created
     *  child Widget.*/
    template <typename Widget_t, typename... Args>
    auto make_page(Glyph_string title, Args&&... args) -> Widget_t&
    {
        static_assert(std::is_base_of<Widget, Widget_t>::value,
                      "Menu_stack::make_page: Widget_t must be a Widget type");
        auto& child =
            this->Stack::make_page<Widget_t>(std::forward<Args>(args)...);
        this->connect_to_menu(std::move(title), this->Stack::size() - 1);
        return child;
    }

    /// Add an existing Widget as a page to the end of the Stack.
    template <typename Widget_t>
    void append_page(Glyph_string title, std::unique_ptr<Widget_t> w_ptr)
    {
        static_assert(
            std::is_base_of<Child_t, Widget_t>::value,
            "Menu_stack::append_page: Widget_t must be a Child_t type");
        this->Stack::append_page(std::move(w_ptr));
        this->connect_to_menu(std::move(title), this->Stack::size() - 1);
    }

    template <typename Widget_t>
    void append_page(Glyph_string title, Widget_t&& w)
    {
        this->append_page(std::move(title), std::make_unique<Widget_t>(
                                                std::forward<Widget_t>(w)));
    }

    /// Insert a Widget at \p index.
    /** No-op if \p index is larger than Widget::child_count() - 1. */
    void insert_page(Glyph_string title,
                     std::unique_ptr<Widget> widget,
                     std::size_t index)
    {
        this->Stack::insert_page(std::move(widget), index + 1);
        this->connect_to_menu(std::move(title), index + 1);
    }

    /// Remove a page from the Stack, by \p index value, and delete it.
    /** No-op if index is not valid. Sets active page to the menu if current
     *  active page is being deleted. */
    void delete_page(std::size_t index)
    {
        this->remove_from_menu(index + 1);
        this->Stack::delete_page(index + 1);
    }

    /// Remove a page from the Stack, by \p index value, and return it.
    /** Useful if you need to move a page into another Widget. Use delete_page()
     *  if you want to remove a page and destroy it. No-op if index is not
     *  valid. Sets active page to menu if active page is being removed. */
    auto remove_page(std::size_t index) -> std::unique_ptr<Widget>
    {
        this->remove_from_menu(index + 1);
        return this->Stack::remove_page(index + 1);
    }

    /// Remove and delete all pages except menu.
    void clear()
    {
        this->goto_menu();
        // Can't use a range-for loop, delete_page modifies the child_list_
        while (this->child_count() > 1)
            this->delete_page(1);
    }

    /// Return the number of pages in this Stack, not including the Menu Widget.
    auto size() const -> std::size_t { return this->Stack::size() - 1; }

    /// Return reference to the Menu Widget at the front of the Stack.
    auto menu() -> Menu& { return menu_; }

    /// Return const reference to the Menu Widget at the front of the Stack.
    auto menu() const -> Menu const& { return menu_; }

    /// Set the active page to the menu, useful to control returning to menu.
    void goto_menu() { this->Stack::set_active_page(menu_index_); }

    /// Set the active page, first page is index 0, etc...
    void set_active_page(std::size_t index)
    {
        this->Stack::set_active_page(index + 1);
    }

   protected:
    auto focus_in_event() -> bool override
    {
        System::set_focus(menu_);
        return true;
    }

   private:
    Menu& menu_;

   private:
    /// Remove item at \p index from menu, where index 0 is the menu.
    /** Reset menu as active page if the current active page is being removed.*/
    void remove_from_menu(std::size_t index)
    {
        menu_.remove_item(index - 1);
        if (this->Stack::active_page_index() == index)
            this->Stack::set_active_page(menu_index_);
    }

    /// Insert new menu item and connect its signal to set index as active page.
    /** Used with menu as index 0. */
    void connect_to_menu(Glyph_string title, std::size_t index)
    {
        auto& signal = menu_.insert_item(std::move(title), index - 1);
        signal.connect(slot::set_active_page(*this, index));
    }
};

/// Helper function to create an instance.
template <typename... Args>
auto menu_stack(Args&&... args) -> std::unique_ptr<Menu_stack>
{
    return std::make_unique<Menu_stack>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_MENU_STACK_HPP
