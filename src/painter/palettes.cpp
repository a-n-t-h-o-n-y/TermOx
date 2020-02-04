#include <cppurses/painter/palettes.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/palette.hpp>

namespace cppurses {

Palette Palettes::Standard()
{
    return {{{Color::Black, {0, 0, 0}},
             {Color::Dark_red, {128, 0, 0}},
             {Color::Dark_blue, {0, 0, 128}},
             {Color::Dark_gray, {105, 105, 105}},
             {Color::Brown, {128, 128, 0}},
             {Color::Green, {0, 128, 0}},
             {Color::Red, {255, 0, 0}},
             {Color::Gray, {128, 128, 128}},
             {Color::Blue, {0, 0, 255}},
             {Color::Orange, {255, 165, 0}},
             {Color::Light_gray, {192, 192, 192}},
             {Color::Light_green, {0, 255, 0}},
             {Color::Violet, {255, 0, 255}},
             {Color::Light_blue, {0, 255, 255}},
             {Color::Yellow, {255, 255, 0}},
             {Color::White, {255, 255, 255}}}};
}

Palette Palettes::DawnBringer()
{
    return {{{Color::Black, {20, 12, 28}},
             {Color::Dark_red, {68, 36, 52}},
             {Color::Dark_blue, {48, 52, 109}},
             {Color::Dark_gray, {78, 74, 78}},
             {Color::Brown, {133, 76, 48}},
             {Color::Green, {52, 101, 36}},
             {Color::Red, {208, 70, 72}},
             {Color::Gray, {117, 113, 97}},
             {Color::Blue, {89, 125, 206}},
             {Color::Orange, {210, 125, 44}},
             {Color::Light_gray, {133, 149, 161}},
             {Color::Light_green, {109, 170, 44}},
             {Color::Violet, {210, 170, 153}},
             {Color::Light_blue, {109, 194, 202}},
             {Color::Yellow, {218, 212, 94}},
             {Color::White, {222, 238, 214}}}};
}

}  // namespace cppurses
