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

    Horizontal_layout layout;
    auto& list = layout.make_child<List<Item>>();
    list.add_property("Name", [](const Item& i) { return i.name; });
    list.add_property("Value",
                      [](const Item& i) { return std::to_string(i.value); });
    list.add_property("Color", [](const Item& i) { return i.color; });

    sys.set_head(&layout);
    return sys.run();
}
