#pragma once

#include <array>
#include <string_view>

#include <ox/core/core.hpp>

namespace ox::shape {

struct HLine {
    int length;
    char32_t symbol = U'─';
    Color foreground = XColor::Default;
};

struct VLine {
    int length;
    char32_t symbol = U'│';
    Color foreground = XColor::Default;
};

struct Box {
    /// {top left, top right, bottom left, bottom right}
    std::array<char32_t, 4> corners = {U'┌', U'┐', U'└', U'┘'};

    /// {N, S, E, W}
    std::array<char32_t, 4> walls = {U'─', U'─', U'│', U'│'};

    Color foreground = XColor::Default;
    Area size;
};

struct Fill {
    Glyph glyph;
    Area size;
};

struct Corners {
    Color foreground = XColor::Default;
};

struct CornersRound {
    Color foreground = XColor::Default;
};

struct Frame {
    Color foreground = XColor::Default;
};

struct FrameRound {
    Color foreground = XColor::Default;
};

struct FrameBold {
    Color foreground = XColor::Default;
};

struct FrameTwin {
    Color foreground = XColor::Default;
};

struct HalfFrame {
    Color foreground = XColor::Default;
};

struct HalfFrameRound {
    Color foreground = XColor::Default;
};

struct HalfFrameBold {
    Color foreground = XColor::Default;
};

struct HalfFrameTwin {
    Color foreground = XColor::Default;
};

struct Brackets {
    Color foreground = XColor::Default;
};

struct BracketsRound {
    Color foreground = XColor::Default;
};

struct BracketsBold {
    Color foreground = XColor::Default;
};

struct BracketsTwin {
    Color foreground = XColor::Default;
};

}  // namespace ox::shape

namespace ox {

/**
 * Put a single Glyph at the given position on the Canvas.
 * @details Does bounds checking and is no-op if \p at is out of bounds.
 */
void put(Canvas& c, Point at, Glyph const& item);

/**
 * Put a single character type to the Canvas at the given position.
 * @details Uses the default Brush and the given character.
 */
void put(Canvas& c, Point at, Character auto item)
{
    put(c, at, Glyph{.symbol = static_cast<char32_t>(item)});
}

void put(Canvas& c, Point at, GlyphString auto const& item)
{
    if (at.y >= c.size.height) { return; }

    for (auto const& g : item) {
        if (at.x >= c.size.width) { break; }
        c[at] = g;
        ++at.x;
    }
}

void put(Canvas& c, Point at, std::string_view element);

void put(Canvas& c, Point at, std::u32string_view element);

void put(Canvas& c, Point at, shape::HLine const& item);

void put(Canvas& c, Point at, shape::VLine const& item);

void put(Canvas& c, Point at, shape::Box const& item);

void put(Canvas& c, Point at, shape::Fill const& item);

void put(Canvas& c, shape::Corners const& item);

void put(Canvas& c, shape::CornersRound const& item);

void put(Canvas& c, shape::Frame const& item);

void put(Canvas& c, shape::FrameRound const& item);

void put(Canvas& c, shape::FrameBold const& item);

void put(Canvas& c, shape::FrameTwin const& item);

void put(Canvas& c, shape::HalfFrame const& item);

void put(Canvas& c, shape::HalfFrameRound const& item);

void put(Canvas& c, shape::HalfFrameBold const& item);

void put(Canvas& c, shape::HalfFrameTwin const& item);

void put(Canvas& c, shape::Brackets const& item);

void put(Canvas& c, shape::BracketsRound const& item);

void put(Canvas& c, shape::BracketsBold const& item);

void put(Canvas& c, shape::BracketsTwin const& item);

void clear(Canvas& c);

}  // namespace ox
