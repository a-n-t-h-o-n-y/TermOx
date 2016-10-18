#ifndef WIDGET_HPP
#define WIDGET_HPP

#include "../system_module/object.hpp"
#include "../painter_module/canvas.hpp"
#include "../system_module/events/paint_event.hpp"

#include <memory>

namespace mcurses {

class Widget : public Object, public Canvas {
public:
	Widget(unsigned x, unsigned y, unsigned width, unsigned height);
	bool has_coordinates(unsigned glob_x, unsigned glob_y) override;
	
	void update();
private:

};

} // namespace mcurses

#endif // WIDGET_HPP