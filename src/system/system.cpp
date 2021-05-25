#include <termox/system/system.hpp>

#include <cstdlib>
#include <functional>
#include <utility>
#include <variant>

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

System::System(Mouse_mode mouse_mode, Signals signals)
{
    Terminal::initialize(mouse_mode, signals);
}

System::~System()
{
    System::exit();
    Terminal::uninitialize();
}

auto System::focus_widget() -> Widget* { return detail::Focus::focus_widget(); }

void System::set_focus(Widget& w) { detail::Focus::set(w); }

void System::clear_focus() { detail::Focus::clear(); }

void System::enable_tab_focus() { detail::Focus::enable_tab_focus(); }

void System::disable_tab_focus() { detail::Focus::disable_tab_focus(); }

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

auto System::head() -> Widget* { return head_.load(); }

auto System::run(Widget& head) -> int
{
    System::set_head(&head);
    return this->run();
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

void System::post_event(Event e) { current_queue_.get().append(std::move(e)); }

void System::exit()
{
    user_input_loop_.exit(0);
    Terminal::uninitialize();
    std::_Exit(0);
}

void System::enable_animation(Widget& w, Animation_engine::Interval_t interval)
{
    if (!animation_engine_.is_running())
        animation_engine_.start();
    animation_engine_.register_widget(w, interval);
}

void System::enable_animation(Widget& w, FPS fps)
{
    if (!animation_engine_.is_running())
        animation_engine_.start();
    animation_engine_.register_widget(w, fps);
}

void System::disable_animation(Widget& w)
{
    animation_engine_.unregister_widget(w);
}

void System::set_cursor(Cursor cursor, Point offset)
{
    if (!cursor.is_enabled())
        Terminal::show_cursor(false);
    else {
        Terminal::move_cursor(
            {offset.x + cursor.position().x, offset.y + cursor.position().y});
        Terminal::show_cursor();
    }
}

void System::set_current_queue(Event_queue& queue) { current_queue_ = queue; }

sl::Slot<void()> System::quit = [] { System::exit(); };

detail::User_input_event_loop System::user_input_loop_;
Animation_engine System::animation_engine_;
std::reference_wrapper<Event_queue> System::current_queue_ =
    user_input_loop_.event_queue();

}  // namespace ox
