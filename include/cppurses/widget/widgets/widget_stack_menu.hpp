#ifndef CPPURSES_WIDGET_WIDGETS_WIDGET_STACK_MENU_HPP
#define CPPURSES_WIDGET_WIDGETS_WIDGET_STACK_MENU_HPP
#include <cstddef>
#include <memory>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/menu.hpp>

namespace cppurses {
class Widget;

class Widget_stack_menu : public layout::Vertical {
   public:
    Widget_stack_menu(Glyph_string title = "");

    template <typename T, typename... Args>
    T& make_page(Glyph_string title, Args&&... args);

    // Menu is index 0
    void add_page(Glyph_string title, std::unique_ptr<Widget> widget);
    void insert_page(Glyph_string title,
                     std::size_t index,
                     std::unique_ptr<Widget> widget);
    void remove_page(std::size_t index);

    std::size_t size() const;

    Menu& menu();
    const Menu& menu() const;

    // Signals
    sig::Signal<void(std::size_t)> page_changed;

   protected:
    bool focus_in_event() override;

   private:
    layout::Stack& stack_{this->make_child<layout::Stack>()};
    Menu& menu_;

    void initialize();
};

// - - - - - - - - - - - - Template Implementations - - - - - - - - - - - - - -

template <typename T, typename... Args>
T& Widget_stack_menu::make_page(Glyph_string title, Args&&... args) {
    auto& ret = stack_.make_page<T>(std::forward<Args>(args)...);
    auto& signal = menu_.add_item(std::move(title));
    signal.connect(slot::set_active_page(stack_, this->size() - 1));
    this->update();
    return ret;
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_WIDGET_STACK_MENU_HPP
