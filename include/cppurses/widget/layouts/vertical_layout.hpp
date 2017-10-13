#ifndef WIDGET_LAYOUTS_VERTICAL_LAYOUT_HPP
#define WIDGET_LAYOUTS_VERTICAL_LAYOUT_HPP
#include "widget/layout.hpp"

#include <cstddef>
#include <functional>
#include <vector>

namespace cppurses {
class Widget;

class Vertical_layout : public Layout {
   protected:
    void update_geometry() override;

   private:
    std::vector<std::size_t> size_widgets();
    void position_widgets(const std::vector<std::size_t>& heights);

    void distribute_space(std::vector<Dimensions_reference> widget,
                          int height_left);

    void collect_space(std::vector<Dimensions_reference> widgets,
                       int height_left);
};

}  // namespace cppurses
#endif  // WIDGET_LAYOUTS_VERTICAL_LAYOUT_HPP
