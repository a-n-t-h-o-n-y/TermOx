#include <cppurses/system/events/terminal_resize_event.hpp>

#include <stdio.h>
#include <sys/ioctl.h>

#include <cppurses/system/system.hpp>

namespace cppurses {

bool Terminal_resize_event::send() const {
    struct winsize w;
    ioctl(fileno(stdin), TIOCGWINSZ, &w);
    System::terminal.resize(w.ws_col, w.ws_row);
    return true;
}

}  // namespace cppurses
