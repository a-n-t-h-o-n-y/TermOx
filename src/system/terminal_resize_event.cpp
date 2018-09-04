#include <cppurses/system/events/terminal_resize_event.hpp>

#include <ncurses.h>
#include <stdio.h>
#include <sys/ioctl.h>

namespace cppurses {

Terminal_resize_event::Terminal_resize_event(Event_handler* receiver)
    : Event{Event::TerminalResize, receiver} {}

bool Terminal_resize_event::send() const {
    struct winsize w;
    ioctl(fileno(stdin), TIOCGWINSZ, &w);
    ::resizeterm(w.ws_row, w.ws_col);  // glitch here w/multi-thread?
    return true;
}

bool Terminal_resize_event::filter_send(Event_handler* filter) const {
    return true;
}

}  // namespace cppurses
