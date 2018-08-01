#ifndef PAINTER_PALETTE_HPP
#define PAINTER_PALETTE_HPP
#include <array>
#include <cstdint>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/rgb.hpp>

namespace cppurses {

class Palette {
   public:
    Palette() = default;
    Palette(const Palette&) = default;
    Palette& operator=(const Palette&) = default;
    Palette(Palette&&) noexcept = default;
    Palette& operator=(Palette&&) noexcept = default;
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

// TODO You could have function that return a Palette with specific definitions.
class DawnBringer_palette : public Palette {
   public:
    DawnBringer_palette();
};

}  // namespace cppurses
#endif  // PAINTER_PALETTE_HPP
