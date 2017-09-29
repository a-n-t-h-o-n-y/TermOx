#include <string>
#include "cppurses.hpp"

struct Item {
    std::string name{"Default"};
    int value{5};
    std::string color{"Blue"};
};

using namespace cppurses;

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

    // Rotate Button
    auto& rotate_button = leftl.make_child<Push_button>("Rotate Properties");

    // Signals
    list.selected.connect(slot::add_item(list2));
    rotate_button.clicked.connect(slot::rotate_properties(list));

    // Aesthetics
    list.set_background(Color::Light_blue);
    list.height_policy.stretch(5);
    list2.set_background(Color::Light_gray);
    rotate_button.set_background(Color::Gray);
    rotate_button.set_foreground(Color::Violet);

    Focus::set_focus_to(&list);
    sys.set_head(&layout);
    return sys.run();
}
