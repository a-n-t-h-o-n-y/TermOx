#ifndef CANVAS_HPP
#define CANVAS_HPP

#include "detail/ncurses_paint_engine.hpp"

#include <memory>

namespace mcurses {
class Paint_engine;

class Canvas {
public:
	Canvas(unsigned x, unsigned y, unsigned width, unsigned height);
	bool check_coordinates(unsigned glob_x, unsigned glob_y);
	unsigned position_x() const { return x_; }
	unsigned position_y() const { return y_; }
	unsigned height() const { return height_; }
	unsigned width() const { return width_; }
	unsigned max_x() const { return x_ + height_; }
	unsigned max_y() const { return y_ + width_; }
	Paint_engine& engine() const { return *engine_; }
	bool show_cursor() const {return show_cursor_; }

protected:
	unsigned x_;
	unsigned y_;
	unsigned width_;
	unsigned height_;
	std::unique_ptr<Paint_engine> engine_ = std::make_unique<detail::NCurses_paint_engine>();
	bool show_cursor_ = true;
};

} // namespace mcurses

#endif // CANVAS_HPP