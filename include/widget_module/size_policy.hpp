#ifndef SIZE_POLICY
#define SIZE_POLICY

#include <cstddef>

namespace mcurses {

class Size_policy {
   public:
    enum Policy {
        Fixed,
        Minimum,
        Maximum,
        Preferred,
        Expanding,
        MinimumExpanding,
        Ignored
    };

    explicit Size_policy(Policy horizontal = Preferred,
                         Policy vertical = Preferred)
        : policy_h_{horizontal}, policy_v_{vertical} {}

   private:
    Policy policy_h_;
    Policy policy_v_;
    std::size_t stretch_h_ = 1;
    std::size_t stretch_v_ = 1;
    bool height_for_width_ = false;
};

}  // namespace mcurses
#endif  // SIZE_POLICY
