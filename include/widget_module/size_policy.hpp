#ifndef SIZE_POLICY
#define SIZE_POLICY

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

    Policy horizontal_policy;
    Policy vertical_policy;
    std::size_t horizontal_stretch = 1;
    std::size_t vertical_stretch = 1;
    bool height_for_width = false; // essentially fixed, nothing in width policy matters
    bool width_for_height = false; // you can only check for one on hori/vert layouts otherwise recursive loop

    explicit Size_policy(Policy horizontal = Preferred,
                         Policy vertical = Preferred)
        : horizontal_policy{horizontal}, vertical_policy{vertical} {}
};

}  // namespace twf
#endif  // SIZE_POLICY
