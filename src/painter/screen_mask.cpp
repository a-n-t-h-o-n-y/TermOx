#include <cppurses/painter/detail/screen_mask.hpp>

#include <cppurses/widget/area.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/widget.hpp>

namespace {
using namespace cppurses;
using namespace cppurses::detail;

Point make_offset(const Widget& w, Screen_mask::Constructor_tag tag) {
    return tag == Screen_mask::Outer ? Point{w.x(), w.y()}
                                     : Point{w.inner_x(), w.inner_y()};
}

Area make_area(const Widget& w, Screen_mask::Constructor_tag tag) {
    return tag == Screen_mask::Outer ? Area{w.outer_width(), w.outer_height()}
                                     : Area{w.width(), w.height()};
}

}  // namespace

namespace cppurses {
namespace detail {

Screen_mask::Screen_mask(const Widget& w, Constructor_tag tag)
    : offset_{make_offset(w, tag)},
      area_{make_area(w, tag)},
      bits_(area_.width * area_.height) {}

}  // namespace detail
}  // namespace cppurses
