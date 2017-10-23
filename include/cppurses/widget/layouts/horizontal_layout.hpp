#ifndef WIDGET_LAYOUTS_HORIZONTAL_LAYOUT_HPP
#define WIDGET_LAYOUTS_HORIZONTAL_LAYOUT_HPP
#include <cppurses/widget/layout.hpp>

#include <cstddef>
#include <functional>
#include <vector>

namespace cppurses {
class Widget;

class Horizontal_layout : public Layout {
   protected:
    void update_geometry() override;

   private:
    std::vector<std::size_t> size_widgets();
    void position_widgets(const std::vector<std::size_t>& widths);

    void distribute_space(std::vector<Dimensions_reference> widgets,
                          int width_left);

    void collect_space(std::vector<Dimensions_reference> widgets,
                       int width_left);
};

}  // namespace cppurses
#endif  // WIDGET_LAYOUTS_HORIZONTAL_LAYOUT_HPP
