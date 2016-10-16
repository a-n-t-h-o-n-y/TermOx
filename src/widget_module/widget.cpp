#include <mcurses/widget_module/widget.hpp>

namespace mcurses {

Widget::Widget(unsigned x, unsigned y, unsigned width, unsigned height)
:Canvas{x, y, width, height}
{}

bool
Widget::has_coordinates(unsigned glob_x, unsigned glob_y)
{
	return Canvas::check_coordinates(glob_x, glob_y);
}

} // namespace mcurses