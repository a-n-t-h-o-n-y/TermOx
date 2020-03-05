#include <cppurses/system/system.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include <signals/slot.hpp>

#include <cppurses/painter/palette.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/detail/event_engine.hpp>
#include <cppurses/system/detail/event_queue.hpp>
#include <cppurses/system/detail/focus.hpp>
#include <cppurses/system/detail/user_input_event_loop.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/event_loop.hpp>
#include <cppurses/system/events/focus_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/terminal/terminal.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/widget.hpp>

namespace {

/// Indicates if the receiver of the Event is able to have the Event sent to it.
auto is_sendable(cppurses::Event const& event) -> bool
{
    using Event = cppurses::Event;
    return event.receiver().enabled() or
           (event.type() == Event::Delete or event.type() == Event::Disable or
            event.type() == Event::FocusOut);
}

}  // namespace

namespace cppurses {

auto System::focus_widget() -> cppurses::Widget*
{
    return detail::Focus::focus_widget();
}

void System::set_focus(Widget& w) { detail::Focus::set(w); }

void System::clear_focus() { detail::Focus::clear(); }

void System::enable_tab_focus() { detail::Focus::enable_tab_focus(); }

void System::disable_tab_focus() { detail::Focus::disable_tab_focus(); }

void System::post_event(std::unique_ptr<Event> event)
{
    detail::Event_engine::get().queue().append(std::move(event));
}

void System::exit(int exit_code)
{
    System::exit_requested_ = true;
    System::exit_signal(exit_code);
}

void System::set_head(Widget* new_head)
{
    if (head_ != nullptr)
        head_->disable();
    head_ = new_head;
}

auto System::run() -> int
{
    if (head_ == nullptr)
        return -1;
    terminal.initialize();
    head_->enable();
    System::post_event<Resize_event>(*System::head(),
                                     Area{terminal.width(), terminal.height()});
    detail::Focus::set(*head_);
    auto const exit_code = user_input_loop_.run();
    terminal.uninitialize();
    return exit_code;
}

auto System::send_event(Event const& event) -> bool
{
    if (!is_sendable(event))
        return false;
    auto handled = event.send_to_all_filters();
    if (!handled)
        handled = event.send();
    return handled;
}

sig::Slot<void()> System::quit = []() { System::exit(); };
sig::Signal<void(int)> System::exit_signal;
Widget* System::head_        = nullptr;
bool System::exit_requested_ = false;
detail::User_input_event_loop System::user_input_loop_;
Animation_engine System::animation_engine_;
Terminal System::terminal;

}  // namespace cppurses
