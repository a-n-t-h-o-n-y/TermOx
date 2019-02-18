#include <cppurses/widget/widgets/menu.hpp>

#include <cstddef>
#include <cstdint>
#include <iterator>
#include <string>
#include <utility>

#include <signals/signals.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/widget.hpp>
#include <cppurses/widget/widgets/label.hpp>
#include <cppurses/widget/widgets/push_button.hpp>

namespace cppurses {

Menu::Menu_item::Menu_item(Push_button& ref) : button{ref} {}

Menu::Menu(Glyph_string title)
    : title_{this->make_child<Label>(std::move(title))} {
    this->focus_policy = Focus_policy::Strong;
    title_.set_alignment(Alignment::Center);
    title_.brush.add_attributes(Attribute::Bold);
    space1.wallpaper = L'─';
}

sig::Signal<void()>& Menu::add_item(Glyph_string label) {
    Push_button& button_ref{this->make_child<Push_button>(std::move(label))};
    button_ref.install_event_filter(*this);
    items_.emplace_back(button_ref);
    if (items_.size() == 1) {
        this->select_item(0);
    }
    button_ref.height_policy.fixed(1);
    auto& signal_ref{items_.back().selected};
    button_ref.clicked.connect([this, index = items_.size() - 1] {
        this->select_item(index);
        this->call_current_item();
    });
    this->update();
    return signal_ref;
}

sig::Signal<void()>& Menu::insert_item(Glyph_string label, std::size_t index) {
    auto button_ptr = std::make_unique<Push_button>(std::move(label));
    button_ptr->install_event_filter(*this);
    button_ptr->height_policy.fixed(1);
    auto& new_button = *button_ptr;
    items_.insert(std::begin(items_) + index, Menu_item{new_button});
    if (items_.size() == 1) {
        this->select_item(0);
    }
    auto& signal_ref = items_[index].selected;
    new_button.clicked.connect([this, index] { items_[index].selected(); });
    this->update();
    return signal_ref;
}

void Menu::remove_item(std::size_t index) {
    if (index >= items_.size()) {
        return;
    }
    items_[index].button.get().close();
    items_.erase(std::begin(items_) + index);
    if (index == selected_index_) {
        this->select_item(0);
    }
    this->update();
}

void Menu::select_up(std::size_t n) {
    auto new_index = selected_index_;
    if (new_index > n) {
        new_index -= n;
    } else {
        new_index = 0;
    }
    this->select_item(new_index);
}

void Menu::select_down(std::size_t n) {
    this->select_item(selected_index_ + n);
}

void Menu::select_item(std::size_t index) {
    if (items_.empty()) {
        return;
    }
    auto& previous_btn = items_[selected_index_].button.get();
    previous_btn.brush.remove_attributes(Attribute::Inverse);
    previous_btn.update();
    if (index >= items_.size()) {
        selected_index_ = items_.size() - 1;
    } else {
        selected_index_ = index;
    }
    auto& current_btn = items_[selected_index_].button.get();
    current_btn.brush.add_attributes(Attribute::Inverse);
    current_btn.update();
}

std::size_t Menu::size() const {
    return items_.size();
}

bool Menu::key_press_event(const Key::State& keyboard) {
    if (keyboard.key == Key::Arrow_down || keyboard.key == Key::j) {
        this->select_down();
    } else if (keyboard.key == Key::Arrow_up || keyboard.key == Key::k) {
        this->select_up();
    } else if (keyboard.key == Key::Enter) {
        this->call_current_item();
    }
    return true;
}

bool Menu::mouse_press_event(const Mouse::State& mouse) {
    if (mouse.button == Mouse::Button::ScrollUp) {
        this->select_up();
    } else if (mouse.button == Mouse::Button::ScrollDown) {
        this->select_down();
    }
    return layout::Vertical::mouse_press_event(mouse);
}

bool Menu::mouse_press_event_filter(Widget& /* receiver */,
                                    const Mouse::State& mouse) {
    if (mouse.button == Mouse::Button::ScrollUp) {
        this->select_up();
        return true;
    }
    if (mouse.button == Mouse::Button::ScrollDown) {
        this->select_down();
        return true;
    }
    return false;
}

void Menu::call_current_item() {
    if (!items_.empty()) {
        items_[selected_index_].selected();
    }
}

namespace slot {

sig::Slot<void(std::size_t)> select_up(Menu& m) {
    sig::Slot<void(std::size_t)> slot{[&m](auto n) { m.select_up(n); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void()> select_up(Menu& m, std::size_t n) {
    sig::Slot<void()> slot{[&m, n] { m.select_up(n); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> select_down(Menu& m) {
    sig::Slot<void(std::size_t)> slot{[&m](auto n) { m.select_down(n); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void()> select_down(Menu& m, std::size_t n) {
    sig::Slot<void()> slot{[&m, n] { m.select_down(n); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> select_item(Menu& m) {
    sig::Slot<void(std::size_t)> slot{
        [&m](auto index) { m.select_item(index); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void()> select_item(Menu& m, std::size_t index) {
    sig::Slot<void()> slot{[&m, index] { m.select_item(index); }};
    slot.track(m.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
