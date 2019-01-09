#include "palette_demo.hpp"

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace palette {
Palette_demo::Palette_demo() {
    Glyph_string rise{"▉▊▋▌▍▎▏"};
    // slider1.set_lower(rise, false);
    // slider1.set_upper(rise, false);
    // slider1.set_indicator("█");
    slider1.width_policy.type(Size_policy::Fixed);
    slider1.width_policy.hint(3);

    slider2.invert();
    Glyph_string bars{"❚❙❘"};
    slider2.width_policy.type(Size_policy::Fixed);
    slider2.width_policy.hint(3);
    // slider2.set_upper(bars, false);
    // slider2.set_indicator("█");

    slider3.width_policy.type(Size_policy::Fixed);
    slider3.width_policy.hint(3);
}

}  // namespace palette
