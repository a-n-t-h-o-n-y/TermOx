#include <string>
#include "cppurses.hpp"
using namespace cppurses;

class click_widget : public Widget {
   public:
    click_widget() {
        this->focus_policy = Focus_policy::Click;
        enable_border(*this);
    }

    bool paint_event() override {
        Painter p{this};
        p.put("X", clicked_.x, clicked_.y);
        return Widget::paint_event();
    }

    bool mouse_press_event(Mouse_button button,
                           std::size_t global_x,
                           std::size_t global_y,
                           std::size_t local_x,
                           std::size_t local_y,
                           std::uint8_t device_id) override {
        clicked_.x = local_x;
        clicked_.y = local_y;
        this->update();
        return Widget::mouse_press_event(button, global_x, global_y, local_x,
                                         local_y, device_id);
    }

   private:
    Coordinates clicked_{0, 0};
};

struct Item {
    std::string name{"Default"};
    int value{5};
    std::string color{"Blue"};
};

int main() {
    System sys;

    // Layouts
    Horizontal_layout layout;
    auto& leftl = layout.make_child<Vertical_layout>();

    // List Left
    auto& list = leftl.make_child<List<Item>>();
    list.add_property("Name", [](const Item& i) { return i.name; });
    list.add_property("Value",
                      [](const Item& i) { return std::to_string(i.value); });
    list.add_property("Color", [](const Item& i) { return i.color; });

    list.add_item(Item{});
    list.add_item(Item{"Item 2", 7, "Green"});
    list.add_item(Item{"Item 2", 7, "Green"});

    // List Right
    auto& list2 = layout.make_child<List<Item>>();
    list2.add_property("Color", [](const Item& i) { return i.color; });
    list2.add_property("Value",
                       [](const Item& i) { return std::to_string(i.value); });
    list2.add_property("Name", [](const Item& i) { return i.name; });
    leftl.find_child<List<Item>>("");

    // Clicked Widget
    auto& clik = layout.make_child<click_widget>();

    // Rotate Button
    auto& rotate_button = leftl.make_child<Push_button>("Rotate Properties");

    // Signals
    list.selected.connect(slot::add_item(list2));
    rotate_button.clicked.connect(slot::rotate_properties(list));

    // Aesthetics
    set_background(list, Color::Light_blue);
    list.height_policy.stretch(5);
    set_background(list2, Color::Light_gray);
    set_background(rotate_button, Color::Gray);
    set_foreground(rotate_button, Color::Violet);

    Focus::set_focus_to(&list);
    sys.set_head(&layout);
    return sys.run();
}
