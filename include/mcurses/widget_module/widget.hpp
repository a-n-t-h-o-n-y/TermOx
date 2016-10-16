#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "../system_module/object.hpp"
#include "../painter_module/canvas.hpp"

namespace mcurses {

class Widget : public Object, public Canvas {
public:
	Widget(unsigned x, unsigned y, unsigned width, unsigned height);
	bool has_coordinates(unsigned glob_x, unsigned glob_y) override;
	
private:

};

} // namespace mcurses

#endif // WIDGET_HPP