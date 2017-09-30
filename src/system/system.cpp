#include "system/system.hpp"
#include "painter/color.hpp"
#include "painter/paint_engine.hpp"
#include "painter/palette.hpp"
#include "system/event.hpp"
#include "system/event_loop.hpp"
#include "system/events/paint_event.hpp"
#include "system/detail/ncurses_event_listener.hpp"
#include "widget/widget.hpp"
#include "widget/focus_policy.hpp"
#include <signals/signals.hpp>
#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>

namespace cppurses {

sig::Slot<void()> System::quit = []() { System::exit(); };  // NOLINT

Widget* System::head_ = nullptr;  // NOLINT
Event_loop System::event_loop_;
std::unique_ptr<Paint_engine> System::engine_ = nullptr;  // NOLINT
std::unique_ptr<detail::Abstract_event_listener> System::event_listener_ =
    std::make_unique<detail::NCurses_event_listener>();  // NOLINT

std::unique_ptr<Palette> System::system_palette_ = nullptr;  // NOLINT

void System::post_event(std::unique_ptr<Event> event) {
    System::event_loop_.event_queue.append(std::move(event));
}

bool System::send_event(const Event& event) {
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

Paint_engine* System::paint_engine() {
    return engine_.get();
}

void System::set_paint_engine(std::unique_ptr<Paint_engine> engine) {
    engine_ = std::move(engine);
    if (engine_) {
        System::post_event<Paint_event>(System::head());
    }
}

unsigned System::max_width() {
    return System::paint_engine()->screen_width();
}

unsigned System::max_height() {
    return System::paint_engine()->screen_height();
}

Widget* System::head() {
    return head_;
}

void System::set_palette(std::unique_ptr<Palette> palette) {
    system_palette_ = std::move(palette);
    Palette* p = System::palette();

    System::paint_engine()->set_rgb(Color::Black, p->red_value(Color::Black),
                                    p->green_value(Color::Black),
                                    p->blue_value(Color::Black));
    System::paint_engine()->set_rgb(
        Color::Dark_red, p->red_value(Color::Dark_red),
        p->green_value(Color::Dark_red), p->blue_value(Color::Dark_red));
    System::paint_engine()->set_rgb(
        Color::Dark_blue, p->red_value(Color::Dark_blue),
        p->green_value(Color::Dark_blue), p->blue_value(Color::Dark_blue));
    System::paint_engine()->set_rgb(
        Color::Dark_gray, p->red_value(Color::Dark_gray),
        p->green_value(Color::Dark_gray), p->blue_value(Color::Dark_gray));
    System::paint_engine()->set_rgb(Color::Brown, p->red_value(Color::Brown),
                                    p->green_value(Color::Brown),
                                    p->blue_value(Color::Brown));
    System::paint_engine()->set_rgb(Color::Green, p->red_value(Color::Green),
                                    p->green_value(Color::Green),
                                    p->blue_value(Color::Green));
    System::paint_engine()->set_rgb(Color::Red, p->red_value(Color::Red),
                                    p->green_value(Color::Red),
                                    p->blue_value(Color::Red));
    System::paint_engine()->set_rgb(Color::Gray, p->red_value(Color::Gray),
                                    p->green_value(Color::Gray),
                                    p->blue_value(Color::Gray));
    System::paint_engine()->set_rgb(Color::Blue, p->red_value(Color::Blue),
                                    p->green_value(Color::Blue),
                                    p->blue_value(Color::Blue));
    System::paint_engine()->set_rgb(Color::Orange, p->red_value(Color::Orange),
                                    p->green_value(Color::Orange),
                                    p->blue_value(Color::Orange));
    System::paint_engine()->set_rgb(
        Color::Light_gray, p->red_value(Color::Light_gray),
        p->green_value(Color::Light_gray), p->blue_value(Color::Light_gray));
    System::paint_engine()->set_rgb(
        Color::Light_green, p->red_value(Color::Light_green),
        p->green_value(Color::Light_green), p->blue_value(Color::Light_green));
    System::paint_engine()->set_rgb(Color::Violet, p->red_value(Color::Violet),
                                    p->green_value(Color::Violet),
                                    p->blue_value(Color::Violet));
    System::paint_engine()->set_rgb(
        Color::Light_blue, p->red_value(Color::Light_blue),
        p->green_value(Color::Light_blue), p->blue_value(Color::Light_blue));
    System::paint_engine()->set_rgb(Color::Yellow, p->red_value(Color::Yellow),
                                    p->green_value(Color::Yellow),
                                    p->blue_value(Color::Yellow));
    System::paint_engine()->set_rgb(Color::White, p->red_value(Color::White),
                                    p->green_value(Color::White),
                                    p->blue_value(Color::White));
    System::paint_engine()->flush(false);
}

Palette* System::palette() {
    return system_palette_.get();
}

System::System(std::unique_ptr<Paint_engine> engine) {
    System::set_paint_engine(std::move(engine));
    System::set_palette(std::make_unique<DawnBringer_palette>());
}

System::~System() {
    System::set_paint_engine(nullptr);
}

void System::set_head(Widget* head_widget) {
    head_ = head_widget;
}

int System::run() {
    int return_code = event_loop_.run();
    return return_code;
}

}  // namespace cppurses
