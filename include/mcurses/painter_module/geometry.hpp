#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "../system_module/system.hpp"
#include "../widget_module/size_policy.hpp"

#include <limits>

namespace mcurses {
class Widget;

class Geometry {
public:
	Geometry(Widget* widget = nullptr, unsigned width_hint = System::max_width(), unsigned height_hint = System::max_height(), const Size_policy& policy = Size_policy{})
	:widget_{widget}, width_hint_{width_hint}, height_hint_{height_hint}, size_policy_{policy}
	{}

	void set_widget(Widget* widget) { widget_ = widget; }

	// Set
	void set_width(unsigned width) { width_ = width; }
	void set_height(unsigned height) { height_ = height; }
	void set_width_hint(unsigned width_hint);
	void set_height_hint(unsigned height_hint);
	void set_max_width(unsigned max_width);
	void set_max_height(unsigned max_height);
	void set_min_width(unsigned min_width);
	void set_min_width_hint(unsigned min_width_hint);
	void set_min_height(unsigned min_height);
	void set_min_height_hint(unsigned min_height_hint);
	void set_size_policy(const Size_policy& policy);

	void set_fixed_width(unsigned width) {
		this->set_min_width(width); this->set_max_width(width); return;
	}
	void set_fixed_height(unsigned height) {
		this->set_min_height(height); this->set_max_height(height); return;
	}
	
	void set_active_region(unsigned west_offset = 0, unsigned east_offset = 0, unsigned north_offset = 0, unsigned south_offset = 0) {
		offset_west_ = west_offset; offset_east_ = east_offset;
		offset_north_ = north_offset; offset_south_ = south_offset;
	}

	// Get
	unsigned width() const { return width_; }
	unsigned height() const { return height_; }

	unsigned width_hint() const { return width_hint_; }
	unsigned height_hint() const { return height_hint_; }
	unsigned max_width() const { return max_width_; }
	unsigned max_height() const { return max_height_; }
	unsigned min_width() const { return min_width_; }
	unsigned min_width_hint() const { return min_width_hint_; }
	unsigned min_height() const { return min_height_; }
	unsigned min_height_hint() const { return min_height_hint_; }
	
	Size_policy size_policy() const { return size_policy_; }

	unsigned active_x_min() const { return this->offset_west_; }
	unsigned active_x_max() const { return this->width() - 1 - this->offset_east_; }
	unsigned active_y_min() const { return this->offset_north_; }
	unsigned active_y_max() const { return this->height() - 1 - this->offset_south_; }

private:
	Widget* widget_;

	// Parameters for Layouts to use
	unsigned width_hint_;
	unsigned height_hint_;
	unsigned max_width_ = std::numeric_limits<unsigned>::max();
	unsigned max_height_ = std::numeric_limits<unsigned>::max();
	unsigned min_width_ = 0;
	unsigned min_width_hint_ = 0;
	unsigned min_height_ = 0;
	unsigned min_height_hint_ = 0;

	Size_policy size_policy_;

	// Active Region Parameters
	unsigned offset_west_ = 0;
	unsigned offset_east_ = 0;
	unsigned offset_north_ = 0;
	unsigned offset_south_ = 0;

	// Actual Size
	unsigned width_ = width_hint_;
	unsigned height_ = height_hint_;
};

} // namespace mcurses
#endif // GEOMETRY_HPP