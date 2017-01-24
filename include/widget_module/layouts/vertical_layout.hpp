#ifndef VERTICAL_LAYOUT_HPP
#define VERTICAL_LAYOUT_HPP

#include "../layout.hpp"

namespace twf {

class Vertical_layout : public Layout {
   public:
   protected:
    void update_geometry() override;
};

}  // namespace twf
#endif  // VERTICAL_LAYOUT_HPP
