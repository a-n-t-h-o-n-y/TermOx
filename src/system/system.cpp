#include <cppurses/system/system.hpp>

#include <algorithm>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include <signals/slot.hpp>

#include <cppurses/painter/palette.hpp>
#include <cppurses/painter/palettes.hpp>
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
    main_loop_.exit(return_code);
    System::exit_requested_ = true;
}

Event_loop& System::find_event_loop() {
    std::lock_guard<std::mutex> lock{running_loops_mtx_};
    std::thread::id id{std::this_thread::get_id()};
    // Check with the main loop
    // if (main_loop_.get_thread_id() == id) {
    //     return main_loop_;
    // }
    // Check with animation Loops
    // Event_loop* loop_ptr = System::animation_engine().get_event_loop(id);
    // if (loop_ptr != nullptr) {
    //     return *loop_ptr;
    // }
    // return main_loop_;
    for (Event_loop* loop : running_event_loops_) {
        if (loop->get_thread_id() == id) {
            return *loop;
        }
    }
    // should not get here
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
    System::terminal.set_color_palette(Palettes::DawnBringer());
    System::terminal.handle_control_characters(false);
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
    int return_code = main_loop_.run();
    return return_code;
}

}  // namespace cppurses
