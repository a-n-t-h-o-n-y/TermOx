#include <cppurses/painter/detail/screen_mask.hpp>

#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {

Screen_mask::Screen_mask(const Widget& w)
    : offset_{w.x(), w.y()},
      area_{w.outer_width(), w.outer_height()},
      bits_(area_.width * area_.height) {}

}  // namespace detail
}  // namespace cppurses
