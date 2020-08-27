#ifndef CPPURSES_DEMOS_GAME_OF_LIFE_MAKE_BREAK_HPP
#define CPPURSES_DEMOS_GAME_OF_LIFE_MAKE_BREAK_HPP
#include <memory>

#include <cppurses/widget/pipe.hpp>
#include <cppurses/widget/widget.hpp>

#include "colors.hpp"

namespace gol {

inline auto make_break() -> std::unique_ptr<cppurses::Widget>
{
    using namespace cppurses::pipe;
    return cppurses::widget() | fixed_height(1uL) |
           wallpaper(L'─' | foreground(color::Teal));
}

}  // namespace gol
#endif  // CPPURSES_DEMOS_GAME_OF_LIFE_MAKE_BREAK_HPP
