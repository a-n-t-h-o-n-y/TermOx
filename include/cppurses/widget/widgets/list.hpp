#ifndef WIDGET_WIDGETS_LIST_HPP
#define WIDGET_WIDGETS_LIST_HPP
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/widget.hpp>

#include <signals/signals.hpp>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>

namespace cppurses {

template <typename T>
class List : public Widget {
   public:
    List();
    void add_property(Glyph_string property_name,
                      std::function<Glyph_string(const T&)> get_value);
    void add_item(const T& item);

    void rotate_properties();

    // Signals
    sig::Signal<void(T&)> selected;

   protected:
    bool paint_event() override;
    bool key_press_event(Key key, char symbol) override;
    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override;

   private:
    struct Property {
        Glyph_string name;
        std::function<Glyph_string(const T&)> get_value;
    };
    std::vector<T> items_;
    std::vector<Property> properties_;
    std::size_t selected_index_{1};

    void select_up(std::size_t n);
    void select_down(std::size_t n);
};

template <typename T>
List<T>::List() {
    this->focus_policy = Focus_policy::Strong;
}

template <typename T>
void List<T>::add_property(Glyph_string property_name,
                           std::function<Glyph_string(const T&)> get_value) {
    properties_.emplace_back(Property{property_name, get_value});
    this->update();
}

template <typename T>
void List<T>::add_item(const T& item) {
    items_.emplace_back(item);
    this->update();
}

template <typename T>
void List<T>::rotate_properties() {
    if (properties_.size() > 1) {
        std::rotate(std::begin(properties_), std::begin(properties_) + 1,
                    std::end(properties_));
    }
    this->update();
}

template <typename T>
bool List<T>::paint_event() {
    Painter p{this};
    p.move_cursor_on_put = true;
    for (const auto& prop : properties_) {
        p.put(Glyph_string{prop.name}.append(" | "));
    }
    std::size_t count{1};
    for (const auto& item : items_) {
        Glyph_string display;
        for (const auto& prop : properties_) {
            display.append(prop.get_value(item));
            display.append(" | ");
        }
        if (count == selected_index_) {
            display.add_attributes(Attribute::Bold);
        }
        p.put(display, 0, count);
        ++count;
    }
    move_cursor(*this, 0, 0);
    return Widget::paint_event();
}

template <typename T>
void List<T>::select_up(std::size_t n) {
    if (selected_index_ > n) {
        selected_index_ -= n;
    }
}

template <typename T>
void List<T>::select_down(std::size_t n) {
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

template <typename T>
bool List<T>::key_press_event(Key key, char symbol) {
    if (key == Key::Arrow_down || key == Key::Arrow_left) {
        this->select_down(1);
    } else if (key == Key::Arrow_up || key == Key::Arrow_right) {
        this->select_up(1);
    } else if (key == Key::Enter) {
        if (!items_.empty()) {
            selected(items_.at(selected_index_ - 1));
        }
    }
    this->update();
    return Widget::key_press_event(key, symbol);
}

template <typename T>
bool List<T>::mouse_press_event(Mouse_button button,
                                std::size_t global_x,
                                std::size_t global_y,
                                std::size_t local_x,
                                std::size_t local_y,
                                std::uint8_t device_id) {
    return Widget::mouse_press_event(button, global_x, global_y, local_x,
                                     local_y, device_id);
}

namespace slot {

template <typename T>
sig::Slot<void(const T&)> add_item(List<T>& list) {
    sig::Slot<void(const T&)> slot{
        [&list](const T& item) { list.add_item(item); }};
    slot.track(list.destroyed);
    return slot;
}

template <typename T>
sig::Slot<void()> add_item(List<T>& list, const T& item) {
    sig::Slot<void()> slot{[&list, item] { list.add_item(item); }};
    slot.track(list.destroyed);
    return slot;
}

template <typename T>
sig::Slot<void()> rotate_properties(List<T>& list) {
    sig::Slot<void()> slot{[&list] { list.rotate_properties(); }};
    slot.track(list.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_LIST_HPP

// Each property has its width that is the length of the longest string.
// values of properties should be centered or have the option of left/right alin
// Some kind of table widget that you could use, that provides all the logic,
// you just add the text.
