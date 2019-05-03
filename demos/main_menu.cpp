#include "main_menu.hpp"

#include <cppurses/system/events/key.hpp>
#include <cppurses/system/shortcuts.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/menu.hpp>

using namespace cppurses;
namespace demos {

Main_menu::Main_menu() {
    this->set_name("Main_menu - head widget");
    titlebar.set_name("Titlebar in Main_menu");
    main_menu.set_name("Menu_stack in Main_menu");
    main_menu.menu().set_name("Menu in Main Widget Stack Menu");

    auto& esc_short = Shortcuts::add_shortcut(Key::Escape);
    esc_short.connect([this] { main_menu.goto_menu(); });
}
}  // namespace demos
