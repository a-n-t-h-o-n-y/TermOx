#include <cppurses/system/system.hpp>

#include <algorithm>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <utility>

#include <signals/slot.hpp>

#include <cppurses/painter/detail/ncurses_data.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/detail/event_queue.hpp>
#include <cppurses/system/detail/user_input_event_loop.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/event_loop.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/system/terminal_properties.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {

sig::Slot<void()> System::quit = []() { System::exit(); };
std::vector<Event_loop*> System::running_event_loops_;
std::mutex System::running_loops_mtx_;
Widget* System::head_ = nullptr;
detail::User_input_event_loop System::main_loop_;
Animation_engine System::animation_engine_;
Terminal_properties System::terminal;
bool System::exit_requested_{false};

void System::post_event(std::unique_ptr<Event> event) {
    Event_loop& loop{System::find_event_loop()};
    loop.event_queue_.append(std::move(event));
}

bool System::send_event(const Event& event) {
    if (event.receiver() == nullptr ||
        (!event.receiver()->enabled() &&
         (event.type() != Event::Delete && event.type() != Event::Disable))) {
        return false;
    }
    bool handled = event.send_to_all_filters();
    if (!handled) {
        handled = event.send();
    }
    return handled;
}

bool System::exit_requested() {
    return exit_requested_;
}

void System::exit(int return_code) {
    animation_engine_.shutdown();
    std::lock_guard<std::mutex> lock{running_loops_mtx_};
    for (Event_loop* loop : running_event_loops_) {
        loop->exit(return_code);
    }
    System::exit_requested_ = true;
}

Event_loop& System::find_event_loop() {
    std::lock_guard<std::mutex> lock{running_loops_mtx_};
    std::thread::id id{std::this_thread::get_id()};
    for (Event_loop* loop : running_event_loops_) {
        if (loop->get_thread_id() == id) {
            return *loop;
        }
    }
    return main_loop_;
}

void System::register_event_loop(Event_loop* loop) {
    std::lock_guard<std::mutex> lock{running_loops_mtx_};
    running_event_loops_.push_back(loop);
}

void System::deregister_event_loop(Event_loop* loop) {
    std::lock_guard<std::mutex> lock{running_loops_mtx_};
    auto iter = std::find(std::begin(running_event_loops_),
                          std::end(running_event_loops_), loop);
    if (iter != std::end(running_event_loops_)) {
        running_event_loops_.erase(iter);
    }
}

Widget* System::head() {
    return head_;
}

System::System() {
    detail::NCurses_data::initialize();
    terminal.initialize();
}

System::~System() {
    animation_engine_.shutdown();
}

void System::set_head(Widget* head_widget) {
    if (head_ != nullptr) {
        head_->disable();
    }
    head_ = head_widget;
    if (head_ != nullptr) {
        head_->enable();
        System::post_event<Resize_event>(
            head_, Area{System::terminal.width(), System::terminal.height()});
        head_->update();
    }
}

Animation_engine& System::animation_engine() {
    return animation_engine_;
}

int System::run() {
    if (head_ == nullptr) {
        return -1;
    }
    return main_loop_.run();
}

}  // namespace cppurses
