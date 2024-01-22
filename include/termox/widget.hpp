#pragma once

#include <concepts>

#include <esc/area.hpp>
#include <esc/point.hpp>

#include <termox/glyph.hpp>

namespace ox {

/**
 * A Widget is a graphical element that can be displayed on the screen.
 *
 * Keeping the concept as loose as possible, currently anything with a size and
 * location can be considered a Widget. The event system will check if the
 * Widget has a handle function before implementing a call to the handler, they
 * are not required.
 */
template <typename T>
concept Widget = requires(T t) {
    {
        t.coordinates
    } -> std::same_as<esc::Point&>;
    {
        t.size
    } -> std::same_as<esc::Area&>;
};

/**
 * Checks if a type has a fill Glyph for the background.
 */
template <typename T>
concept HasFillGlyph = Widget<T> && requires(T t) {
    {
        t.fill_glyph
    } -> std::same_as<Glyph&>;
};

}  // namespace ox