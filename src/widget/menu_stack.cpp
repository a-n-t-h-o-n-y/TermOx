#include <cppurses/widget/widgets/menu_stack.hpp>

#include <signals/signal.hpp>
#include <signals/slot.hpp>

#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/menu.hpp>

namespace cppurses {

Menu_stack::Menu_stack(Glyph_string title)
    : menu_{stack_.make_page<Menu>(std::move(title))} {
    this->initialize();
    this->focus_policy = Focus_policy::Click;
}

void Menu_stack::initialize() {
    stack_.page_changed.connect(sig::Slot<void(std::size_t)>{page_changed});
    stack_.set_active_page(0);
    auto& esc_short = Shortcuts::add_shortcut(Key::Escape);
    esc_short.connect(slot::set_active_page(stack_, 0));
    esc_short.connect([this] { Focus::set_focus_to(&menu_); });
}

void Menu_stack::add_page(Glyph_string title, std::unique_ptr<Widget> widget) {
    stack_.add_page(std::move(widget));
    auto& signal = menu_.add_item(std::move(title));
    signal.connect(slot::set_active_page(stack_, this->size() - 1));
}

void Menu_stack::insert_page(Glyph_string title,
                             std::size_t index,
                             std::unique_ptr<Widget> widget) {
    stack_.insert_page(index, std::move(widget));
    auto& signal = menu_.insert_item(std::move(title), index);
    signal.connect(slot::set_active_page(stack_, index));
}

void Menu_stack::remove_page(std::size_t index) {
    menu_.remove_item(index);
}

std::size_t Menu_stack::size() const {
    return stack_.size();
}

Menu& Menu_stack::menu() {
    return menu_;
}

const Menu& Menu_stack::menu() const {
    return menu_;
}

bool Menu_stack::focus_in_event() {
    Focus::set_focus_to(&menu_);
    return layout::Vertical::focus_in_event();
    ;
}
}  // namespace cppurses
