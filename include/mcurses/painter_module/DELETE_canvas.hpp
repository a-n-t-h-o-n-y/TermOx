#ifndef CANVAS_HPP
#define CANVAS_HPP

#include "detail/ncurses_paint_engine.hpp"
#include "size_policy.hpp"

#include <memory>
#include <limits>

namespace mcurses {
class Paint_engine;

// Coordinates are relative to parent
class Canvas {
public:
	// Each of these set functions should send signal or event to notify change of geometry
	// void set_geometry(unsigned x, unsigned y, unsigned width, unsigned height);
	// void set_x(unsigned x){ x_ = x; }
	// void set_y(unsigned y){ y_ = y; }
	// void set_width(unsigned width) { width_ = width; }
	// void set_height(unsigned height) { height_ = height; }
	// void set_width_hint(unsigned width_hint) { width_hint_ = width_hint; }
	// void set_height_hint(unsigned height_hint) { height_hint_ = height_hint; }
	// void set_max_width(unsigned max_width) { max_width_ = max_width; }
	// void set_max_height(unsigned max_height) { max_height_ = max_height; }
	// void set_min_width(unsigned min_width) { min_width_ = min_width; }
	// void set_min_width_hint(unsigned min_width_hint) { min_width_hint_ = min_width_hint; }
	// void set_min_height(unsigned min_height) { min_height_ = min_height; }
	// void set_min_height_hint(unsigned min_height_hint) { min_height_hint_ = min_height_hint; }
	// void set_size_policy(const Size_policy& policy) { size_policy_ = policy; }
	// void set_show_cursor(bool show) { show_cursor_ = show; }
	// void set_paint_engine(std::unique_ptr<Paint_engine> engine) { paint_engine_ = engine; }

	// unsigned x() const { return x_; }
	// unsigned y() const { return y_; }
	// unsigned width() const { return width_; }
	// unsigned height() const { return height_; }
	// unsigned width_hint() const { return width_hint_; }
	// unsigned height_hint() const { return height_hint_; }
	// unsigned max_width() const { return max_width_; }
	// unsigned max_height() const { return max_height_; }
	// unsigned min_width() const { return min_width_; }
	// unsigned min_width_hint() const { return min_width_hint_; }
	// unsigned min_height() const { return min_height_; }
	// unsigned min_height_hint() const { return min_height_hint; }
	// Size_policy size_policy() const { return size_policy_; }
	// unsigned max_x() const { return x_ + height_; }
	// unsigned max_y() const { return y_ + width_; }
	// bool show_cursor() const {return show_cursor_; }
	// Paint_engine& engine() const { return *engine_; }

	// bool check_coordinates(unsigned glob_x, unsigned glob_y);

protected:
	Canvas() = default;

	// unsigned x_ = 0;
	// unsigned y_ = 0;

	// unsigned width_ = 0;
	// unsigned height_ = 0;

	// unsigned width_hint_ = 0;
	// unsigned height_hint_ = 0;

	// unsigned max_width_ = std::numeric_limits<unsigned>::max();
	// unsigned max_height_ = std::numeric_limits<unsigned>::max();
	// unsigned min_width_ = 0;
	// unsigned min_width_hint = 0;
	// unsigned min_height_ = 0;
	// unsigned min_height_hint = 0;

	// Size_policy size_policy_;

	// bool show_cursor_ = true;

	// std::unique_ptr<Paint_engine> engine_ = std::make_unique<detail::NCurses_paint_engine>();
};

} // namespace mcurses

#endif // CANVAS_HPP