#pragma once

#include <cstddef>
#include <vector>

#include <esc/area.hpp>

#include <termox/glyph.hpp>
#include <termox/widget.hpp>

namespace ox {

/**
 * @brief A 2D Matrix of Glyphs that represents a paintable screen.
 */
class ScreenBuffer {
   public:
    /**
     * @brief Construct a ScreenBuffer with the given dimensions.
     *
     * @param area The dimensions of the ScreenBuffer.
     */
    explicit ScreenBuffer(esc::Area area)
        : area_{area}, buffer_((std::size_t)(area.width * area.height))
    {}

   public:
    /**
     * @brief Access the Glyph at the given position.
     *
     * The top left is {0, 0} and the bottom right is {width - 1, height - 1}.
     * Does no bounds checking.
     *
     * @param p The Point position of the Glyph.
     * @return Glyph& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](esc::Point p) -> Glyph&
    {
        return buffer_[(std::size_t)(p.y * area_.width + p.x)];
    }

    /**
     * @brief Access the Glyph at the given position.
     *
     * The top left is {0, 0} and the bottom right is {width - 1, height - 1}.
     * Does no bounds checking.
     *
     * @param p The Point position of the Glyph.
     * @return Glyph const& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](esc::Point p) const -> Glyph const&
    {
        return buffer_[(std::size_t)(p.y * area_.width + p.x)];
    }

    /**
     * @brief Resize the ScreenBuffer to the given dimensions and clears state.
     *
     * This does not preserve the contents of the ScreenBuffer, it overwrites
     * all Glyph symbols with null bytes.
     *
     * This function is provided as an optimized alternative to constructing
     * a new ScreenBuffer and moving it into the previous one. This minimizes
     * the vector allocations by re-using the existing buffer.
     *
     * @param a The new dimensions of the ScreenBuffer.
     */
    auto reset(esc::Area a) -> void
    {
        area_ = a;
        buffer_.resize((std::size_t)(a.width * a.height));
        this->clear();
    }

    /**
     * @brief Clear the ScreenBuffer, setting all Glyphs symbols to null.
     *
     * This does not reset their Brush members, a null symbol is 'empty'.
     */
    auto clear() -> void
    {
        for (auto& g : buffer_) {
            g.symbol = U'\0';
        }
    }

   private:
    esc::Area area_;
    std::vector<Glyph> buffer_;
};

class Terminal {
   public:
    inline static ScreenBuffer next_screen{{0, 0}};
    inline static ScreenBuffer current_screen{{0, 0}};

   public:
    // TODO probably just provide an initialize() member function instead of a
    // constructor, since everything is static.
    Terminal()
    {
        // TODO get current screen size and set screen buffers to that size.

        // TODO does this automatically initialze the terminal or is that a
        // separate explicitly called member function? and if so then you should
        // resize the screen buffers there.
    }

   private:
};

/**
 * @brief A 2D Matrix of Glyphs that represents a paintable area of a Widget.
 *
 * This is a non-owning view into a global ScreenBuffer, it is not a copy of the
 * data. This allows Widgets to write directly to the Terminal's next_screen
 * ScreenBuffer.
 *
 * A Widget should create a Canvas object whenever it wants to update it's
 * appearance and return it via an event handler.
 */
class Canvas {
   public:
    /**
     * @brief Construct a Canvas with the given Widget's coordinates and size.
     *
     * @param widget The Widget to create a Canvas for.
     */
    template <Widget T>
    explicit Canvas(T& widget)
        : offset_{widget.coordinates},
          size_{widget.size},
          screen_{Terminal::next_screen}
    {
        // TODO fill with wallpaper if widget has wallpaper?
    }

   public:
    /**
     * @brief Access the Glyph at the given position, offset for Widget.
     *
     * The top left is {0, 0} and the bottom right is {width - 1, height - 1}.
     * Does no bounds checking.
     *
     * @param p The Point position of the Glyph.
     * @return Glyph& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](esc::Point p) -> Glyph&
    {
        return screen_[{p.x + offset_.x, p.y + offset_.y}];
    }

    /**
     * @brief Access the Glyph at the given position, offset for Widget.
     *
     * The top left is {0, 0} and the bottom right is {width - 1, height - 1}.
     * Does no bounds checking.
     *
     * @param p The Point position of the Glyph.
     * @return Glyph const& A reference to the Glyph at the given position.
     */
    [[nodiscard]] auto operator[](esc::Point p) const -> Glyph const&
    {
        return screen_[{p.x + offset_.x, p.y + offset_.y}];
    }

    /**
     * @brief Clear the Canvas, setting all Glyphs symbols to null.
     *
     * This does not reset their Brush members, a null symbol is 'empty'.
     */
    auto clear() -> void
    {
        for (auto x = offset_.x; x < offset_.x + size_.width; ++x) {
            for (auto y = offset_.y; y < offset_.y + size_.height; ++y) {
                screen_[{x, y}].symbol = U'\0';
            }
        }
    }

    /**
     * @brief Fill the Canvas with the given Glyph.
     *
     * @param g The Glyph to fill the Canvas with.
     */
    auto fill(Glyph const& g) -> void
    {
        for (auto x = offset_.x; x < offset_.x + size_.width; ++x) {
            for (auto y = offset_.y; y < offset_.y + size_.height; ++y) {
                screen_[{x, y}] = g;
            }
        }
    }

    // TODO maybe a border() drawing function? You'll have to handle smaller
    // dimensions yourself in the widget then.

   private:
    esc::Point offset_;
    esc::Area size_;
    ScreenBuffer& screen_;
};

}  // namespace ox