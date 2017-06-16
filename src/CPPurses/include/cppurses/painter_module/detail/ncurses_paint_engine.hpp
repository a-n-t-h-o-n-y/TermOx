#ifndef PAINTER_MODULE_DETAIL_NCURSES_PAINT_ENGINE_HPP
#define PAINTER_MODULE_DETAIL_NCURSES_PAINT_ENGINE_HPP

#include "painter_module/attribute.hpp"
#include "painter_module/color.hpp"
#include "painter_module/paint_engine.hpp"
#include <cstddef>
#include <cstdint>
#include <string>

namespace cppurses {
namespace detail {

class NCurses_paint_engine : public Paint_engine {
   public:
    NCurses_paint_engine();
    NCurses_paint_engine(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine(NCurses_paint_engine&&) noexcept = default;  // NOLINT
    NCurses_paint_engine& operator=(const NCurses_paint_engine&) = delete;
    NCurses_paint_engine& operator=(NCurses_paint_engine&&) =
        default;  // NOLINT
    ~NCurses_paint_engine() override;

    void set_rgb(Color c,
                 std::int16_t r,
                 std::int16_t g,
                 std::int16_t b) override;
    void show_cursor(bool show = true) override;  // NOLINT
    void hide_cursor(bool hide = true) override;  // NOLINT
    std::size_t screen_width() override;
    std::size_t screen_height() override;
    void touch_all() override;
    void set_ctrl_char(bool enable) override;

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
}  // namespace cppurses
#endif  // PAINTER_MODULE_DETAIL_NCURSES_PAINT_ENGINE_HPP
