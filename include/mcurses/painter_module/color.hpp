#ifndef COLOR_HPP
#define COLOR_HPP

namespace mcurses {

enum class Color {
	Black,
	White,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan

};

namespace detail {

enum class BackgroundColor {
	Black,
	White,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan
};

enum class ForegroundColor {
	Black,
	White,
	Red,
	Green,
	Yellow,
	Blue,
	Magenta,
	Cyan
};

} // namespace detail

// Free Functions
inline detail::BackgroundColor
background(Color c) {
	return static_cast<detail::BackgroundColor>(c);
}

inline detail::ForegroundColor
foreground(Color c) {
	return static_cast<detail::ForegroundColor>(c);
}

} // namespace mcurses
#endif // COLOR_HPP