#ifndef HORIZONTAL_LAYOUT_HPP
#define HORIZONTAL_LAYOUT_HPP

#include "../layout.hpp"

namespace twf {

class Horizontal_layout : public Layout {
   public:
   protected:
    void update_geometry() override;
};

}  // namespace twf
#endif  // HORIZONTAL_LAYOUT_HPP
