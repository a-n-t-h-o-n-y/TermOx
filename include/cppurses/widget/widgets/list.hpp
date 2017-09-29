#ifndef WIDGET_WIDGETS_LIST_HPP
#define WIDGET_WIDGETS_LIST_HPP
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <vector>
#include "painter/glyph_string.hpp"
#include "system/events/mouse_event.hpp"
#include "system/key.hpp"
#include "widget/widget.hpp"

namespace cppurses {

template <typename T>
class List : public Widget {
   public:
    void add_property(Glyph_string property_name,
                      std::function<Glyph_string(const T&)> get_value);
    void add_item(const T& item);

    void rotate_properties();

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
};

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
    // std::size_t count{1};
    // for (const auto& item : items_) {
    //     Glyph_string display;
    //     if (count == selected_index_) {
    //         display.add_attributes(Attribute::Bold);
    //     }
    //     p.put(display, 0, count);
    //     ++count;
    // }
    this->move_cursor(0, 0);
    return Widget::paint_event();
}

template <typename T>
bool List<T>::key_press_event(Key key, char symbol) {
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

}  // namespace cppurses

#endif  // WIDGET_WIDGETS_LIST_HPP
