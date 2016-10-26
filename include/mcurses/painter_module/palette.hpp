#ifndef PALETTE_HPP
#define PALETTE_HPP

#include "color.hpp"

namespace mcurses {

class Palette {
public:
	// Types
	enum class Style { Standard, Light, Dark };

	Style style() const { return style_; }
	Color background() const { return background_; }
	Color foreground() const { return foreground_; }

private:
	Style style_ = Style::Standard;
	Color background_ = Color::Black;
	Color foreground_ = Color::White;
};

} // namespace mcurses
#endif // PALETTE_HPP