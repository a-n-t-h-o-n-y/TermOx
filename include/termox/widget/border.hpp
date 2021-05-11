#ifndef TERMOX_WIDGET_BORDER_HPP
#define TERMOX_WIDGET_BORDER_HPP
#include <termox/painter/glyph.hpp>

namespace ox {

/// Provides representation of a Widget's visible, surrounding border.
class Border {
   public:
    /// Holds enabled state and appearance for a Border Segment.
    class Segment : public Glyph {
       public:
        /// Construct a Border Segment with \g for appearance.
        constexpr Segment(Glyph g) : Glyph{g} {}

        /// Enable the Segment to be displayable.
        void constexpr enable() { enabled_ = true; }

        /// Disable the Segment, making it non-displayable.
        void constexpr disable() { enabled_ = false; }

        /// Return whether or not the border is enabled.
        [[nodiscard]] auto constexpr is_enabled() const -> bool
        {
            return enabled_;
        }

        /// Can assign to this as if it is a Glyph.
        using Glyph::operator=;

       private:
        bool enabled_ = true;
    };

    /// Holds each of the 8 Segments of a Border.
    struct Segments {
        Segment north      = Glyph{U'─'};
        Segment south      = Glyph{U'─'};
        Segment east       = Glyph{U'│'};
        Segment west       = Glyph{U'│'};
        Segment north_west = Glyph{U'┌'};
        Segment north_east = Glyph{U'┐'};
        Segment south_west = Glyph{U'└'};
        Segment south_east = Glyph{U'┘'};
    };

   public:
    /// Contains the look/appearance and enabled state for each segment.
    Segments segments;

   public:
    /// Enable the Border.
    /** This will give it space within its Widget and make it displayable.
     *  Segments are only displayed if their Border is enabled. */
    void constexpr enable() { enabled_ = true; }

    /// Disable the Border.
    void constexpr disable() { enabled_ = false; }

    /// Return whether the border is enabled.
    [[nodiscard]] auto constexpr is_enabled() const -> bool { return enabled_; }

   private:
    bool enabled_ = false;
};

/// Disable all Segments.
void constexpr disable_all(Border::Segments& x)
{
    x.north.disable();
    x.south.disable();
    x.east.disable();
    x.west.disable();
    x.north_west.disable();
    x.north_east.disable();
    x.south_west.disable();
    x.south_east.disable();
}

/// Enable all corners.
/** Corners are north_west, north_east, south_west, and south_east. */
void constexpr enable_corners(Border::Segments& x)
{
    x.north_west.enable();
    x.north_east.enable();
    x.south_west.enable();
    x.south_east.enable();
}

/// Disable all corners.
/** Corners are north_west, north_east, south_west, and south_east. */
void constexpr disable_corners(Border::Segments& x)
{
    x.north_west.disable();
    x.north_east.disable();
    x.south_west.disable();
    x.south_east.disable();
}

/// Enable all sides.
/** Sides are north, south, east, and west. */
void constexpr enable_sides(Border::Segments& x)
{
    x.north.enable();
    x.south.enable();
    x.east.enable();
    x.west.enable();
}

/// Disable all sides.
/** Sides are north, south, east, and west. */
void constexpr disable_sides(Border::Segments& x)
{
    x.north.disable();
    x.south.disable();
    x.east.disable();
    x.west.disable();
}

}  // namespace ox
#endif  // TERMOX_WIDGET_BORDER_HPP
