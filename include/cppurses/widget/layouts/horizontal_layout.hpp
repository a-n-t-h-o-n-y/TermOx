#ifndef WIDGET_LAYOUTS_HORIZONTAL_LAYOUT_HPP
#define WIDGET_LAYOUTS_HORIZONTAL_LAYOUT_HPP

#include "widget/layout.hpp"
#include <cstddef>
#include <functional>
#include <tuple>
#include <vector>

namespace cppurses {
class Widget;

class Horizontal_layout : public Layout {
   protected:
    void update_geometry() override;

   private:
    std::vector<std::size_t> size_widgets();
    void position_widgets(const std::vector<std::size_t>& widths);

    void distribute_space(
        std::vector<std::tuple<Widget*,
                               std::reference_wrapper<std::size_t>,
                               std::reference_wrapper<std::size_t>>> widgets,
        int width_left);

    void collect_space(
        std::vector<std::tuple<Widget*,
                               std::reference_wrapper<std::size_t>,
                               std::reference_wrapper<std::size_t>>> widgets,
        int width_left);
};

}  // namespace cppurses
#endif  // WIDGET_LAYOUTS_HORIZONTAL_LAYOUT_HPP
