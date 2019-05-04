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

Menu::Menu(Glyph_string title_text)
    : title{this->make_child<Label>(std::move(title_text))} {
    this->focus_policy = Focus_policy::Strong;
    title.set_alignment(Alignment::Center);
    title.brush.add_attributes(Attribute::Bold);
    line_break.wallpaper = L'─';
}

sig::Signal<void()>& Menu::append_item(Glyph_string label) {
    return this->insert_item(std::move(label), this->size());
}

sig::Signal<void()>& Menu::insert_item(Glyph_string label, std::size_t index) {
    auto button_ptr = std::make_unique<Push_button>(std::move(label));
    Push_button& new_button = *button_ptr;
    this->children.insert(std::move(button_ptr), index + 2);
    items_.emplace(std::begin(items_) + index, new_button);
    new_button.install_event_filter(*this);
    new_button.height_policy.fixed(1);

    if (items_.size() == 1) {
        this->select_item(0);
    }
    auto& signal_ref = items_[index].selected;
    new_button.clicked.connect([this, index] {
        this->select_item(index);
        this->send_selected_signal();
    });
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
}

void Menu::select_up(std::size_t n) {
    const auto new_index = selected_index_ > n ? selected_index_ - n : 0;
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
    previous_btn.brush.remove_attributes(selected_attr_);
    previous_btn.update();

    selected_index_ = index >= items_.size() ? items_.size() - 1 : index;

    auto& current_btn = items_[selected_index_].button.get();
    current_btn.brush.add_attributes(selected_attr_);
    current_btn.update();
}

void Menu::set_selected_attribute(const Attribute& attr) {
    auto& selected_btn = items_[selected_index_].button.get();
    selected_btn.brush.remove_attributes(selected_attr_);
    selected_attr_ = attr;
    selected_btn.brush.add_attributes(selected_attr_);
    selected_btn.update();
}

void Menu::hide_title() {
    title_enabled_ = false;
    this->enable(this->enabled());
}

void Menu::show_title() {
    title_enabled_ = true;
    this->enable(this->enabled());
}

void Menu::hide_line_break() {
    line_break_enabled_ = false;
    this->enable(this->enabled());
}

void Menu::show_line_break() {
    line_break_enabled_ = true;
    this->enable(this->enabled());
}

void Menu::enable(bool enable, bool post_child_polished_event) {
    this->enable_and_post_events(enable, post_child_polished_event);
    line_break.enable(line_break_enabled_ && enable, post_child_polished_event);
    title.enable(title_enabled_ && enable, post_child_polished_event);
    for (Menu_item& item : items_) {
        item.button.get().enable(enable, post_child_polished_event);
    }
}

bool Menu::key_press_event(const Key::State& keyboard) {
    if (keyboard.key == Key::Arrow_down || keyboard.key == Key::j) {
        this->select_down();
    } else if (keyboard.key == Key::Arrow_up || keyboard.key == Key::k) {
        this->select_up();
    } else if (keyboard.key == Key::Enter) {
        this->send_selected_signal();
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
    } else if (mouse.button == Mouse::Button::ScrollDown) {
        this->select_down();
        return true;
    }
    return false;
}

void Menu::send_selected_signal() {
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
