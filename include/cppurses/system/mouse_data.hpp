#ifndef CPPURSES_SYSTEM_MOUSE_DATA_HPP
#define CPPURSES_SYSTEM_MOUSE_DATA_HPP
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {

struct Mouse_data {
    Mouse_button button;
    Point global;
    Point local;
    short device_id;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_MOUSE_DATA_HPP
