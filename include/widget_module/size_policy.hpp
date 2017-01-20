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
    bool height_for_width = false;

    explicit Size_policy(Policy horizontal = Preferred,
                         Policy vertical = Preferred)
        : horizontal_policy{horizontal}, vertical_policy{vertical} {}
};

}  // namespace twf
#endif  // SIZE_POLICY
