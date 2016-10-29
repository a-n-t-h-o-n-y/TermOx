#include <mcurses/painter_module/brush.hpp>
#include <mcurses/painter_module/attribute.hpp>

#include <algorithm>

namespace mcurses {

void
Brush::add_attribute(Attribute attr) {
	auto at = std::find(std::begin(attributes_), std::end(attributes_), attr);
	if(at == std::end(attributes_)) {
		attributes_.push_back(attr);
	}
	return;
}

void
Brush::remove_attribute(Attribute attr) {
	auto at = std::find(std::begin(attributes_), std::end(attributes_), attr);
	if(at != std::end(attributes_)) {
		attributes_.erase(at);
	}
	return;
}

} // namespace mcurses