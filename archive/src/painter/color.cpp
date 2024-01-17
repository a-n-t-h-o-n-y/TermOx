#include <termox/painter/color.hpp>

#include <string>

namespace ox {

auto color_to_string(Color c) -> std::string
{
    switch (c.value) {
        case 0: return "Black";
        case 1: return "Dark Red";
        case 2: return "Green";
        case 3: return "Brown";
        case 4: return "Dark Blue";
        case 5: return "Violet";
        case 6: return "Light Blue";
        case 7: return "Light Gray";
        case 8: return "Dark Gray";
        case 9: return "Red";
        case 10: return "Light Green";
        case 11: return "Yellow";
        case 12: return "Blue";
        case 13: return "Orange";
        case 14: return "Gray";
        case 15: return "White";
        default: return "";
    }
}
}  // namespace ox
