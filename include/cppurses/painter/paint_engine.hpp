#ifndef PAINTER_PAINT_ENGINE_HPP
#define PAINTER_PAINT_ENGINE_HPP

#include "painter/attribute.hpp"
#include "painter/color.hpp"
#include "painter/detail/paint_buffer.hpp"
#include "painter/glyph_string.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

namespace cppurses {

class Paint_engine {
   public:
    Paint_engine() = default;
    Paint_engine(const Paint_engine&) = delete;
    Paint_engine(Paint_engine&&) noexcept = default;  // NOLINT
    Paint_engine& operator=(const Paint_engine&) = delete;
    Paint_engine& operator=(Paint_engine&&) noexcept = default;  // NOLINT
    virtual ~Paint_engine() = default;

    virtual void move(std::size_t x, std::size_t y) = 0;

    // Put to buffer
    void put(std::size_t x, std::size_t y, const Glyph& g);
    void put_glyph(const Glyph& g);

    // Flush to screen
    void flush(bool optimize);

    virtual void set_rgb(Color c,
                         std::int16_t r,
                         std::int16_t g,
                         std::int16_t b) = 0;

    virtual void show_cursor(bool show = true) = 0;  // NOLINT
    virtual void hide_cursor(bool hide = true) = 0;  // NOLINT
    virtual std::size_t screen_width() = 0;
    virtual std::size_t screen_height() = 0;

    virtual void clear_attributes() = 0;
    virtual void touch_all() = 0;
    virtual void set_ctrl_char(bool enable) = 0;

    detail::Paint_buffer& buffer() { return buffer_; }
    const detail::Paint_buffer& buffer() const { return buffer_; }

   protected:
    // Functions to put to screen, not buffer.
    virtual void put_string(const char* s) = 0;
    virtual void put_string(const std::string& sym) = 0;

    virtual void set_attribute(Attribute attr) = 0;
    virtual void set_background_color(Color c) = 0;
    virtual void set_foreground_color(Color c) = 0;

    virtual void refresh() = 0;

    detail::Paint_buffer buffer_;
};

}  // namespace cppurses
#endif  // PAINTER_PAINT_ENGINE_HPP
