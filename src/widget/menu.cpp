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
#include <cppurses/widget/widgets/button.hpp>
#include <cppurses/widget/widgets/label.hpp>

namespace cppurses {

Menu::Menu(Glyph_string title_text)
    : title{this->make_child<Label>(std::move(title_text))}
{
    this->focus_policy = Focus_policy::Strong;
    title.set_alignment(Alignment::Center);
    title.brush.add_attributes(Attribute::Bold);
    line_break.set_wallpaper(L'─');
}

auto Menu::insert_item(Glyph_string label, std::size_t index)
    -> sig::Signal<void()>&
{
    auto button_ptr    = std::make_unique<Button>(std::move(label));
    Button& new_button = *button_ptr;
    this->insert_child(std::move(button_ptr), index + 2);
    items_.emplace(std::begin(items_) + index, new_button);
    new_button.install_event_filter(*this);
    new_button.height_policy.fixed(1);

    if (items_.size() == 1)
        this->select_item(0);
    new_button.clicked.connect([this, index] {
        this->select_item(index);
        this->send_selected_signal();
    });
    return items_[index].selected;
}

void Menu::remove_item(std::size_t index)
{
    if (index >= items_.size())
        return;
    items_[index].button.get().close();
    items_.erase(std::begin(items_) + index);
    if (index == selected_index_)
        this->select_item(0);
}

void Menu::select_item(std::size_t index)
{
    if (items_.empty())
        return;
    auto& previous_btn = items_[selected_index_].button.get();
    previous_btn.brush.remove_attributes(selected_attr_);
    previous_btn.update();

    selected_index_ = index >= items_.size() ? items_.size() - 1 : index;

    auto& current_btn = items_[selected_index_].button.get();
    current_btn.brush.add_attributes(selected_attr_);
    current_btn.update();
}

void Menu::set_selected_attribute(Attribute const& attr)
{
    auto& selected_btn = items_[selected_index_].button.get();
    selected_btn.brush.remove_attributes(selected_attr_);
    selected_attr_ = attr;
    selected_btn.brush.add_attributes(selected_attr_);
    selected_btn.update();
}

void Menu::enable(bool enable, bool post_child_polished_event)
{
    this->enable_and_post_events(enable, post_child_polished_event);
    line_break.enable(line_break_enabled_ and enable,
                      post_child_polished_event);
    title.enable(title_enabled_ and enable, post_child_polished_event);
    for (Menu_item& item : items_) {
        item.button.get().enable(enable, post_child_polished_event);
    }
}

auto Menu::key_press_event(Key::State const& keyboard) -> bool
{
    switch (keyboard.key) {
        case Key::Arrow_down:
        case Key::j: this->select_down(); break;
        case Key::Arrow_up:
        case Key::k: this->select_up(); break;
        case Key::Enter: this->send_selected_signal();
        default: break;
    }
    return layout::Vertical<>::key_press_event(keyboard);
}

auto Menu::mouse_wheel_event(Mouse::State const& mouse) -> bool
{
    switch (mouse.button) {
        case Mouse::Button::ScrollUp: this->select_up(); break;
        case Mouse::Button::ScrollDown: this->select_down(); break;
        default: break;
    }
    return layout::Vertical<>::mouse_wheel_event(mouse);
}

auto Menu::mouse_wheel_event_filter(Widget& /* receiver */,
                                    Mouse::State const& mouse) -> bool
{
    switch (mouse.button) {
        case Mouse::Button::ScrollUp: this->select_up(); return true;
        case Mouse::Button::ScrollDown: this->select_down(); return true;
        default: return false;
    }
}

namespace slot {

auto select_up(Menu& m) -> sig::Slot<void(std::size_t)>
{
    auto slot = sig::Slot<void(std::size_t)>{[&m](auto n) { m.select_up(n); }};
    slot.track(m.destroyed);
    return slot;
}

auto select_up(Menu& m, std::size_t n) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&m, n] { m.select_up(n); }};
    slot.track(m.destroyed);
    return slot;
}

auto select_down(Menu& m) -> sig::Slot<void(std::size_t)>
{
    auto slot =
        sig::Slot<void(std::size_t)>{[&m](auto n) { m.select_down(n); }};
    slot.track(m.destroyed);
    return slot;
}

auto select_down(Menu& m, std::size_t n) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&m, n] { m.select_down(n); }};
    slot.track(m.destroyed);
    return slot;
}

auto select_item(Menu& m) -> sig::Slot<void(std::size_t)>
{
    auto slot = sig::Slot<void(std::size_t)>{
        [&m](auto index) { m.select_item(index); }};
    slot.track(m.destroyed);
    return slot;
}

auto select_item(Menu& m, std::size_t index) -> sig::Slot<void()>
{
    auto slot = sig::Slot<void()>{[&m, index] { m.select_item(index); }};
    slot.track(m.destroyed);
    return slot;
}

}  // namespace slot
}  // namespace cppurses
