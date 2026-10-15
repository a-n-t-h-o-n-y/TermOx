#pragma once
#include <memory>
#include <ox/button.hpp>
#include <ox/command.hpp>
#include <ox/listview.hpp>
#include <ox/popup.hpp>
namespace ox {
/** Null command is a separator. Children describe a submenu. */
struct MenuItem {
    std::shared_ptr<Command> command;
    std::vector<MenuItem> children = {};
};
class Menu : public ListView {
   public:
    using ListView::handle;
    explicit Menu(std::vector<MenuItem> items = {});
    ~Menu();
    Menu(Menu&&);
    auto operator=(Menu&&) -> Menu&;
    auto entries() const -> std::span<MenuItem const> { return entries_; }
    void refresh() override;
    void set_items(std::vector<MenuItem>);
    void close_submenu()
    {
        submenu_.reset();
        submenu_row_.reset();
        request_layout();
    }
    sl::Signal<void(std::vector<std::size_t> const&)> on_trigger;
    sl::Signal<void()> on_back;
    sl::Signal<void(int)> on_next;
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
struct MenuGroup {
    std::string text;
    std::vector<MenuItem> items;
};
class MenuBar : public Widget {
   public:
    using Widget::handle;
    explicit MenuBar(std::vector<MenuGroup> groups = {});
    sl::Signal<void(std::vector<std::size_t> const&)> on_trigger;
    auto menu(std::size_t index) -> Menu& { return popups_.at(index).child; }
    auto open(std::size_t) -> bool;
    void close();
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
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
