#include "system/system.hpp"
#include "painter/color.hpp"
#include "painter/paint_engine.hpp"
#include "painter/palette.hpp"
// #include "system/detail/posted_event.hpp"
// #include "system/detail/thread_data.hpp"
#include "system/event.hpp"
#include "system/event_loop.hpp"
#include "system/object.hpp"
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

Object* System::head_ = nullptr;                          // NOLINT
std::unique_ptr<Paint_engine> System::engine_ = nullptr;  // NOLINT
std::unique_ptr<detail::Abstract_event_listener> System::event_listener_ =
    std::make_unique<detail::NCurses_event_listener>();  // NOLINT

std::unique_ptr<Palette> System::system_palette_ = nullptr;  // NOLINT
Widget* System::focus_widg_ = nullptr;                       // NOLINT

void System::post_event(std::unique_ptr<Event> event) {
    event_loop_.event_queue.append(std::move(event));
}

bool System::send_event(const Event& event) {
    bool handled = event.send_to_all_filters();
    if (!handled) {
        event.send();
    }
    return handled;
}

// void System::post_event(Object* obj,
//                         std::unique_ptr<Event> event,
//                         int priority) {
//     if (obj == nullptr) {
//         return;
//     }
//     detail::Posted_event pe{obj, std::move(event), priority};
//     detail::Thread_data::current().event_queue.add_event(std::move(pe));
// }

// void System::remove_posted_event(Event* event) {
//     auto& queue = detail::Thread_data::current().event_queue;
//     auto pos =
//         std::find_if(std::begin(queue), std::end(queue),
//                      [&event](auto& pe) { return (&(pe.event()) == event);
//                      });
//     if (pos != std::end(queue)) {
//         queue.erase(pos);
//     }
// }

// bool System::send_event(Object* obj, const Event& event) {
//     return (obj != nullptr) ? notify(obj, event) : false;
// }

// void System::send_posted_events(Object* obj_filter, Event::Type etype_filter)
// {
//     auto& queue = detail::Thread_data::current().event_queue;
//     auto posted_iter = std::begin(queue);
//     while (posted_iter != std::end(queue)) {
//         if (posted_iter->receiver() != nullptr) {
//             auto event_t = posted_iter->event().type();
//             if ((obj_filter == nullptr ||
//                  obj_filter == posted_iter->receiver()) &&
//                 (etype_filter == Event::None ||
//                  etype_filter == posted_iter->event().type()) &&
//                 (posted_iter->event().type() != Event::DeferredDelete ||
//                  etype_filter == Event::DeferredDelete)) {
//                 if (event_t == Event::DeferredDelete) {
//                     auto parent = posted_iter->receiver()->parent();
//                     if (parent == nullptr) {
//                         if (posted_iter->receiver() == System::head()) {
//                             System::exit();
//                         }
//                         queue.erase(posted_iter);
//                         posted_iter = std::begin(queue);
//                     } else {
//                         parent->delete_child(
//                             const_cast<Object*>(posted_iter->receiver()));
//                         posted_iter = std::begin(queue);
//                         queue.erase(posted_iter);
//                         posted_iter = std::begin(queue);
//                     }
//                 } else {
//                     // const_cast because of multi-set const only iterator.
//                     System::notify(const_cast<Object*>(posted_iter->receiver()),
//                                    posted_iter->event());
//                     posted_iter = std::begin(queue);
//                     queue.erase(posted_iter);
//                     posted_iter = std::begin(queue);
//                 }
//             } else {
//                 ++posted_iter;
//             }
//         } else {
//             ++posted_iter;
//         }
//     }
// }

// bool System::notify(Object* obj, const Event& event) {
//     bool handled{false};
//     // Send event to any filter objects
//     unsigned i{0};
//     while (i < obj->event_filter_objects_.size() && !handled) {
//         handled = obj->event_filter_objects_[i]->event_filter(obj, event);
//         ++i;
//     }
//     if (handled) {
//         return true;
//     }

//     return notify_helper(obj, event);
// }

// bool System::notify_helper(Object* obj, const Event& event) {
//     bool handled{false};
//     // Send event to object
//     handled = obj->event(event);
//     // Propagate event to parent
//     if (!handled) {
//         Object* parent = obj->parent();
//         if (parent != nullptr) {
//             handled = notify_helper(parent, event);
//         }
//     }
//     return handled;
// }

void System::exit(int return_code) {
    // auto& data = detail::Thread_data::current();
    // data.quit_now = true;
    // for (auto loop_ptr : data.event_loops) {
    //     loop_ptr->exit(return_code);
    // }
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
        // TODO Remove all static casts to widget once object is gone, or make
        // everything refer to widgets if you keep object base class.
        System::post_event(std::make_unique<Paint_event>(
            static_cast<Widget*>(System::head())));
    }
}

unsigned System::max_width() {
    return System::paint_engine()->screen_width();
}

unsigned System::max_height() {
    return System::paint_engine()->screen_height();
}

Object* System::head() {
    return head_;
}

Widget* System::focus_widget() {
    return focus_widg_;
}

void System::set_focus_widget(Widget* widg, bool clear_focus) {
    if (widg == focus_widg_) {
        return;
    }
    // Old focus widget
    if (focus_widg_ != nullptr && clear_focus) {
        focus_widg_->clear_focus();
    }
    // New focus widget
    focus_widg_ = widg;
    if (widg != nullptr) {
        focus_widg_->set_focus(true);
    }
}

void System::cycle_tab_focus() {
    if (System::head() == nullptr) {
        return;
    }
    std::vector<Object*> objects;

    // Populate objects vector
    objects.push_back(System::head());
    int i{0};
    while (i < objects.size()) {
        Object* current = objects[i];
        auto children = current->children();
        std::for_each(std::begin(children), std::end(children),
                      [&objects](Object* p) { objects.push_back(p); });
        ++i;
    }

    // Rearrange objects vector
    auto* current_focus_object = static_cast<Object*>(System::focus_widget());
    if (current_focus_object != nullptr) {
        auto current_iter = std::find(std::begin(objects), std::end(objects),
                                      current_focus_object);
        if (current_iter != std::end(objects)) {
            std::move(std::begin(objects), current_iter + 1,
                      std::back_inserter(objects));
        }
    }

    // Find the next focus widget
    for (Object* child : objects) {
        auto* widg = dynamic_cast<Widget*>(child);
        if (widg != nullptr) {
            if (widg->focus_policy() == Focus_policy::Tab ||
                widg->focus_policy() == Focus_policy::Strong) {
                System::set_focus_widget(widg);
                widg->paint_engine().move(widg->x() + widg->cursor_x(),
                                          widg->y() + widg->cursor_y());
                return;
            }
        }
    }
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

void System::set_head(Object* obj) {
    head_ = obj;
}

int System::run() {
    // Event_loop main_loop;
    // auto& data = detail::Thread_data::current();
    // data.quit_now = false;
    int return_code = event_loop_.run();
    // data.quit_now = false;
    return return_code;
}

}  // namespace cppurses
