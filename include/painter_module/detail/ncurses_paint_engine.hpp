#ifndef PAINTER_MODULE_DETAIL_NCURSES_PAINT_ENGINE_HPP
#define PAINTER_MODULE_DETAIL_NCURSES_PAINT_ENGINE_HPP

#include "painter_module/attribute.hpp"
#include "painter_module/color.hpp"
#include "painter_module/paint_engine.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

namespace twf {
namespace detail {

class NCurses_paint_engine : public Paint_engine {
   public:
    NCurses_paint_engine();
    NCurses_paint_engine(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine(NCurses_paint_engine&&) noexcept = default;
    NCurses_paint_engine& operator=(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine& operator=(NCurses_paint_engine&&) = default;
    ~NCurses_paint_engine() override;

    void set_rgb(Color c,
                 std::uint8_t r,
                 std::uint8_t g,
                 std::uint8_t b) override;
    void show_cursor() override;
    void hide_cursor() override;
    std::size_t screen_width() override;
    std::size_t screen_height() override;
    void touch_all() override;

   protected:
    void move(std::size_t x, std::size_t y) override;
    void put_string(const char* s) override;
    void put_string(const std::string& s) override;

    void set_attribute(Attribute attr) override;
    void clear_attributes() override;
    void set_background_color(Color c) override;
    void set_foreground_color(Color c) override;

    void refresh() override;

   private:
    Color current_foreground();
    Color current_background();
};

}  // namespace detail
}  // namespace twf
#endif  // PAINTER_MODULE_DETAIL_NCURSES_PAINT_ENGINE_HPP
