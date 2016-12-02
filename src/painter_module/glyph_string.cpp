#include <mcurses/painter_module/glyph_string.hpp>

#include <algorithm>

namespace mcurses {

bool operator==(const Glyph_string& x, const Glyph_string& y) {
	return std::equal(std::begin(x), std::end(x), std::begin(y), std::end(y));
}

bool operator!=(const Glyph_string& x, const Glyph_string& y) {
	return !(x == y);
}

} // namespace mcurses