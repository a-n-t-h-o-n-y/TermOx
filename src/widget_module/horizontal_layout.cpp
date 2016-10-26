#include <mcurses/widget_module/layouts/horizontal_layout.hpp>

namespace mcurses {

void Horizontal_layout::refresh()
{
	// for each child, look at its size hint and policy and figure out where it should be and 
	// what it's dimensions should be, then post a resize and move event to that child.
	for(Object* c : this->children()) {
		// unsigned new_width = find_width(c); // uses the size policy and hints along with other children to figure this out
		// unsigned new_height = find_height(c); // probably wrong way to do this, since you need to consider all children together.
		// System::post_event(c, std::make_unique<Move_event>(old_width, old_height, new_width, new_height);
	}
}

} // namespace mcurses