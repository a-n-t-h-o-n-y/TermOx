#include <painter_module/brush.hpp>
#include <painter_module/attribute.hpp>

#include <algorithm>

namespace mcurses {

void
Brush::remove_attribute(Attribute attr) {
	auto at = std::find(std::begin(attributes_), std::end(attributes_), attr);
	if(at != std::end(attributes_)) {
		attributes_.erase(at);
	}
	return;
}

void
Brush::push_attribute(Attribute attr) {
	auto at = std::find(std::begin(attributes_), std::end(attributes_), attr);
	if(at == std::end(attributes_)) {
		attributes_.push_back(attr);
	}
	return;
}

} // namespace mcurses
