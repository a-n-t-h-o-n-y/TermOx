#ifndef PAINTER_MODULE_ATTRIBUTE_HPP
#define PAINTER_MODULE_ATTRIBUTE_HPP

#include <cstdint>

namespace twf {

enum class Attribute : std::int8_t {
    Bold,
    Italic,
    Underline,
    Standout,
    Dim,
    Inverse,
    Invisible,
    Blink
};

}  // namespace twf
#endif  // PAINTER_MODULE_ATTRIBUTE_HPP
