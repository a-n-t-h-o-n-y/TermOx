#include <cppurses/widget/widgets/menu_stack.hpp>

#include <signals/signal.hpp>
#include <signals/slot.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/menu.hpp>

namespace {
const std::size_t menu_index{0u};
}  // namespace

namespace cppurses {

Menu_stack::Menu_stack(Glyph_string title)
    : menu_{this->Stack::make_page<Menu>(std::move(title))} {
    this->Stack::set_active_page(menu_index);
    this->focus_policy = Focus_policy::Click;
}

void Menu_stack::append_page(Glyph_string title,
                             std::unique_ptr<Widget> widget) {
    this->Stack::append_page(std::move(widget));
    this->connect_to_menu(std::move(title), this->Stack::size() - 1);
}

void Menu_stack::insert_page(Glyph_string title,
                             std::size_t index,
                             std::unique_ptr<Widget> widget) {
    this->Stack::insert_page(index + 1, std::move(widget));
    this->connect_to_menu(std::move(title), index + 1);
}

void Menu_stack::delete_page(std::size_t index) {
    this->remove_from_menu(index);
    this->Stack::delete_page(index + 1);
}

std::unique_ptr<Widget> Menu_stack::remove_page(std::size_t index) {
    this->remove_from_menu(index);
    return this->Stack::remove_page(index + 1);
}

void Menu_stack::clear() {
    this->goto_menu();
    for (auto i = 1u; i < this->Stack::size(); ++i) {
        this->children.get()[i]->close();
    }
}

void Menu_stack::goto_menu() {
    this->Stack::set_active_page(menu_index);
}

void Menu_stack::set_active_page(std::size_t index) {
    this->Stack::set_active_page(index + 1);
}

bool Menu_stack::focus_in_event() {
    Focus::set_focus_to(&menu_);
    return Stack::focus_in_event();
}

void Menu_stack::remove_from_menu(std::size_t index) {
    menu_.remove_item(index);
    if (this->Stack::active_page_index() == ++index) {
        this->Stack::set_active_page(menu_index);
    }
}

void Menu_stack::connect_to_menu(Glyph_string title, std::size_t index) {
    auto& signal = menu_.add_item(std::move(title));
    signal.connect(slot::set_active_page(*this, index));
}
}  // namespace cppurses
