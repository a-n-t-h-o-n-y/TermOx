#ifndef CPPURSES_WIDGET_LAYOUTS_HORIZONTAL_HPP
#define CPPURSES_WIDGET_LAYOUTS_HORIZONTAL_HPP
#include <cstddef>
#include <vector>

#include <cppurses/widget/layout.hpp>

namespace cppurses {
class Widget;
namespace layout {

class Horizontal : public Layout {
   protected:
    void update_geometry() override;

   private:
    std::vector<Dimensions> calculate_widget_sizes();
    void move_and_resize_children(const std::vector<Dimensions>& dimensions);

    void distribute_space(std::vector<Dimensions_reference> widgets,
                          int width_left);

    void collect_space(std::vector<Dimensions_reference> widgets,
                       int width_left);
    // void collect_space(std::vector<Dimensions>& widgets, int width_left);
};

}  // namespace layout
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_LAYOUTS_HORIZONTAL_HPP
