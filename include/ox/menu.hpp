/** @file
 * Popup menus, nested menu items, and menu bars.
 */
#pragma once
#include <memory>
#include <ox/button.hpp>
#include <ox/command.hpp>
#include <ox/listview.hpp>
#include <ox/popup.hpp>
namespace ox {
/** Null command is a separator. Children describe a submenu. */
struct MenuItem {
    /** Shared command; null represents a noninteractive menu separator. */
    std::shared_ptr<Command> command;
    /** Owned submenu entries; empty means this item is a leaf action. */
    std::vector<MenuItem> children = {};
};
/** Owned menu model over a row list, with nested popup submenus and shared Command leaf
 * actions.
 */
class Menu : public ListView {
   public:
    using ListView::handle;
    /** Validate and retain menu entries; null commands are noninteractive separators. */
    explicit Menu(std::vector<MenuItem> items = {});
    /** Destroy nested popup submenus and their retained command references. */
    ~Menu();
    /** Transfer widget identity, retained state, and signal subscriptions; tracked
     * references follow the move.
     */
    Menu(Menu&&);
    /** Replace this widget's identity and retained state with the source; previous
     * lifetime views of this identity expire.
     */
    auto operator=(Menu&&) -> Menu&;
    /** Borrow original nested menu entries until replacement. */
    auto entries() const -> std::span<MenuItem const> { return entries_; }
    void refresh() override;
    /** Validate and replace the nested menu model, close stale submenus, and refresh
     * visible rows.
     */
    void set_items(std::vector<MenuItem>);
    /** Close the active child submenu and request layout; leave the parent menu available. */
    void close_submenu()
    {
        submenu_.reset();
        submenu_row_.reset();
        request_layout();
    }
    /** Emits the activated leaf's index path; MenuBar prepends the top-level menu index. */
    sl::Signal<void(std::vector<std::size_t> const&)> on_trigger;
    /** Requests that the host close this submenu and return to its parent menu. */
    sl::Signal<void()> on_back;
    /** Requests switching to an adjacent top-level menu, with signed direction. */
    sl::Signal<void(int)> on_next;
    /** Dispatch matching enabled shared command shortcuts from this menu tree and return
     * action effects.
     */
    auto handle_command(KeyEvent const&) -> EventResponse;
    auto paint(Canvas) -> Cursor override;
    auto handle(KeyEvent const&) -> EventResponse override;
    auto handle(MousePress const&) -> EventResponse override;
    auto handle(MouseMove const&) -> EventResponse override;
    auto get_children() -> zzz::Generator<Widget&> override;
    auto get_children() const -> zzz::Generator<Widget const&> override;

   private:
    using ListView::erase_item;
    using ListView::insert_item;
    using ListView::set_item;
    std::vector<MenuItem> entries_;
    std::unique_ptr<Widget> submenu_;
    std::optional<std::size_t> submenu_row_;
    void activate_item(std::size_t);
};
/** A top-level menu-bar label and its nested menu entries. */
struct MenuGroup {
    /** Printable single-line menu-bar heading. */
    std::string text;
    /** Owned entries belonging to this top-level menu group. */
    std::vector<MenuItem> items;
};
/** Top-level menu headings and anchored popups with F10, Alt-mnemonic, arrow, and
 * pointer navigation.
 */
class MenuBar : public Widget {
   public:
    using Widget::handle;
    /** Own validated menu groups, buttons, and root popups; retain shared leaf commands. */
    explicit MenuBar(std::vector<MenuGroup> groups = {});
    /** Emits the activated leaf's index path; MenuBar prepends the top-level menu index. */
    sl::Signal<void(std::vector<std::size_t> const&)> on_trigger;
    /** Borrow a top-level menu. @throws std::out_of_range The group index is invalid. */
    auto menu(std::size_t index) -> Menu& { return popups_.at(index).child; }
    /** Open a valid root menu and transfer focus; return whether it opened, throwing for
     * an invalid index.
     */
    auto open(std::size_t) -> bool;
    /** Dismiss all open menu popups and clear top-level navigation state. */
    void close();
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
    /** Handle F10/Alt mnemonic or shared command bindings from the eligible application
     * shortcut scope.
     */
    auto handle_shortcut(KeyEvent const&) -> EventResponse;
    auto handle(KeyEvent const&) -> EventResponse override;
    auto get_children() -> zzz::Generator<Widget&> override;
    auto get_children() const -> zzz::Generator<Widget const&> override;

   protected:
    void on_arrange(Rect) override;

   private:
    std::vector<Button> buttons_;
    std::vector<Popup<Menu>> popups_;
    std::optional<std::size_t> current_;
};
}  // namespace ox
