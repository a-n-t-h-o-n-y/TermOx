#include <cppurses/system/system.hpp>

#include <memory>
#include <mutex>
#include <utility>

#include <signals/slot.hpp>

// #include <cppurses/painter/detail/ncurses_paint_engine.hpp>
#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/detail/event_queue.hpp>
#include <cppurses/system/detail/ncurses_event_listener.hpp>
#include <cppurses/system/detail/repaint_all.hpp>
#include <cppurses/system/detail/user_input_event_loop.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/event_loop.hpp>
#include <cppurses/system/events/on_tree_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/widget/layout.hpp>
#include <cppurses/widget/widget.hpp>

// Temps
// #include <cppurses/system/detail/event_as_string.hpp>
// #include <utility/log.hpp>

namespace cppurses {
namespace detail {
class Abstract_event_listener;
}  // namespace detail

sig::Slot<void()> System::quit = []() { System::exit(); };
Widget* System::head_ = nullptr;
detail::User_input_event_loop System::main_loop_;
Animation_engine System::animation_engine_;
// detail::NCurses_paint_engine System::paint_engine_;
Paint_buffer System::paint_buffer_;
std::unique_ptr<detail::Abstract_event_listener> System::event_listener_ =
    std::make_unique<detail::NCurses_event_listener>();
std::unique_ptr<Palette> System::system_palette_ = nullptr;

void System::post_event(std::unique_ptr<Event> event) {
    Event_loop& loop{System::find_event_loop()};
    loop.event_queue.append(std::move(event));
}

bool System::send_event(const Event& event) {
    // static std::recursive_mutex send_mtx;
    // std::lock_guard<std::recursive_mutex> guard{send_mtx};

    // static int recursion_levels{0};
    // static long call_n{0};
    // utility::Log l;
    // ++call_n;
    bool handled = event.send_to_all_filters();
    if (!handled) {
        // l << call_n << " call to send_event()\n";
        // l << recursion_levels++ << " Recursion Levels Deep.\n";
        // l << "Event Type: " << detail::event_as_string(event) << '\n';
        // l << "Event Receiver: " << event.receiver() << '\n' << std::endl;
        handled = event.send();
    }
    // --recursion_levels;
    return handled;
}

void System::exit(int return_code) {
    animation_engine_.shutdown();
    main_loop_.exit(return_code);
}

Event_loop& System::find_event_loop() {
    std::thread::id id{std::this_thread::get_id()};
    // Check with the main loop
    if (main_loop_.get_thread_id() == id) {
        return main_loop_;
    }
    // Check with animation Loops
    // TODO Reimplement this cleanly
    Event_loop* loop_ptr = System::animation_engine().get_event_loop(id);
    if (loop_ptr != nullptr) {
        return *loop_ptr;
    }
    // for (Animation_loop& al : System::animation_engine().loops) {
    //     if (al.loop.get_thread_id() == id) {
    //         return al.loop;
    //     }
    // }
    return main_loop_;
}

detail::Abstract_event_listener* System::event_listener() {
    return event_listener_.get();
}

Paint_buffer& System::paint_buffer() {
    return paint_buffer_;
}

std::size_t System::max_width() {
    System::paint_buffer().update_width();
    return System::paint_buffer().screen_width();
}

std::size_t System::max_height() {
    System::paint_buffer().update_height();
    return System::paint_buffer().screen_height();
}

Widget* System::head() {
    return head_;
}

void System::set_palette(std::unique_ptr<Palette> palette) {
    system_palette_ = std::move(palette);
    system_palette_->initialize();
    detail::repaint_all();
}

Palette* System::palette() {
    return system_palette_.get();
}

System::System() {
    System::set_palette(std::make_unique<DawnBringer_palette>());
    this->disable_ctrl_characters();
}

System::~System() {
    animation_engine_.shutdown();
}

void System::set_head(Widget* head_widget) {
    if (head_ != nullptr) {
        System::post_event<On_tree_event>(head_, false);
    }
    head_ = head_widget;
    if (head_ != nullptr) {
        System::post_event<On_tree_event>(head_, true);
        if (dynamic_cast<Layout*>(head_) != nullptr) {
            System::post_event<Resize_event>(
                head_, Area{System::max_width(), System::max_height()});
        }
        head_->update();
    }
}

void System::enable_ctrl_characters() {
    event_listener_->enable_ctrl_characters();
}

void System::disable_ctrl_characters() {
    event_listener_->disable_ctrl_characters();
}

Animation_engine& System::animation_engine() {
    return animation_engine_;
}

// detail::NCurses_paint_engine& System::paint_engine() {
//     return paint_engine_;
// }

int System::run() {
    int return_code = main_loop_.run();
    return return_code;
}

}  // namespace cppurses
