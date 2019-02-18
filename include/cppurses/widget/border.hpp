#ifndef CPPURSES_WIDGET_BORDER_HPP
#define CPPURSES_WIDGET_BORDER_HPP
#include <cppurses/painter/glyph.hpp>

namespace cppurses {

/// Provides representation of a Widget's visible, surrounding border.
class Border {
   public:
    /// Holds enabled state and appearance for a Border Segment.
    class Segment : public Glyph {
       public:
        /// Construct a Border Segment with \g for appearance.
        Segment(const Glyph& g) : Glyph{g} {}

        /// Enable the Segment to be displayable.
        void enable() { enabled_ = true; }

        /// Disable the Segment, making it non-displayable.
        void disable() { enabled_ = false; }

        /// Return whether or not the border is enabled.
        bool enabled() const { return enabled_; }

        /// Can assign to this as if it is a Glyph.
        using Glyph::operator=;

       private:
        bool enabled_{true};
    };

    /// Holds all 8 Segments that a Border is made up of.
    struct Segments {
        Segment north{L'─'};
        Segment south{L'─'};
        Segment east{L'│'};
        Segment west{L'│'};
        Segment north_west{L'┌'};
        Segment north_east{L'┐'};
        Segment south_west{L'└'};
        Segment south_east{L'┘'};

        /// Enable all Segments, allowing them to be displayed.
        void enable_all();

        /// Disable all Segments.
        void disable_all();

        /// Enable all corners.
        /** Corners are north_west, north_east, south_west, and south_east. */
        void enable_corners();

        /// Disable all corners.
        /** Corners are north_west, north_east, south_west, and south_east. */
        void disable_corners();

        /// Enable all sides.
        /** Sides are north, south, east, and west. */
        void enable_sides();

        /// Disable all sides.
        /** Sides are north, south, east, and west. */
        void disable_sides();
    };

    /// Contains the look/appearance and enabled state for each segment.
    Segments segments;

    /// Enable the Border.
    /** This will give it space within its Widget and make it displayable.
     *  Segments are only displayed if their Border is enabled. */
    void enable() { enabled_ = true; }

    /// Disable the Border.
    void disable() { enabled_ = false; }

    /// Return whether the border is enabled.
    bool enabled() const { return enabled_; }

   private:
    bool enabled_{false};
};
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_BORDER_HPP
