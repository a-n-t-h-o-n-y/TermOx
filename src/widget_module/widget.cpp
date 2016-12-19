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
#include <system_module/system.hpp>
#include <widget_module/widget.hpp>

#include <memory>

namespace mcurses {

Widget::Widget() {
    this->initialize();
    this->brush().set_background(Color::Black);
    this->brush().set_foreground(Color::White);
    this->update();
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
        Paint_event e;
        System::send_event(this, e);
    };
    this->repaint.track(this->destroyed);

    this->give_focus = [this]() { std::bind(&Widget::set_focus, this, true); };
    this->give_focus.track(this->destroyed);

    this->update_me = [this]() { std::bind(&Widget::update, this); };
    this->update_me.track(this->destroyed);
}

bool Widget::has_coordinates(std::size_t glob_x, std::size_t glob_y) {
    if (!this->is_enabled() || !this->visible()) {
        return false;
    }
    return (glob_x >= this->global_x()) && (glob_x < (this->global_max_x())) &&
           (glob_y >= this->global_y()) && (glob_y < (this->global_max_y()));
}

void Widget::enable_border() {
    this->border_.enable();
    std::size_t offset = this->border_.thickness();
    this->geometry().set_active_region(offset, offset, offset, offset);
    this->update();
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
    System::post_event(this, std::make_unique<Paint_event>());
}

bool Widget::event(const Event& event) {
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
        // this kind of thing below should be in paint_event handler
        this->erase_widget_screen();
        if (this->visible() && this->is_enabled()) {
            return this->paint_event(static_cast<const Paint_event&>(event));
        } else if (this->visible() && !this->is_enabled()) {
            this->paint_disabled_widget();
        } else if (!this->visible()) {
        }
        return true;
    }

    // Mouse_events
    if (event.type() == Event::MouseButtonPress) {
        // should most of this be in event handler function?
        if (!this->is_enabled() || !this->visible()) {
            return true;
        }
        if (this->focus_policy() == Focus_policy::ClickFocus ||
            this->focus_policy() == Focus_policy::StrongFocus) {
            System::set_focus_widget(this);
        }
        return this->mouse_press_event(static_cast<const Mouse_event&>(event));
    }
    if (event.type() == Event::MouseButtonRelease) {
        if (!this->is_enabled() || !this->visible()) {
            return true;
        }
        return this->mouse_release_event(
            static_cast<const Mouse_event&>(event));
    }
    if (event.type() == Event::MouseButtonDblClick) {
        if (!this->is_enabled() ||
            !this->visible()) {  // things like this should be in the handler
            return true;
        }
        return this->mouse_double_click_event(
            static_cast<const Mouse_event&>(event));
    }
    if (event.type() == Event::Wheel) {
        if (!this->is_enabled() || !this->visible()) {
            return true;
        }
        return this->wheel_event(static_cast<const Mouse_event&>(event));
    }
    if (event.type() == Event::MouseMove && this->has_mouse_tracking()) {
        if (!this->is_enabled() || !this->visible()) {
            return true;
        }
        return this->mouse_move_event(static_cast<const Mouse_event&>(event));
    }

    // KeyEvent
    if (event.type() == Event::KeyPress) {
        if (!this->is_enabled() || !this->visible()) {
            return true;  // is this the right response? or false?
        }
        return this->key_press_event(static_cast<const Key_event&>(event));
    }
    if (event.type() == Event::KeyRelease) {
        if (!this->is_enabled() || !this->visible()) {
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

bool Widget::move_event(const Move_event& event) {
    this->set_x(event.new_x());
    this->set_y(event.new_y());
    // Widget* parent = dynamic_cast<Widget*>(this->parent()); // causes infinte
    // loop
    // if (parent) { parent->update(); }
    this->update();
    return true;
}

bool Widget::resize_event(const Resize_event& event) {
    this->geometry().set_width(event.new_width());
    this->geometry().set_height(event.new_height());
    return true;
}

bool Widget::paint_event(const Paint_event& event) {
    // Post paint event to each child
    if (border_.is_enabled()) {
        Painter p{this};
        p.border(border_);
    }
    for (Object* c : this->children()) {
        Widget* child = dynamic_cast<Widget*>(c);
        if (child != nullptr) {
            child->update();
        }
    }
    return true;
}
// probably not needed anymore??
void Widget::erase_widget_screen() {
    Painter p{this};
    if (this->brush().background_color()) {
        p.fill(0, 0, this->geometry().width(), this->geometry().height(),
               *this->brush().background_color());
    }
}

void Widget::paint_disabled_widget() {
    // Re-implement this to change the brush to grey and repaint, this might be
    // done
    // elsewhere when the widget is disabled, the brush is changed to greyscale,
    // then
    // a typical update is done. The function will probably not be needed.
    Painter p{this};
    Widget* parent = dynamic_cast<Widget*>(this->parent());
    Color background = Color::Black;
    if (parent != nullptr && parent->brush().background_color()) {
        background = *parent->brush().background_color();
    }
    p.fill(0, 0, this->geometry().width(), this->geometry().height(),
           background);
}

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

bool Widget::hide_event(const Hide_event& event) {
    return true;
}

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
        p.move(cursor_x_, cursor_y_);
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

std::size_t Widget::find_global_x() const  // previously get_global_x
{
    Widget* parent_widg = dynamic_cast<Widget*>(this->parent());
    if (parent_widg == nullptr) {
        return this->x();
    }
    return this->x() + parent_widg->find_global_x();
}

std::size_t Widget::find_global_y() const {
    Widget* parent_widg = dynamic_cast<Widget*>(this->parent());
    if (parent_widg == nullptr) {
        return this->y();
    }
    return this->y() + parent_widg->find_global_y();
}

Paint_engine& Widget::paint_engine() const {
    if (paint_engine_) {
        return *paint_engine_;
    }
    return *System::paint_engine();
}

}  // namespace mcurses
