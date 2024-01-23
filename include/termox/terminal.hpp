#pragma once

#include <cassert>
#include <concepts>
#include <cstddef>
#include <optional>
#include <stop_token>
#include <string>
#include <vector>

#include <esc/area.hpp>
#include <esc/event.hpp>
#include <esc/io.hpp>
#include <esc/point.hpp>
#include <esc/sequence.hpp>
#include <esc/terminal.hpp>

#include <termox/common.hpp>
#include <termox/glyph.hpp>

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

    /**
     * @brief Return the dimensions of the ScreenBuffer.
     *
     * @return esc::Area The dimensions of the ScreenBuffer.
     */
    [[nodiscard]] auto area() const -> esc::Area { return area_; }

   private:
    esc::Area area_;
    std::vector<Glyph> buffer_;
};

class Terminal {
   public:
    inline static ScreenBuffer changes{{0, 0}};
    inline static ScreenBuffer current_screen{{0, 0}};

    /**
     * @brief The current cursor position on the terminal.
     *
     * This is used by the event loop after `changes` has been committed to the
     * terminal. If this is std::nullopt, then the cursor is not displayed.
     */
    inline static std::optional<esc::Point> cursor{std::nullopt};

   public:
    /**
     * @brief Write changes ScreenBuffer to the terminal and update
     * current_screen.
     *
     * This is called automatically by the Application class after an event has
     * been processed.
     */
    static auto commit_changes() -> void
    {
        escape_sequence_.clear();
        assert(changes.area() == current_screen.area());

        for (auto x = 0; x < changes.area().width; ++x) {
            for (auto y = 0; y < changes.area().height; ++y) {
                auto const& change  = changes[{x, y}];
                auto const& current = current_screen[{x, y}];
                if (change.symbol != U'\0' && change != current) {
                    escape_sequence_ += esc::escape(esc::Cursor_position{x, y});
                    escape_sequence_ += esc::escape(change.brush);
                    escape_sequence_ += ::ox::u32_to_mb(change.symbol);
                    current_screen[{x, y}] = change;
                }
            }
        }

        esc::write(escape_sequence_);

        if (cursor.has_value()) {
            esc::set(esc::Cursor::Show);
            esc::write(esc::escape(esc::Cursor_position{*cursor}));
        }
        else {
            esc::set(esc::Cursor::Hide);
        }

        esc::flush();
    }

    /**
     * @brief Runs a loop that reads input from the terminal and appends it to
     * the given queue. Exits when the stop_token is stop_requested().
     *
     * @param st The stop_token to check for stop_requested().
     * @param queue The queue to append input events to, must have append(Event)
     */
    template <typename QueueType>
    static auto run_read_loop(std::stop_token st, QueueType& queue) -> void
    {
        queue.append(esc::Window_resize{Terminal::area()});

        while (!st.stop_requested()) {
            if (auto const event = esc::read(16); event.has_value()) {
                queue.append(std::visit(
                    [](auto const& e) -> QueueType::value_type { return e; },
                    *event));
            }
        }
    }

    /**
     * @brief Return the current dimensions of the terminal.
     *
     * @return esc::Area The current dimensions of the terminal.
     */
    [[nodiscard]] static auto area() -> esc::Area
    {
        return esc::terminal_area();
    }

    // TODO other terminal specific settings, like cursor visibility, etc.
    // OS Signal handling? could emit signals? Not sure if that's a good idea.

   private:
    inline static std::string escape_sequence_;
};

/**
 * @brief A 2D Rectangle that represents a paintable area on the terminal with
 * location and size.
 */
template <typename T>
concept Canvas = requires(T t) {
    {
        t.coordinates
    } -> std::same_as<esc::Point&>;
    {
        t.size
    } -> std::same_as<esc::Area&>;
};

/**
 * @brief Provides a way to draw to the terminal screen.
 *
 * This writes directly to the global ScreenBuffer. It can be created from
 * anywhere; if constructed with a Canvas type it will constrain the drawing to
 * the Canvas' coordinates and size.
 */
class Painter {
   public:
    /**
     * @brief Construct a Painter for the entire terminal screen.
     */
    Painter()
        : offset_{0, 0}, size_{Terminal::area()}, screen_{Terminal::changes}
    {}

    /**
     * @brief Construct a Painter with the given Canvas' coordinates and size.
     *
     * @param canvas The Canvas to create a Painter for.
     */
    template <Canvas T>
    explicit Painter(T& canvas)
        : offset_{canvas.coordinates},
          size_{canvas.size},
          screen_{Terminal::changes}
    {}

   public:
    /**
     * @brief Access the Glyph at the given position, offset for Canvas.
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
     * @brief Access the Glyph at the given position, offset for Canvas.
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
     * @brief Clear the underlying Canvas, setting all Glyphs symbols to null.
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
     * @brief Fill the underlying Canvas with the given Glyph.
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