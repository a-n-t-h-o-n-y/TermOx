#ifndef TERMOX_DEMOS_GAME_OF_LIFE_MAKE_BREAK_HPP
#define TERMOX_DEMOS_GAME_OF_LIFE_MAKE_BREAK_HPP
#include <memory>

#include <termox/widget/pipe.hpp>
#include <termox/widget/widget.hpp>

#include "colors.hpp"

namespace gol {

inline auto make_break() -> std::unique_ptr<ox::Widget>
{
    using namespace ox::pipe;
    return ox::widget() | fixed_height(1uL) | wallpaper(L'─' | fg(color::Teal));
}

}  // namespace gol
#endif  // TERMOX_DEMOS_GAME_OF_LIFE_MAKE_BREAK_HPP
