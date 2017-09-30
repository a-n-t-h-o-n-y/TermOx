#include "widget/widgets/menu.hpp"
#include <signals/signals.hpp>
#include <string>
#include <utility>
#include "painter/glyph_string.hpp"
#include "painter/painter.hpp"
#include "widget/focus_policy.hpp"

namespace cppurses {

Menu::Menu(Glyph_string title) : menu_title_{std::move(title)} {
    this->focus_policy = Focus_policy::Strong;
}

void Menu::add_item(Menu_item item) {
    items_.emplace_back(std::move(item));
}

void Menu::make_item(Glyph_string title, sig::Slot<void()> action) {
    items_.emplace_back(Menu_item{std::move(title), std::move(action)});
}

void Menu::remove_item(std::size_t index) {
    if (index > items_.size() || index < 1) {
        return;
    }
    items_.erase(std::begin(items_) + index - 1);
}

void Menu::select_up(std::size_t n) {
    if (selected_index_ > n) {
        selected_index_ -= n;
    }
}

void Menu::select_down(std::size_t n) {
    if (items_.empty()) {
        return;
    }
    if (selected_index_ != items_.size()) {
        selected_index_ += n;
    }
    if (selected_index_ > items_.size()) {
        selected_index_ = items_.size();
    }
}

void Menu::select_item(std::size_t index) {
    if (items_.empty()) {
        return;
    }
    if (index > items_.size()) {
        selected_index_ = items_.size();
    } else if (index < 1) {
        selected_index_ = 1;
    } else {
        selected_index_ = index;
    }
}

std::size_t Menu::size() const {
    return items_.size();
}

bool Menu::paint_event() {
    Painter p{this};
    p.put(menu_title_);
    std::size_t count{1};
    for (const auto& item : items_) {
        Glyph_string display{Glyph_string{std::to_string(count) + ". "} +
                             item.title};
        if (count == selected_index_) {
            display.add_attributes(Attribute::Bold);
        }
        p.put(display, 0, count);
        ++count;
    }
    return Widget::paint_event();
}

bool Menu::key_press_event(Key key, char symbol) {
    if (key == Key::Arrow_down || key == Key::Arrow_left) {
        this->select_down();
    } else if (key == Key::Arrow_up || key == Key::Arrow_right) {
        this->select_up();
    } else if (key == Key::Enter) {
        if (!items_.empty()) {
            items_.at(selected_index_ - 1).action();
        }
    }
    this->update();
    return true;
}

namespace slot {

sig::Slot<void(Menu_item)> add_item(Menu& m) {
    sig::Slot<void(Menu_item)> slot{
        [&m](auto item) { m.add_item(std::move(item)); }};
    slot.track(m.destroyed);
    return slot;
}

// TODO: Generalized lambda capture of item with std::move() and parameter
// Menu_item by value. Below as well.  Once Clang-Format properly handles this.
sig::Slot<void()> add_item(Menu& m, const Menu_item& item) {
    sig::Slot<void()> slot{[&m, item] { m.add_item(item); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void(Glyph_string, sig::Slot<void()>)> make_item(Menu& m) {
    sig::Slot<void(Glyph_string, sig::Slot<void()>)> slot{
        [&m](auto title, auto action) {
            m.make_item(std::move(title), std::move(action));
        }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void(Glyph_string)> make_item(Menu& m,
                                        const sig::Slot<void()>& action) {
    sig::Slot<void(Glyph_string)> slot{[&m, action](auto title) {
        m.make_item(std::move(title), std::move(action));
    }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void(sig::Slot<void()>)> make_item(Menu& m,
                                             const Glyph_string& title) {
    sig::Slot<void(sig::Slot<void()>)> slot{[&m, title](auto action) {
        m.make_item(std::move(title), std::move(action));
    }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void()> make_item(Menu& m,
                            const Glyph_string& title,
                            const sig::Slot<void()>& action) {
    sig::Slot<void()> slot{[&m, title, action] { m.make_item(title, action); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void(std::size_t)> remove_item(Menu& m) {
    sig::Slot<void(std::size_t)> slot{
        [&m](auto index) { m.remove_item(index); }};
    slot.track(m.destroyed);
    return slot;
}

sig::Slot<void()> remove_item(Menu& m, std::size_t index) {
    sig::Slot<void()> slot{[&m, index] { m.remove_item(index); }};
    slot.track(m.destroyed);
    return slot;
}

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
