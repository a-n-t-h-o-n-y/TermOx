#ifndef CPPURSES_WIDGET_LAYOUTS_VERTICAL_HPP
#define CPPURSES_WIDGET_LAYOUTS_VERTICAL_HPP
#include <cstddef>
#include <vector>

#include <cppurses/widget/layout.hpp>

namespace cppurses {
class Widget;
namespace layout {

class Vertical : public layout::Layout {
   protected:
    void update_geometry() override;

   private:
    std::vector<Dimensions> calculate_widget_sizes();
    void move_and_resize_children(const std::vector<Dimensions>& dimensions);

    void distribute_space(std::vector<Dimensions_reference> widgets,
                          int height_left);

    void collect_space(std::vector<Dimensions_reference> widgets,
                       int height_left);
};

}  // namespace layout
}  // namespace cppurses
#endif  // CPPURSES_WIDGET_LAYOUTS_VERTICAL_HPP
