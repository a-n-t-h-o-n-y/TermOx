#include <painter_module/color.hpp>
#include <painter_module/detail/ncurses_paint_engine.hpp>
#include <painter_module/paint_engine.hpp>
#include <painter_module/palette.hpp>
#include <system_module/detail/posted_event.hpp>
#include <system_module/detail/thread_data.hpp>
#include <system_module/event.hpp>
#include <system_module/event_loop.hpp>
#include <system_module/events/paint_event.hpp>
#include <system_module/object.hpp>
#include <system_module/system.hpp>
#include <widget_module/widget.hpp>

#include <algorithm>
#include <iterator>
#include <memory>
#include <vector>

namespace mcurses {

void System::post_event(Object* obj,
                        std::unique_ptr<Event> event,
                        int priority) {
    if (obj == nullptr) {
        return;
    }
    detail::Posted_event pe(obj, std::move(event), priority);
    detail::Thread_data::current().event_queue.add_event(pe);
}

void System::remove_posted_event(Event* event) {
    detail::Posted_event_queue& queue =
        detail::Thread_data::current().event_queue;
    auto pos =
        std::find_if(std::begin(queue), std::end(queue),
                     [&event](auto& pe_) { return (&(pe_.event()) == event); });
    if (pos == std::end(queue)) {
        return;
    }
    queue.erase(pos);
}

bool System::send_event(Object* obj, Event& event) {
    return (obj != nullptr) ? notify(obj, event) : false;
}

void System::send_posted_events(Object* obj, Event::Type etype) {
    auto& queue = detail::Thread_data::current().event_queue;
    while (!queue.empty()) {
        auto& pe = queue.front();
        if ((obj == nullptr ||
             obj == pe.reciever()) &&  // can you clean this up
            (etype == Event::None || etype == pe.event().type()) &&
            ((pe.event().type() != Event::DeferredDelete) ||
             etype == Event::DeferredDelete)) {
            detail::Posted_event posted = queue.next_posted_event();
            if (posted.event().type() == Event::DeferredDelete) {
                Object* parent = obj->parent();
                if (parent == nullptr) {
                    if (obj == System::head()) {  // used to be parent == Sy...
                        System::set_head(nullptr);
                    }
                    return;
                }
                parent->delete_child(obj);
                return;
            }
            notify(posted.reciever(), posted.event());
        }
    }
}

bool System::notify(Object* obj, Event& event) {
    bool handled{false};

    // Send event to any filter objects
    unsigned i{0};
    while (i < obj->event_filter_objects_.size() &&
           !handled) {  // While loop in case event_filter invalidates iterators
        handled = obj->event_filter_objects_[i]->event_filter(obj, event);
        ++i;
    }
    if (handled) {
        return true;
    }

    return notify_helper(obj, event);
}

bool System::notify_helper(Object* obj, Event& event) {
    bool handled{false};
    // Send event to object
    handled = obj->event(event);

    // Propagate event to parent
    if (!handled) {  // && event type can propogate
        Object* parent = obj->parent();
        if (parent != nullptr) {
            handled = notify_helper(parent, event);
        }
    }
    return handled;
}

void System::exit(int return_code) {
    auto& data = detail::Thread_data::current();
    data.quit_now = true;
    // call exit on each event loop
    for (int i{0}; i < data.event_loops.size(); ++i) {
        data.event_loops.top()->exit(return_code);
        data.event_loops.pop();
    }
}

Paint_engine* System::paint_engine() {
    return engine_.get();
}

void System::set_paint_engine(std::unique_ptr<Paint_engine> engine) {
    engine_ = std::move(engine);
    if (engine_) {
        System::post_event(System::head(), std::make_unique<Paint_event>());
    }
}

unsigned System::max_width() {
    return System::paint_engine()->screen_width();
}

unsigned System::max_height() {
    return System::paint_engine()->screen_height();
}

Object* System::head_ = nullptr;
std::unique_ptr<Paint_engine> System::engine_ = nullptr;

Object* System::head() {
    return head_;
}

Widget* System::focus_widg_ = nullptr;

Widget* System::focus_widget() {
    return focus_widg_;
}

void System::set_focus_widget(Widget* widg) {
    if (focus_widg_ != nullptr) {
        focus_widg_->clear_focus();
    }
    focus_widg_ = widg;
    focus_widg_->set_focus(true);
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
    Object* current_focus_object = static_cast<Object*>(System::focus_widget());
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
        Widget* widg = dynamic_cast<Widget*>(child);
        if (widg != nullptr) {
            if (widg->focus_policy() == Widget::Focus_policy::TabFocus ||
                widg->focus_policy() == Widget::Focus_policy::StrongFocus) {
                System::set_focus_widget(widg);
                return;
            }
        }
    }
}

std::unique_ptr<Palette> System::system_palette_ = nullptr;

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

    // Set all static objects to their default state(mainly for testing)
    // there is a static variable in a function somewhere.
}

void System::set_head(Object* obj) {
    head_ = obj;
}

int System::run() {
    Event_loop main_loop;

    detail::Thread_data& data = detail::Thread_data::current();
    data.event_loops.push(&main_loop);
    data.quit_now = false;

    int return_code = main_loop.run();

    data.quit_now = false;

    return return_code;
}

}  // namespace mcurses
