#include <string>

#include <termox/system/system.hpp>
#include <termox/widget/array.hpp>
#include <termox/widget/layouts/horizontal.hpp>
#include <termox/widget/layouts/vertical.hpp>
#include <termox/widget/pair.hpp>
#include <termox/widget/tuple.hpp>
#include <termox/widget/widgets/line.hpp>
#include <termox/widget/widgets/slider.hpp>
#include <termox/widget/widgets/text_display.hpp>

using namespace ox;

namespace {

template <typename Slider_t>
struct Slider_view : VPair<Slider_t, Text_display> {
    Slider_view() : VPair<Slider_t, Text_display>{{{-255, 255}, 0}, {U"Init"}}
    {
        this->first.value_changed.connect(
            [this](int v) { this->second.set_contents(std::to_string(v)); });
        this->second | pipe::fixed_height(1);
    }
};

}  // namespace

int main()
{
    System{Mouse_mode::Drag}
        .run<HTuple<Slider_view<Slider_top>, VLine, Slider_view<Slider_bottom>,
                    VLine, Slider_view<Slider_left>, VLine,
                    Slider_view<Slider_right>>>();
}
