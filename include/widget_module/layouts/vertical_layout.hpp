#ifndef WIDGET_MODULE_LAYOUTS_VERTICAL_LAYOUT_HPP
#define WIDGET_MODULE_LAYOUTS_VERTICAL_LAYOUT_HPP

#include "widget_module/layout.hpp"
#include <cstddef>
#include <functional>
#include <tuple>
#include <vector>

namespace cppurses {

class Vertical_layout : public Layout {
   protected:
    void update_geometry() override;

   private:
    std::vector<std::size_t> size_widgets();
    void position_widgets(const std::vector<std::size_t>& heights);

    void distribute_space(
        std::vector<std::tuple<Widget*,
                               std::reference_wrapper<std::size_t>,
                               std::reference_wrapper<std::size_t>>> widgets,
        int height_left);

    void collect_space(
        std::vector<std::tuple<Widget*,
                               std::reference_wrapper<std::size_t>,
                               std::reference_wrapper<std::size_t>>> widgets,
        int height_left);
};

}  // namespace cppurses
#endif  // WIDGET_MODULE_LAYOUTS_VERTICAL_LAYOUT_HPP
