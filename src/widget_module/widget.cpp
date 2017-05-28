#include "widget_module/widget.hpp"

#include "painter_module/brush.hpp"
#include "painter_module/color.hpp"
#include "painter_module/geometry.hpp"
#include "painter_module/painter.hpp"
#include "system_module/events/child_event.hpp"
#include "system_module/events/clear_screen_event.hpp"
#include "system_module/events/close_event.hpp"
#include "system_module/events/enable_event.hpp"
#include "system_module/events/focus_event.hpp"
#include "system_module/events/hide_event.hpp"
#include "system_module/events/key_event.hpp"
#include "system_module/events/mouse_event.hpp"
#include "system_module/events/move_event.hpp"
#include "system_module/events/paint_event.hpp"
#include "system_module/events/resize_event.hpp"
#include "system_module/events/show_event.hpp"
#include "system_module/key.hpp"
#include "system_module/object.hpp"
#include "system_module/system.hpp"
#include "widget_module/border.hpp"
#include "widget_module/coordinates.hpp"
#include <cstddef>
#include <memory>
#include <utility>

namespace cppurses {

Widget::Widget() {
    this->Widget::initialize();
    this->update();
}

Widget::~Widget() {
    if (System::focus_widget() == this) {
        System::set_focus_widget(nullptr, false);
    }
}

void Widget::initialize() {
    close = [this]() {
        System::post_event(this, std::make_unique<Close_event>());
    };
    close.track(this->destroyed);

    hide = [this]() { this->set_visible(false); };
    hide.track(this->destroyed);

    show = [this]() { this->set_visible(true); };
    show.track(this->destroyed);

    repaint = [this]() { System::send_event(this, Paint_event()); };
    repaint.track(this->destroyed);

    give_focus = [this]() { this->set_focus(true); };
    give_focus.track(this->destroyed);

    update_me = [this]() { this->update(); };
    update_me.track(this->destroyed);

    click_me = [this](Mouse_button b, Coordinates c) {
        System::send_event(
            this, Mouse_event(Event::MouseButtonPress, b, this->x() + c.x,
                              this->y() + c.y, c.x, c.y, 0));
    };
    click_me.track(this->destroyed);

    keypress_me = [this](Key key) {
        System::send_event(this,
                           Key_event(Event::KeyPress, static_cast<int>(key)));
    };
    keypress_me.track(this->destroyed);

    set_background = [this](Color c) { this->set_background_(c); };
    set_background.track(this->destroyed);
    set_foreground = [this](Color c) { this->set_foreground_(c); };
    set_foreground.track(this->destroyed);
}

void Widget::set_x(std::size_t global_x) {
    auto parent = dynamic_cast<Widget*>(this->parent());
    auto screen_width = System::max_width();
    if (global_x >= screen_width && screen_width > 0) {
        global_x = screen_width - 1;
    } else if (screen_width == 0) {
        global_x = 0;
    }
    if (parent != nullptr) {
        if (global_x >= parent->x()) {
            position_.x = global_x - parent->x();
        } else {
            position_.x = 0;
        }
    } else {
        position_.x = global_x;
    }
}

void Widget::set_y(std::size_t global_y) {
    auto parent = dynamic_cast<Widget*>(this->parent());
    auto screen_height = System::max_height();
    if (global_y >= screen_height && screen_height > 0) {
        global_y = screen_height - 1;
    } else if (screen_height == 0) {
        global_y = 0;
    }
    if (parent != nullptr) {
        if (global_y >= parent->y()) {
            position_.y = global_y - parent->y();
        } else {
            position_.y = 0;
        }
    } else {
        position_.y = global_y;
    }
}

void Widget::set_background_(Color c) {
    this->brush().set_background(c);
    this->update();
}

void Widget::set_foreground_(Color c) {
    this->brush().set_foreground(c);
    this->update();
}

bool Widget::has_coordinates(std::size_t global_x, std::size_t global_y) {
    if (!this->enabled() || !this->visible()) {
        return false;
    }
    auto border = this->border();
    bool within_west = global_x >= (this->x() + west_border_offset(border));
    bool within_east =
        global_x < (this->x() + this->width() + west_border_offset(border));
    bool within_north = global_y >= (this->y() + north_border_offset(border));
    bool within_south =
        global_y < (this->y() + this->height() + north_border_offset(border));
    return within_west && within_east && within_north && within_south;
}

void Widget::enable_border() {
    this->border_.enable();
    auto event = std::make_unique<Child_event>(Event::ChildPolished, this);
    System::post_event(this->parent(), std::move(event));
}

void Widget::disable_border() {
    this->border_.disable();
    auto event = std::make_unique<Child_event>(Event::ChildPolished, this);
    System::post_event(this->parent(), std::move(event));
}

void Widget::set_geometry(const Geometry& g) {
    geometry_ = g;
    geometry_.set_widget(this);
    this->update();
}

void Widget::update() {
    this->clear_screen();
    System::post_event(this, std::make_unique<Paint_event>());
}

void Widget::set_brush(Brush brush) {
    default_brush_ = std::move(brush);
    this->update();
}

void Widget::clear_screen() {
    System::post_event(this, std::make_unique<Clear_screen_event>());
}

void Widget::move_cursor(Coordinates c) {
    this->move_cursor(c.x, c.y);
}

void Widget::move_cursor(std::size_t x, std::size_t y) {
    this->move_cursor_x(x);
    this->move_cursor_y(y);
}

void Widget::move_cursor_x(std::size_t x) {
    if (x < this->width()) {
        cursor_position_.x = x;
    } else if (this->width() != 0) {
        cursor_position_.x = this->width() - 1;
    }
}

void Widget::move_cursor_y(std::size_t y) {
    if (y < this->height()) {
        cursor_position_.y = y;
    } else if (this->height() != 0) {
        cursor_position_.y = this->height() - 1;
    }
}

bool Widget::event(const Event& event) {
    auto visible_and_enabled = [this]() {
        return this->visible() && this->enabled();
    };

    if (event.type() == Event::Move) {
        return this->move_event(static_cast<const Move_event&>(event));
    }

    if (event.type() == Event::Resize) {
        return this->resize_event(static_cast<const Resize_event&>(event));
    }

    if (event.type() == Event::Paint) {
        if (visible_and_enabled()) {
            return this->paint_event(static_cast<const Paint_event&>(event));
        }
        if (this->visible() && !this->enabled()) {
            // Layout should handle disable widgets, do not allow invisible
        }
        return true;
    }

    if (event.type() == Event::ClearScreen) {
        return this->clear_screen_event(
            static_cast<const Clear_screen_event&>(event));
    }

    if (event.type() == Event::MouseButtonPress) {
        // Handle focus elsewhere.
        if (visible_and_enabled()) {
            if (this->focus_policy() == Focus_policy::Click ||
                this->focus_policy() == Focus_policy::Strong) {
                System::set_focus_widget(this);
                this->paint_engine().move(this->x() + this->cursor_x(),
                                          this->y() + this->cursor_y());
            }
            return this->mouse_press_event(
                static_cast<const Mouse_event&>(event));
        }
        return true;
    }
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

    if (event.type() == Event::Close) {
        return this->close_event(static_cast<const Close_event&>(event));
    }

    if (event.type() == Event::Hide) {
        return this->hide_event(static_cast<const Hide_event&>(event));
    }

    if (event.type() == Event::Show) {
        return this->show_event(static_cast<const Show_event&>(event));
    }

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
    return true;
}

bool Widget::clear_screen_event(const Clear_screen_event& event) {
    Painter p{this};
    p.clear_screen();
    return true;
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
    if (event.key_code() == Key::Tab) {
        System::cycle_tab_focus();
    }
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
    return Object::enable_event(event);
}

bool Widget::focus_event(const Focus_event& event) {
    return true;
}

void Widget::set_focus(bool focus) {
    if (this->focus_policy() == Focus_policy::None) {
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
        auto* child = dynamic_cast<Widget*>(c);
        if (child != nullptr) {
            child->set_visible(visible);
        }
    }
}

std::size_t Widget::x() const {
    if (this->parent() == nullptr) {
        return this->position_.x;
    }
    auto* parent_widg = static_cast<Widget*>(this->parent());
    return this->position_.x + parent_widg->x();
}

std::size_t Widget::y() const {
    if (this->parent() == nullptr) {
        return this->position_.x;
    }
    auto* parent_widg = static_cast<Widget*>(this->parent());
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

}  // namespace cppurses
