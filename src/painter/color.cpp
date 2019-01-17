#include <cppurses/painter/color.hpp>

#include <string>

namespace cppurses {

std::string color_to_string(Color c) {
    std::string name{"Error"};
    switch (c) {
        case Color::Black:
            name = "Black";
            break;
        case Color::Dark_red:
            name = "Dark Red";
            break;
        case Color::Dark_blue:
            name = "Dark Blue";
            break;
        case Color::Dark_gray:
            name = "Dark Gray";
            break;
        case Color::Brown:
            name = "Brown";
            break;
        case Color::Green:
            name = "Green";
            break;
        case Color::Red:
            name = "Red";
            break;
        case Color::Gray:
            name = "Gray";
            break;
        case Color::Blue:
            name = "Blue";
            break;
        case Color::Orange:
            name = "Orange";
            break;
        case Color::Light_gray:
            name = "Light Gray";
            break;
        case Color::Light_green:
            name = "Light Green";
            break;
        case Color::Violet:
            name = "Violet";
            break;
        case Color::Light_blue:
            name = "Light Blue";
            break;
        case Color::Yellow:
            name = "Yellow";
            break;
        case Color::White:
            name = "White";
            break;
    }
    return name;
}

}  // namespace cppurses
