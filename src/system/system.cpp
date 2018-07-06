#include <cppurses/system/system.hpp>

#include <memory>
#include <mutex>
#include <utility>

#include <signals/slot.hpp>

#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/painter/palette.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/detail/event_queue.hpp>
#include <cppurses/system/detail/ncurses_event_listener.hpp>
#include <cppurses/system/event.hpp>
#include <cppurses/system/event_loop.hpp>
#include <cppurses/system/events/on_tree_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/events/resize_event.hpp>
#include <cppurses/widget/layout.hpp>
#include <cppurses/widget/widget.hpp>

namespace cppurses {
namespace detail {
class Abstract_event_listener;
}  // namespace detail

sig::Slot<void()> System::quit = []() { System::exit(); };  // NOLINT

Widget* System::head_ = nullptr;  // NOLINT
Event_loop System::event_loop_;
Animation_engine System::animation_engine_;
std::unique_ptr<Paint_buffer> System::paint_buffer_ = nullptr;  // NOLINT
std::unique_ptr<detail::Abstract_event_listener> System::event_listener_ =
    std::make_unique<detail::NCurses_event_listener>();  // NOLINT

std::unique_ptr<Palette> System::system_palette_ = nullptr;  // NOLINT

void System::post_event(std::unique_ptr<Event> event) {
    System::event_loop_.event_queue.append(std::move(event));
}

bool System::send_event(const Event& event) {
    static std::recursive_mutex send_mtx;
    std::lock_guard<std::recursive_mutex> guard{send_mtx};
    bool handled = event.send_to_all_filters();
    if (!handled) {
        event.send();
    }
    return handled;
}

void System::exit(int return_code) {
    event_loop_.exit(return_code);
}

detail::Abstract_event_listener* System::event_listener() {
    return event_listener_.get();
}

Paint_buffer* System::paint_buffer() {
    return paint_buffer_.get();
}

void System::set_paint_buffer(std::unique_ptr<Paint_buffer> buffer) {
    paint_buffer_ = std::move(buffer);
    if (paint_buffer_ != nullptr) {
        System::post_event<Paint_event>(System::head());
    }
}

unsigned System::max_width() {
    if (System::paint_buffer() != nullptr) {
        return System::paint_buffer()->update_width();
    }
    return 0;
}

unsigned System::max_height() {
    if (System::paint_buffer() != nullptr) {
        return System::paint_buffer()->update_height();
    }
    return 0;
}

Widget* System::head() {
    return head_;
}

void System::set_palette(std::unique_ptr<Palette> palette) {
    system_palette_ = std::move(palette);
    system_palette_->initialize();
}

Palette* System::palette() {
    return system_palette_.get();
}

System::System() {
    System::set_paint_buffer(std::make_unique<Paint_buffer>());
    System::set_palette(std::make_unique<DawnBringer_palette>());
    this->disable_ctrl_characters();
}

System::~System() {
    System::set_paint_buffer(nullptr);
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

int System::run() {
    int return_code = event_loop_.run();
    return return_code;
}

}  // namespace cppurses
