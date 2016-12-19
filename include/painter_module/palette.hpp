#ifndef PALETTE_HPP
#define PALETTE_HPP

#include "color.hpp"

#include <array>
#include <vector>

namespace mcurses {

class Palette {
   public:
    virtual ~Palette() = default;
    void set_rgb(Color c, int r, int g, int b);

    int red_value(Color c) const;
    int green_value(Color c) const;
    int blue_value(Color c) const;

   private:
    using index_ = int;
    static index_ translate_(Color c);

    std::array<std::array<int, 3>, 16> definitions_;
};

class Standard_palette : public Palette {
   public:
    Standard_palette();
};

class DawnBringer_palette : public Palette {
   public:
    DawnBringer_palette();
};

}  // namespace mcurses
#endif  // PALETTE_HPP
