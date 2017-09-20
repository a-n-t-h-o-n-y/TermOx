#include "system/system.hpp"
#include "painter/color.hpp"
#include "painter/paint_engine.hpp"
#include "painter/palette.hpp"
// #include "system/detail/posted_event.hpp"
// #include "system/detail/thread_data.hpp"
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

// Widget* System::focus_widget() {
//     return focus_widg_;
// }

// void System::set_focus_widget(Widget* widg, bool clear_focus) {
//     if (widg == focus_widg_) {
//         return;
//     }
//     // Old focus widget
//     if (focus_widg_ != nullptr && clear_focus) {
//         focus_widg_->clear_focus();
//     }
//     // New focus widget
//     focus_widg_ = widg;
//     if (widg != nullptr) {
//         focus_widg_->set_focus(true);
//     }
// }

// void System::cycle_tab_focus() {
//     if (System::head() == nullptr) {
//         return;
//     }
//     std::vector<Widget*> widgets;

//     // Populate widgets vector
//     widgets.push_back(System::head());
//     int i{0};
//     while (i < widgets.size()) {
//         Widget* current = widgets[i];
//         auto children = current->children();
//         std::for_each(std::begin(children), std::end(children),
//                       [&widgets](Widget* p) { widgets.push_back(p); });
//         ++i;
//     }

//     // Rearrange widgets vector
//     auto* current_focus_widget = System::focus_widget();
//     if (current_focus_widget != nullptr) {
//         auto current_iter = std::find(std::begin(widgets), std::end(widgets),
//                                       current_focus_widget);
//         if (current_iter != std::end(widgets)) {
//             std::move(std::begin(widgets), current_iter + 1,
//                       std::back_inserter(widgets));
//         }
//     }

//     // Find the next focus widget
//     for (Widget* child : widgets) {
//         auto* widg = child;
//         if (widg != nullptr) {
//             if (widg->focus_policy() == Focus_policy::Tab ||
//                 widg->focus_policy() == Focus_policy::Strong) {
//                 System::set_focus_widget(widg);
//                 widg->paint_engine().move(widg->x() + widg->cursor_x(),
//                                           widg->y() + widg->cursor_y());
//                 return;
//             }
//         }
//     }
// }

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
