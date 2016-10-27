#ifndef SIZE_POLICY
#define SIZE_POLICY

namespace mcurses {

class Size_policy {
public:
	enum Policy { Fixed, Minimum, Maximum, Preferred, Expanding, MinimumExpanding, Ignored };

	Size_policy(Policy horizontal = Preferred , Policy vertical = Preferred )
	:policy_h_{horizontal}, policy_v_{vertical}
	{}

private:
	Policy policy_h_;
	Policy policy_v_;
	int stretch_h_;
	int stretch_v_;
	bool height_for_width_;
};

} // namespace mcurses
#endif // SIZE_POLICY