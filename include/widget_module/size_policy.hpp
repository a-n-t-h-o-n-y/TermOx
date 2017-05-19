#ifndef WIDGET_MODULE_SIZE_POLICY
#define WIDGET_MODULE_SIZE_POLICY

#include <cstddef>

namespace twf {

struct Size_policy {
    enum Policy {
        Fixed,
        Minimum,
        Maximum,
        Preferred,
        Expanding,
        MinimumExpanding,
        Ignored
    };

    Policy horizontal_policy = Ignored;
    Policy vertical_policy = Ignored;

    /// 0 value for stretch factors is undefined behavior
    std::size_t horizontal_stretch = 1;

    /// 0 value for stretch factors is undefined behavior
    std::size_t vertical_stretch = 1;

    bool height_for_width = false;
    bool width_for_height = false;
};

}  // namespace twf
#endif  // WIDGET_MODULE_SIZE_POLICY
