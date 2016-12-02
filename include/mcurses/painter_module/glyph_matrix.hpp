#ifndef GLYPH_MATRIX_HPP
#define GLYPH_MATRIX_HPP

#include "glyph.hpp"
#include "color.hpp"

#include <vector>
#include <stdexcept>

namespace mcurses {

class Glyph_matrix {
public:
	explicit Glyph_matrix(unsigned width = 0, unsigned height = 0)
	:matrix_{height, std::vector<Glyph>(width, Glyph{" ", background(Color::Black), foreground(Color::White)})}
	{}

	void resize(unsigned width, unsigned height) {
		matrix_.resize(height);
		for(auto& row : matrix_) {
			row.resize(width, Glyph{" ", background(Color::Black), foreground(Color::White)});
		}
	}

	unsigned width() const {
		if(matrix_.size() == 0) {
			return 0;
		}
		return matrix_[0].size();
	}

	unsigned height() const { return matrix_.size(); }

	Glyph& at(unsigned x, unsigned y) {
		if(matrix_.size() == 0 || y >= matrix_.size() || x >= matrix_[0].size()) {
			throw std::out_of_range("Matrix access");
		}
		return matrix_[y][x];
	}

	const Glyph& at(unsigned x, unsigned y) const {
		auto nc_this = const_cast<Glyph_matrix*>(this);
		return nc_this->at(x, y);
	}

private:
	std::vector<std::vector<Glyph>> matrix_;
};

} // namespace mcurses
#endif // GLYPH_MATRIX_HPP