#ifndef PAINTER_MODULE_PALETTE_HPP
#define PAINTER_MODULE_PALETTE_HPP

#include "painter_module/color.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

namespace cppurses {

class Palette {
   public:
    Palette() = default;
    Palette(const Palette&) = default;
    Palette& operator=(const Palette&) = default;
    Palette(Palette&&) noexcept = default;             // NOLINT
    Palette& operator=(Palette&&) noexcept = default;  // NOLINT
    virtual ~Palette() = default;

    void set_rgb(Color c, std::int16_t r, std::int16_t g, std::int16_t b);
    int red_value(Color c) const;
    int green_value(Color c) const;
    int blue_value(Color c) const;

   private:
    std::array<std::array<int, 3>, 16> definitions_{};
};

class Standard_palette : public Palette {
   public:
    Standard_palette();
};

class DawnBringer_palette : public Palette {
   public:
    DawnBringer_palette();
};

}  // namespace cppurses
#endif  // PAINTER_MODULE_PALETTE_HPP
