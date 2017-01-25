#ifndef GLYPH_MATRIX_HPP
#define GLYPH_MATRIX_HPP

#include "color.hpp"
#include "glyph.hpp"

#include <cstddef>
#include <vector>

namespace twf {

class Glyph_matrix {
   public:
    explicit Glyph_matrix(std::size_t width = 0, std::size_t height = 0)
        : matrix_{height,
                  std::vector<Glyph>(width,
                                     Glyph{" ", background(Color::Black),
                                           foreground(Color::White)})} {}

    void resize(std::size_t width, std::size_t height) {
        matrix_.resize(height);
        for (auto& row : matrix_) {
            row.resize(width, Glyph{" ", background(Color::Black),
                                    foreground(Color::White)});
        }
    }

    std::size_t width() const {
        return matrix_.empty() ? 0 : matrix_[0].size();
    }

    std::size_t height() const { return matrix_.size(); }

    Glyph& at(std::size_t x, std::size_t y) {
        return this->at_impl(*this, x, y);
    }

    const Glyph& at(std::size_t x, std::size_t y) const {
        return this->at_impl(*this, x, y);
    }

   private:
    template <typename T>
    static auto at_impl(T& t, std::size_t x, std::size_t y)
        -> decltype(t.at(x, y)) {
        if(y >= t.matrix_.size() || x >= t.matrix_.at(y).size()) {
            return t.matrix_.at(0).at(0);
        }
        return t.matrix_.at(y).at(x);
    }

    std::vector<std::vector<Glyph>> matrix_;
};

}  // namespace twf
#endif  // GLYPH_MATRIX_HPP
