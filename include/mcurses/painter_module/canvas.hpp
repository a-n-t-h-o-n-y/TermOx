#ifndef CANVAS_HPP
#define CANVAS_HPP

namespace mcurses {

class Canvas {
public:
	Canvas(unsigned x, unsigned y, unsigned width, unsigned height);
	bool check_coordinates(unsigned glob_x, unsigned glob_y);
	unsigned position_x() const { return x_; }
	unsigned position_y() const { return y_; }

protected:
	unsigned x_;
	unsigned y_;
	unsigned width_;
	unsigned height_;
};

} // namespace mcurses

#endif // CANVAS_HPP