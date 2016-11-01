#ifndef BRUSH_HPP
#define BRUSH_HPP

#include "color.hpp"
#include "attribute.hpp"
#include "../signal_module/optional.hpp"

#include <algorithm>
#include <vector>
#include <utility>

namespace mcurses {

class Brush {
public:
	template <typename ... Args>
	Brush(Args... args) {
		this->brush_initializer(std::forward<Args>(args)...);
	}

	void add_attribute(Attribute attr);
	void remove_attribute(Attribute attr);
	void clear_attributes() { attributes_.clear(); }

	void set_background(Color color) { background_color_ = color; }
	void set_foreground(Color color) { foreground_color_ = color; }

	std::vector<Attribute> attributes() const { return attributes_; }
	optional<Color> background_color() const { return background_color_; }
	optional<Color> foreground_color() const { return foreground_color_; }

	friend bool operator==(const Brush& x, const Brush& y) {
	return(std::is_permutation(std::begin(x.attributes_), std::end(x.attributes_), std::begin(y.attributes_), std::end(y.attributes_))
		&& x.background_color_ == y.background_color_
		&& x.foreground_color_ == y.foreground_color_);
}

private:
	// Base Case
	void brush_initializer() {
	}

	// Recursive Case
	template <typename T, typename ... Rest>
	void brush_initializer(T t, Rest... rest) {
		this->do_init(t);
		this->brush_initializer(rest...);
	}

	void do_init(detail::BackgroundColor bc) {
		this->set_background(static_cast<Color>(bc));
		return;
	}

	void do_init(detail::ForegroundColor fc) {
		this->set_foreground(static_cast<Color>(fc));
		return;
	}

	void do_init(Attribute attr) {
		this->add_attribute(attr);
	}

	std::vector<Attribute> attributes_;
	optional<Color> background_color_;
	optional<Color> foreground_color_; 
};

} // namespace mcurses
#endif // BRUSH_HPP