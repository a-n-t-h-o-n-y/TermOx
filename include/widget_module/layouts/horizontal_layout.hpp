#ifndef HORIZONTAL_LAYOUT_HPP
#define HORIZONTAL_LAYOUT_HPP

#include <cstddef>
#include <vector>
#include <tuple>
#include "../layout.hpp"

namespace twf {
class Widget;

class Horizontal_layout : public Layout {
   protected:
    void update_geometry() override;

   private:
    void size_widgets();
    void position_widgets();

    void distribute_space(
        std::vector<std::tuple<Widget*, std::size_t&, std::size_t&>> widgets,
        int width_left);

    void collect_space(
        std::vector<std::tuple<Widget*, std::size_t&, std::size_t&>> widgets,
        int width_left);
};

}  // namespace twf
#endif  // HORIZONTAL_LAYOUT_HPP
