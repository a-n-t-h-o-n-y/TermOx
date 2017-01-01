#ifndef PAINT_ENGINE_HPP
#define PAINT_ENGINE_HPP

#include "attribute.hpp"
#include "color.hpp"
#include "detail/paint_buffer.hpp"
#include "glyph_string.hpp"

#include <string>

namespace twf {

class Paint_engine {
   public:
    virtual ~Paint_engine() = default;

    // Put to buffer
    void put(unsigned x, unsigned y, const Glyph& g);

    // Flush to screen
    void flush();

    virtual void set_rgb(Color c, int r, int g, int b) = 0;

    virtual void show_cursor() = 0;
    virtual void hide_cursor() = 0;
    virtual unsigned screen_width() = 0;
    virtual unsigned screen_height() = 0;

   protected:
    // functions to put to physical screen
    virtual void move(unsigned x, unsigned y) = 0;
    virtual void put_string(const std::string& sym) = 0;

    virtual void set_attribute(Attribute attr) = 0;
    virtual void clear_attributes() = 0;
    virtual void set_background_color(Color c) = 0;
    virtual void set_foreground_color(Color c) = 0;

    virtual void refresh() = 0;

    detail::Paint_buffer buffer_;

   private:
    void put_glyph(const Glyph& g);
};

}  // namespace twf
#endif  // PAINT_ENGINE_HPP
