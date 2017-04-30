#ifndef ATTRIBUTE_HPP
#define ATTRIBUTE_HPP

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
#endif  // ATTRIBUTE_HPP
