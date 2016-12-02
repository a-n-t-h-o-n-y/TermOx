#ifndef PAINT_BUFFER_HPP
#define PAINT_BUFFER_HPP

#include "../glyph_matrix.hpp"

namespace mcurses {
namespace detail {

class Paint_buffer {
public:
	explicit Paint_buffer(unsigned x = 0, unsigned y = 0)
	:backing_store_{x,y}, staging_area_{x,y}
	{}

	void resize(unsigned x, unsigned y) {
		backing_store_.resize(x,y);
		staging_area_.resize(x,y);
	}

	unsigned width() const { return staging_area_.width(); }

	unsigned height() const { return staging_area_.height(); }

	bool commit(unsigned x, unsigned y);

	Glyph& at(unsigned x, unsigned y) {
		return backing_store_.at(x,y);
	}

	void stage(unsigned x, unsigned y, const Glyph& glyph) {
		staging_area_.at(x,y) = glyph;
	}

private:
	Glyph_matrix backing_store_;
	Glyph_matrix staging_area_;
};

} // namespace detail
} // namespace mcurses
#endif // PAINT_BUFFER_HPP