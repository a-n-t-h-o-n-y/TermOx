#ifndef WIDGET_WIDGETS_WIDGET_STACK_MENU_HPP
#define WIDGET_WIDGETS_WIDGET_STACK_MENU_HPP
#include "painter/glyph_string.hpp"
#include "widget/layouts/horizontal_layout.hpp"
#include "widget/layouts/vertical_layout.hpp"
#include "widget/widget.hpp"
#include "widget/widgets/menu.hpp"
#include "widget/widgets/push_button.hpp"
#include "widget/widgets/widget_stack.hpp"

namespace cppurses {

// Menu is index 0
class Widget_stack_menu : public Vertical_layout {
   public:
    Widget_stack_menu();

    template <typename T, typename... Args>
    T& make_page(Glyph_string title, Args&&... args);

    void add_page(Glyph_string title, std::unique_ptr<Widget> widget);
    void insert_page(Glyph_string title,
                     std::size_t index,
                     std::unique_ptr<Widget> widget);
    std::unique_ptr<Widget> remove_page(std::size_t index);

    std::size_t size() const;

    Menu& menu();
    const Menu& menu() const;

    // Signals
    sig::Signal<void(std::size_t)> page_changed;

   protected:
    bool focus_in_event() override;

   private:
    Widget_stack& stack_{this->make_child<Widget_stack>()};
    Menu& menu_{stack_.make_page<Menu>("Main Menu")};
    Horizontal_layout& h_layout_{this->make_child<Horizontal_layout>()};
    Widget& empty_space_{h_layout_.make_child<Widget>()};
    Push_button& main_menu_button{
        h_layout_.make_child<Push_button>("Main Menu")};

    void initialize();
};

// - - - - - - - - - - - - Template Implementations - - - - - - - - - - - - - -

template <typename T, typename... Args>
T& Widget_stack_menu::make_page(Glyph_string title, Args&&... args) {
    auto& ret = stack_.make_page<T>(std::forward<Args>(args)...);
    menu_.make_item(std::move(title),
                    slot::set_active_page(stack_, this->size() - 1));
    this->update();
    return ret;
}

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_WIDGET_STACK_MENU_HPP
