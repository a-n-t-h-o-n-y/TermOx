#include <cppurses/painter/brush.hpp>

#include <cstddef>
#include <cstdint>

#include <optional/optional.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/color.hpp>

namespace {
using opt::Optional;
using namespace cppurses;

void add_background(const Brush& from, Brush& to) {
    if (!to.background_color() && from.background_color()) {
        to.add_attributes(background(*from.background_color()));
    }
}

void add_foreground(const Brush& from, Brush& to) {
    if (!to.foreground_color() && from.foreground_color()) {
        to.add_attributes(foreground(*from.foreground_color()));
    }
}

void copy_attributes(const Brush& from, Brush& to) {
    for (Attribute a : Attribute_list) {
        if (from.has_attribute(a)) {
            to.add_attributes(a);
        }
    }
}

}  // namespace

namespace cppurses {

bool operator==(const Brush& lhs, const Brush& rhs) {
    return (lhs.attributes_ == rhs.attributes_ &&
            lhs.background_color_ == rhs.background_color_ &&
            lhs.foreground_color_ == rhs.foreground_color_);
}

void imprint(const Brush& from, Brush& to) {
    add_background(from, to);
    add_foreground(from, to);
    copy_attributes(from, to);
}

}  // namespace cppurses
