#ifndef PAINT_ENGINE_HPP
#define PAINT_ENGINE_HPP

#include "attribute.hpp"
#include "color.hpp"
#include "detail/paint_buffer.hpp"
#include "glyph_string.hpp"

#include <cstddef>
#include <cstdint>
#include <string>

namespace twf {

class Paint_engine {
   public:
    Paint_engine() = default;
    Paint_engine(const Paint_engine&) = delete;
    Paint_engine(Paint_engine&&) noexcept = default;
    Paint_engine& operator=(const Paint_engine&) = delete;
    Paint_engine& operator=(Paint_engine&&) noexcept = default;
    virtual ~Paint_engine() = default;

    // Put to buffer
    void put(std::size_t x, std::size_t y, const Glyph& g);
    // void clear(std::size_t x, std::size_t y);

    // Flush to screen
    void flush(bool optimize);

    virtual void set_rgb(Color c,
                         std::uint8_t r,
                         std::uint8_t g,
                         std::uint8_t b) = 0;

    virtual void show_cursor() = 0;
    virtual void hide_cursor() = 0;
    virtual std::size_t screen_width() = 0;
    virtual std::size_t screen_height() = 0;

    virtual void clear_attributes() = 0;
    virtual void touch_all() = 0;
    virtual void move(std::size_t x, std::size_t y) = 0;
    void put_glyph(const Glyph& g);

    detail::Paint_buffer& buffer() { return buffer_; }
    const detail::Paint_buffer& buffer() const { return buffer_; }

   protected:
    // functions to put to physical screen
    virtual void put_string(const char* s) = 0;
    virtual void put_string(const std::string& sym) = 0;

    virtual void set_attribute(Attribute attr) = 0;
    virtual void set_background_color(Color c) = 0;
    virtual void set_foreground_color(Color c) = 0;

    virtual void refresh() = 0;

    detail::Paint_buffer buffer_;
};

}  // namespace twf
#endif  // PAINT_ENGINE_HPP
