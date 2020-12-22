#ifndef TERMOX_PAINTER_TRAIT_HPP
#define TERMOX_PAINTER_TRAIT_HPP
#include <stdexcept>
#include <string>

namespace ox {

/// Attributes that can be applied to alter the appearance of a Glyph object.
enum class Trait : unsigned char {
    Bold,
    Italic,
    Underline,
    Standout,
    Dim,
    Inverse,
    Invisible,
    Blink
};

/// Number of Traits in enum, use to iterate from [0, Trait_count).
auto constexpr Trait_count = 8;

inline auto to_string(Trait t) -> std::string
{
    switch (t) {
        case Trait::Bold: return "Bold";
        case Trait::Italic: return "Italic";
        case Trait::Underline: return "Underline";
        case Trait::Standout: return "Standout";
        case Trait::Dim: return "Dim";
        case Trait::Inverse: return "Inverse";
        case Trait::Invisible: return "Invisible";
        case Trait::Blink: return "Blink";
        default: break;
    }
    throw std::runtime_error{"to_string(Trait): Invalid Trait"};
}

}  // namespace ox
#endif  // TERMOX_PAINTER_TRAIT_HPP
