#include "main_menu.hpp"

#include <cppurses/system/events/key.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/menu.hpp>

using namespace cppurses;
namespace demos {

Main_menu::Main_menu()
{
    this->focus_policy = Focus_policy::Direct;

    auto& esc_short = Shortcuts::add_shortcut(Key::Escape);
    esc_short.connect([this] { main_menu.goto_menu(); });
}
}  // namespace demos
