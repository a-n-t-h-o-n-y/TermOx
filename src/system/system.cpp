#include <termox/system/system.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/system/animation_engine.hpp>
#include <termox/system/detail/event_engine.hpp>
#include <termox/system/detail/event_queue.hpp>
#include <termox/system/detail/filter_send.hpp>
#include <termox/system/detail/focus.hpp>
#include <termox/system/detail/is_sendable.hpp>
#include <termox/system/detail/send.hpp>
#include <termox/system/detail/user_input_event_loop.hpp>
#include <termox/system/event.hpp>
#include <termox/system/event_loop.hpp>
#include <termox/system/system.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/widget.hpp>

namespace ox {

auto System::focus_widget() -> Widget* { return detail::Focus::focus_widget(); }

void System::set_focus(Widget& w) { detail::Focus::set(w); }

void System::clear_focus() { detail::Focus::clear(); }

void System::enable_tab_focus() { detail::Focus::enable_tab_focus(); }

void System::disable_tab_focus() { detail::Focus::disable_tab_focus(); }

void System::post_event(Event e)
{
    System::event_engine().queue().append(std::move(e));
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
    System::post_event(Resize_event{*System::head(), terminal.area()});
    detail::Focus::set(*head_);
    auto const exit_code = user_input_loop_.run();
    terminal.uninitialize();
    return exit_code;
}

void System::send_event(Event e)
{
    if (!std::visit([](auto const& e) { return detail::is_sendable(e); }, e))
        return;
    auto const handled =
        std::visit([](auto const& e) { return detail::filter_send(e); }, e);
    if (!handled)
        std::visit([](auto e) { detail::send(std::move(e)); }, std::move(e));
}

void System::send_event(Paint_event e)
{
    if (!detail::is_sendable(e))
        return;
    auto const handled = detail::filter_send(e);
    if (!handled)
        detail::send(std::move(e));
}

void System::send_event(Delete_event e)
{
    auto const handled = detail::filter_send(e);
    if (!handled)
        detail::send(std::move(e));
}

sl::Slot<void()> System::quit = [] { System::exit(); };
detail::Event_engine System::event_engine_;
Animation_engine System::animation_engine_;

// GCC requires this here, it can't find the default constructor when it's in
// system.hpp for whatever reason...
sl::Signal<void(int)> System::exit_signal;
detail::User_input_event_loop System::user_input_loop_;

}  // namespace ox
