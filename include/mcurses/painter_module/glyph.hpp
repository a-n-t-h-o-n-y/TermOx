#ifndef GLYPH_HPP
#define GLYPH_HPP

#include "color.hpp"
#include "brush.hpp"

#include <sstream>
#include <stdexcept>
#include <string>

namespace mcurses {

class Glyph_error : public std::runtime_error {
public:
	using std::runtime_error::runtime_error;
};

class Glyph {
public:
	Glyph() = default;

	template <typename T>
	Glyph(const T& item) {
		this->set_glyph(item);
	}

	template <typename T>
	void set_glyph(const T& item) {
		std::stringstream ss;
		ss << item;
		if(verify_length_(ss.str())) {
			character_ = ss.str();
		} else {
			throw Glyph_error("Printable size is greater than one character.");
		}
	}

	std::string str() const { return character_; }

private:
	bool verify_length_(const std::string& s);

	std::string character_ = " ";
	Brush brush_;
};

} // namespace mcurses
#endif // GLYPH_HPP