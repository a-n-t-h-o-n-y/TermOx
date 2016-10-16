#include <mcurses/painter_module/canvas.hpp>

namespace mcurses {

Canvas::Canvas(unsigned x, unsigned y, unsigned width, unsigned height)
:x_{x}, y_{y}, width_{width}, height_{height}
{}

bool
Canvas::check_coordinates(unsigned glob_x, unsigned glob_y)
{
	if(glob_x >= x_ && glob_x <= x_ + width_) {
		if(glob_y >= y_ && glob_y <= y_ + height_) {
			return true;
		}
	}
	return false;
}

} // namespace mcurses