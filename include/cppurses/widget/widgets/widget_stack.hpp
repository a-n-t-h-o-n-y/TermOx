#ifndef CPPURSES_WIDGET_WIDGETS_WIDGET_STACK_HPP
#define CPPURSES_WIDGET_WIDGETS_WIDGET_STACK_HPP
#include <cstddef>
#include <memory>
#include <utility>
#include <vector>

#include <signals/signals.hpp>

#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/layouts/horizontal_layout.hpp>

namespace cppurses {
class Widget;

class Widget_stack : public Horizontal_layout {
   public:
    void set_active_page(std::size_t index);

    void sets_focus_on_change(bool sets_focus = true);

    template <typename T, typename... Args>
    T& make_page(Args&&... args);

    void add_page(std::unique_ptr<Widget> widget);
    void insert_page(std::size_t index, std::unique_ptr<Widget> widget);
    void remove_page(std::size_t index);
    void clear();

    std::size_t size() const;
    Widget* active_page() const;
    std::size_t active_page_index() const;

    /// Post an Enable_event to this widget, and all descendants.
    void enable(bool enable = true,
                bool post_child_polished_event = true) override;

    // Signals
    sig::Signal<void(std::size_t)> page_changed;

   private:
    Widget* active_page_{nullptr};
    bool sets_focus_{true};
};

// - - - - - - - - - - - - Template Implementations - - - - - - - - - - - - - -

template <typename T, typename... Args>
T& Widget_stack::make_page(Args&&... args) {
    this->add_page(std::make_unique<T>(std::forward<Args>(args)...));
    return static_cast<T&>(*this->children.get().back());
}

// - - - - - - - - - - - - - - - - Slots - - - - - - - - - - - - - - - - - - - -
namespace slot {

sig::Slot<void(std::size_t)> set_active_page(Widget_stack& stack);
sig::Slot<void()> set_active_page(Widget_stack& stack, std::size_t index);

sig::Slot<void(std::size_t)> remove_page(Widget_stack& stack);
sig::Slot<void()> remove_page(Widget_stack& stack, std::size_t index);

sig::Slot<void(std::size_t, std::unique_ptr<Widget>)> insert_page(
    Widget_stack& stack);
sig::Slot<void(std::unique_ptr<Widget>)> insert_page(Widget_stack& stack,
                                                     std::size_t index);

}  // namespace slot
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGETS_WIDGET_STACK_HPP
