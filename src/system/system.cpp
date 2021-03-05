#include <termox/system/system.hpp>

#include <algorithm>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <map>
#include <memory>
#include <shared_mutex>
#include <stdexcept>
#include <thread>
#include <utility>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/system/animation_engine.hpp>
#include <termox/system/detail/filter_send.hpp>
#include <termox/system/detail/focus.hpp>
#include <termox/system/detail/is_sendable.hpp>
#include <termox/system/detail/send.hpp>
#include <termox/system/detail/send_shortcut.hpp>
#include <termox/system/detail/user_input_event_loop.hpp>
#include <termox/system/event.hpp>
#include <termox/system/event_loop.hpp>
#include <termox/system/event_queue.hpp>
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

void System::post_event(Event e) { current_queue_.get().append(std::move(e)); }

void System::exit()
{
    user_input_loop_.exit(0);
    Terminal::uninitialize();
    std::quick_exit(0);
}

void System::set_head(Widget* new_head)
{
    if (auto* const head = head_.load(); head != nullptr)
        head->disable();
    if (new_head != nullptr) {
        new_head->enable();
        System::post_event(Resize_event{*new_head, Terminal::area()});
        detail::Focus::set(*new_head);
    }
    head_ = new_head;
}

auto System::run() -> int
{
    auto* const head = head_.load();
    if (head == nullptr)
        return -1;
    auto const result = user_input_loop_.run();
    // user_input_loop_ is already stopped if you are here.
    animation_engine_.stop();
    Terminal::stop_dynamic_color_engine();
    return result;
}

auto System::send_event(Event e) -> bool
{
    auto handled =
        std::visit([](auto const& e) { return detail::send_shortcut(e); }, e);
    if (!std::visit([](auto const& e) { return detail::is_sendable(e); }, e))
        return false;
    if (!handled) {
        handled =
            std::visit([](auto const& e) { return detail::filter_send(e); }, e);
    }
    if (!handled)
        std::visit([](auto e) { detail::send(std::move(e)); }, std::move(e));
    return true;
}

auto System::send_event(Paint_event e) -> bool
{
    if (!detail::is_sendable(e))
        return false;
    auto const handled = detail::filter_send(e);
    if (!handled)
        detail::send(std::move(e));
    return true;
}

auto System::send_event(Delete_event e) -> bool
{
    auto const handled = detail::filter_send(e);
    if (!handled)
        detail::send(std::move(e));
    return true;
}

sl::Slot<void()> System::quit = [] { System::exit(); };

detail::User_input_event_loop System::user_input_loop_;
Animation_engine System::animation_engine_;
std::reference_wrapper<Event_queue> System::current_queue_ =
    user_input_loop_.event_queue();

}  // namespace ox
