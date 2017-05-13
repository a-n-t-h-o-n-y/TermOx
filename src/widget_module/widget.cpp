#include <painter_module/brush.hpp>
#include <painter_module/color.hpp>
#include <painter_module/painter.hpp>
#include <system_module/events/close_event.hpp>
#include <system_module/events/enable_event.hpp>
#include <system_module/events/focus_event.hpp>
#include <system_module/events/hide_event.hpp>
#include <system_module/events/key_event.hpp>
#include <system_module/events/mouse_event.hpp>
#include <system_module/events/move_event.hpp>
#include <system_module/events/paint_event.hpp>
#include <system_module/events/resize_event.hpp>
#include <system_module/events/show_event.hpp>
#include <system_module/events/child_event.hpp>
#include <system_module/system.hpp>
#include <widget_module/widget.hpp>
#include "widget_module/border.hpp"

#include <memory>

namespace twf {

Widget::Widget() {
    this->Widget::initialize();
    this->brush().set_background(Color::Black);
    this->brush().set_foreground(Color::White);
    this->update();
}

Widget::~Widget() {
    if (System::focus_widget() == this) {
        System::set_focus_widget(nullptr, false);
    }
}

void Widget::initialize() {
    this->close = [this]() {
        System::post_event(this, std::make_unique<Close_event>());
    };
    this->close.track(this->destroyed);

    this->hide = [this]() { this->set_visible(false); };
    this->hide.track(this->destroyed);

    this->show = [this]() { this->set_visible(true); };
    this->show.track(this->destroyed);

    this->repaint = [this]() {
        Paint_event e;  // change this to a single line.
        System::send_event(this, e);
    };
    this->repaint.track(this->destroyed);

    this->give_focus = [this]() { std::bind(&Widget::set_focus, this, true); };
    this->give_focus.track(this->destroyed);

    this->update_me = [this]() { std::bind(&Widget::update, this); };
    this->update_me.track(this->destroyed);
}

void Widget::set_x(std::size_t global_x) {
    auto parent = dynamic_cast<Widget*>(this->parent());
    if (parent != nullptr) {
        position_.x = global_x - parent->x();
    } else {
        position_.x = global_x;
    }
}

void Widget::set_y(std::size_t global_y) {
    auto parent = dynamic_cast<Widget*>(this->parent());
    if (parent != nullptr) {
        position_.y = global_y - parent->y();
    } else {
        position_.y = global_y;
    }
}

void Widget::set_background(Color c) {
    this->brush().set_background(c);
    this->update();
}

void Widget::set_foreground(Color c) {
    this->brush().set_foreground(c);
    this->update();
}

bool Widget::has_coordinates(std::size_t global_x, std::size_t global_y) {
    if (!this->enabled() || !this->visible()) {
        return false;
    }
    // too messy, subfunctions - lambdas
    return global_x >= (this->x() + west_border_offset(this->border())) &&
           global_x < (this->x() + this->width() +
                       west_border_offset(this->border())) &&
           global_y >= (this->y() + north_border_offset(this->border())) &&
           global_y < (this->y() + this->height() +
                       north_border_offset(this->border()));
}

void Widget::enable_border() {
    this->border_.enable();
    System::post_event(this->parent(), std::make_unique<Child_event>(
                                           Event::ChildPolished, this));
}

void Widget::disable_border() {
    this->delete_later();
}

void Widget::set_geometry(const Geometry& g) {
    geometry_.set_widget(nullptr);  // prob not needed
    geometry_ = g;
    geometry_.set_widget(dynamic_cast<Widget*>(this->parent()));
    this->update();
}

void Widget::update() {
    this->clear_screen();
    System::post_event(this, std::make_unique<Paint_event>());
}

void Widget::update_now() {
    this->update();
    twf::System::send_posted_events();
}

void Widget::clear_screen() {
    System::post_event(this, std::make_unique<Clear_screen_event>());
}

bool Widget::event(const Event& event) {
    auto visible_and_enabled = [this]() {
        return this->visible() && this->enabled();
    };

    // Move_event
    if (event.type() == Event::Move) {
        return this->move_event(static_cast<const Move_event&>(event));
    }

    // Resize_event
    if (event.type() == Event::Resize) {
        return this->resize_event(static_cast<const Resize_event&>(event));
    }

    // Paint_event
    if (event.type() == Event::Paint) {
        // handled here so that virtual functions work from derived widgets.
        if (visible_and_enabled()) {
            return this->paint_event(static_cast<const Paint_event&>(event));
        }
        if (this->visible() && !this->enabled()) {
            // this->paint_disabled_widget();
        }
        return true;
    }

    // ClearScreen_event
    if (event.type() == Event::ClearScreen) {
        return this->clear_screen_event(
            static_cast<const Clear_screen_event&>(event));
    }

    // Mouse_events
    if (event.type() == Event::MouseButtonPress) {
        // should most of this be in event handler function?
        if (visible_and_enabled()) {
            if (this->focus_policy() == Focus_policy::ClickFocus ||
                this->focus_policy() == Focus_policy::StrongFocus) {
                System::set_focus_widget(this);
                this->paint_engine().move(this->x() + this->cursor_x(),
                                          this->y() + this->cursor_y());
            }
            return this->mouse_press_event(
                static_cast<const Mouse_event&>(event));
        }
        return true;
    }  // continue change from here
    if (event.type() == Event::MouseButtonRelease) {
        if (visible_and_enabled()) {
            return this->mouse_release_event(
                static_cast<const Mouse_event&>(event));
        }
        return true;
    }
    if (event.type() == Event::MouseButtonDblClick) {
        if (visible_and_enabled()) {
            return this->mouse_double_click_event(
                static_cast<const Mouse_event&>(event));
        }
        return true;
    }
    if (event.type() == Event::Wheel) {
        if (!this->enabled() || !this->visible()) {
            return true;
        }
        return this->wheel_event(static_cast<const Mouse_event&>(event));
    }
    if (event.type() == Event::MouseMove && this->has_mouse_tracking()) {
        if (!this->enabled() || !this->visible()) {
            return true;
        }
        return this->mouse_move_event(static_cast<const Mouse_event&>(event));
    }

    // KeyEvent
    if (event.type() == Event::KeyPress) {
        if (!this->enabled() || !this->visible()) {
            return true;  // is this the right response? or false?
        }
        return this->key_press_event(static_cast<const Key_event&>(event));
    }
    if (event.type() == Event::KeyRelease) {
        if (!this->enabled() || !this->visible()) {
            return true;
        }
        return this->key_release_event(static_cast<const Key_event&>(event));
    }

    // Close Event
    if (event.type() == Event::Close) {
        return this->close_event(static_cast<const Close_event&>(event));
    }

    // Hide Event
    if (event.type() == Event::Hide) {
        return this->hide_event(static_cast<const Hide_event&>(event));
    }

    // Show Event
    if (event.type() == Event::Show) {
        return this->show_event(static_cast<const Show_event&>(event));
    }

    // Focus Event
    if (event.type() == Event::FocusIn || event.type() == Event::FocusOut) {
        return this->focus_event(static_cast<const Focus_event&>(event));
    }

    return Object::event(event);
}

bool Widget::child_event(const Child_event& event) {
    this->update();
    return Object::child_event(event);
}

bool Widget::move_event(const Move_event& event) {
    this->set_x(event.new_x());
    this->set_y(event.new_y());
    // Widget* parent = dynamic_cast<Widget*>(this->parent()); // causes infinte
    // loop
    // if (parent) { parent->update(); }
    // maybe send parent a child event?
    this->update();
    return true;
}

bool Widget::resize_event(const Resize_event& event) {
    this->geometry().set_width(event.new_width());
    this->geometry().set_height(event.new_height());
    this->update();
    return true;
}

bool Widget::paint_event(const Paint_event& event) {
    if (border_.enabled()) {
        Painter p{this};
        p.border(border_);
    }
    this->paint_engine().move(this->x() + this->cursor_x(),
                              this->y() + this->cursor_y());
    // for (Object* c : this->children()) {
    //     Widget* child = dynamic_cast<Widget*>(c);
    //     if (child != nullptr) {
    //         child->update();
    //     }
    // }
    return true;
}

bool Widget::clear_screen_event(const Clear_screen_event& event) {
    Painter p{this};
    p.clear_screen();
    return true;
}

// void Widget::erase_widget_screen() {
//     if (this->y() + this->geometry().height() > System::max_height()) {
//         return;
//     }
//     if (this->x() + this->geometry().width() > System::max_width()) {
//         return;
//     }
//     Painter p{this};
//     if (this->brush().background_color()) {
//         p.fill(0, 0, this->geometry().width(), this->geometry().height(),
//                *this->brush().background_color());
//     }
// }

// void Widget::paint_disabled_widget() {
//     // Re-implement this to change the brush to grey and repaint, this might be
//     // done
//     // elsewhere when the widget is disabled, the brush is changed to greyscale,
//     // then
//     // a typical update is done. The function will probably not be needed.
//     Painter p{this};
//     Widget* parent = dynamic_cast<Widget*>(this->parent());
//     Color background = Color::Black;
//     if (parent != nullptr && parent->brush().background_color()) {
//         background = *parent->brush().background_color();
//     }
//     p.fill(0, 0, this->geometry().width(), this->geometry().height(),
//            background);
// }

bool Widget::mouse_press_event(const Mouse_event& event) {
    return false;
}

bool Widget::mouse_release_event(const Mouse_event& event) {
    return false;
}

bool Widget::mouse_double_click_event(const Mouse_event& event) {
    return false;
}

bool Widget::wheel_event(const Mouse_event& event) {
    return false;
}

bool Widget::mouse_move_event(const Mouse_event& event) {
    return false;
}

bool Widget::key_press_event(const Key_event& event) {
    return false;
}

bool Widget::key_release_event(const Key_event& event) {
    return false;
}

bool Widget::close_event(const Close_event& event) {
    this->delete_later();
    return true;
}
// implement this here
bool Widget::hide_event(const Hide_event& event) {
    return true;
}

// implement this here
bool Widget::show_event(const Show_event& event) {
    return true;
}

bool Widget::enable_event(const Enable_event& event) {
    if (!event.is_enabled()) {
        // save current brush and
        // set brush to greyscale
        return true;  // ? you'll figure it out
    } else {
        // set brush back to saved original
    }
    return Object::enable_event(event);
}

bool Widget::focus_event(const Focus_event& event) {
    if (event.type() == Event::FocusIn) {
        Painter p{this};
        p.set_cursor(this->cursor());
        p.move(cursor_position_.x, cursor_position_.y);
    }  // if(event.type() == FocusOut)
    return true;
}

void Widget::set_focus(bool focus) {
    if (this->focus_policy() == Focus_policy::NoFocus) {
        return;
    }
    focus_ = focus;
    if (focus) {
        System::post_event(this, std::make_unique<Focus_event>(Event::FocusIn));
    } else {
        System::post_event(this,
                           std::make_unique<Focus_event>(Event::FocusOut));
    }
}

void Widget::set_brush_recursively(const Brush& brush) {
    this->default_brush_ = brush;
    this->update();
    for (Object* c : this->children()) {
        Widget* child = dynamic_cast<Widget*>(c);
        if (child != nullptr) {
            child->set_brush_recursively(brush);
        }
    }
}

void Widget::set_visible(bool visible) {
    this->visible_ = visible;
    if (visible) {
        Show_event show;
        System::send_event(this, show);
    } else {
        Hide_event hide;
        System::send_event(this, hide);
    }
    for (Object* c : this->children()) {
        Widget* child = dynamic_cast<Widget*>(c);
        if (child != nullptr) {
            child->set_visible(visible);
        }
    }
}

std::size_t Widget::x() const {
    // Widget* parent_widg = dynamic_cast<Widget*>(
    //     this->parent());
    // if (parent_widg == nullptr) {
    //     return this->position_.x;
    // }
    if (this->parent() == nullptr) {
        return this->position_.x;
    }
    // Object* tree should only contain Widget* this is something to change, to
    // create a Widget* tree instead, dynamic_cast from above is too large a
    // performance hit for a function that is used so many times.
    Widget* parent_widg = static_cast<Widget*>(this->parent());
    return this->position_.x + parent_widg->x();
}

std::size_t Widget::y() const {
    // Widget* parent_widg = dynamic_cast<Widget*>(this->parent());
    // if (parent_widg == nullptr) {
    //     return this->position_.y;
    // }
    if (this->parent() == nullptr) {
        return this->position_.x;
    }
    Widget* parent_widg = static_cast<Widget*>(this->parent());
    return this->position_.y + parent_widg->y();
}

Paint_engine& Widget::paint_engine() const {
    if (paint_engine_) {
        return *paint_engine_;
    }
    return *System::paint_engine();
}

std::size_t Widget::width() const {
    std::size_t width_border_offset =
        west_border_offset(this->border()) + east_border_offset(this->border());
    std::size_t w = this->geometry().width();
    if (width_border_offset > w) {
        return 0;
    }
    return w - width_border_offset;
}

std::size_t Widget::height() const {
    std::size_t height_border_offset = north_border_offset(this->border()) +
                                       south_border_offset(this->border());
    std::size_t h = this->geometry().height();
    if (height_border_offset > h) {
        return 0;
    }
    return h - height_border_offset;
}

}  // namespace twf
