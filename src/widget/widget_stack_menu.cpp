#include <cppurses/system/focus.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/widget/widgets/menu.hpp>
#include <cppurses/widget/widgets/widget_stack_menu.hpp>

namespace cppurses {
Widget_stack_menu::Widget_stack_menu(Glyph_string title)
    : menu_{stack_.make_page<Menu>(std::move(title))} {
    this->initialize();
}

void Widget_stack_menu::add_page(Glyph_string title,
                                 std::unique_ptr<Widget> widget) {
    stack_.add_page(std::move(widget));
    auto& signal = menu_.add_item(std::move(title));
    signal.connect(slot::set_active_page(stack_, this->size() - 1));
    // menu_.make_item(std::move(title),
    //                 slot::set_active_page(stack_, this->size() - 1));
    this->update();
}

void Widget_stack_menu::insert_page(Glyph_string title,
                                    std::size_t index,
                                    std::unique_ptr<Widget> widget) {
    stack_.insert_page(index, std::move(widget));
    auto& signal = menu_.insert_item(std::move(title), index);
    signal.connect(slot::set_active_page(stack_, index));
    this->update();
}

std::unique_ptr<Widget> Widget_stack_menu::remove_page(std::size_t index) {
    menu_.remove_item(index);
    this->update();
    return stack_.remove_page(index);
}

std::size_t Widget_stack_menu::size() const {
    return stack_.size();
}

Menu& Widget_stack_menu::menu() {
    return menu_;
}

const Menu& Widget_stack_menu::menu() const {
    return menu_;
}

void Widget_stack_menu::initialize() {
    this->focus_policy = Focus_policy::Strong;
    stack_.page_changed.connect(page_changed);

    // h_layout_.height_policy.type(Size_policy::Fixed);
    // h_layout_.height_policy.hint(1);
    // main_menu_button.width_policy.type(Size_policy::Minimum);
    // main_menu_button.width_policy.hint(11);
    // empty_space_.width_policy.stretch(2);
    // set_background(empty_space_, Color::Light_gray);
    // set_background(main_menu_button, Color::Light_blue);

    stack_.set_active_page(0);

    // main_menu_button.clicked.connect(slot::set_active_page(stack_, 0));
    // main_menu_button.clicked.connect([this] { Focus::set_focus_to(&menu_);
    // });
    auto& esc_short = Shortcuts::add_shortcut(Key::Escape);
    esc_short.connect(slot::set_active_page(stack_, 0));
    esc_short.connect([this] { Focus::set_focus_to(&menu_); });
}

bool Widget_stack_menu::focus_in_event() {
    Focus::set_focus_to(&menu_);
    return true;
}

}  // namespace cppurses
