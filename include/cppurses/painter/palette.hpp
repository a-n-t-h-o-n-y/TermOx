#ifndef PAINTER_PALETTE_HPP
#define PAINTER_PALETTE_HPP
#include <array>
#include <cstdint>

#include <cppurses/painter/color.hpp>

namespace cppurses {

struct RGB {
    std::int16_t red;
    std::int16_t green;
    std::int16_t blue;
};

class Palette {
   public:
    Palette() = default;
    Palette(const Palette&) = default;
    Palette& operator=(const Palette&) = default;
    Palette(Palette&&) noexcept = default;             // NOLINT
    Palette& operator=(Palette&&) noexcept = default;  // NOLINT
    virtual ~Palette() = default;

    void initialize();

   protected:
    void set_color(Color c, std::int16_t r, std::int16_t g, std::int16_t b);
    void set_color(Color c, RGB values);

   private:
    std::array<RGB, 16> definitions_;

    RGB get_values(Color c) const;
    std::int16_t red_value(Color c) const;
    std::int16_t green_value(Color c) const;
    std::int16_t blue_value(Color c) const;
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
#endif  // PAINTER_PALETTE_HPP
