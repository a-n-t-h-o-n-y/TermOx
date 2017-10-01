#include "widget/widget.hpp"
#include "painter/brush.hpp"
#include "painter/color.hpp"
#include "painter/painter.hpp"
#include "painter/detail/paint_buffer.hpp"
#include "system/events/child_event.hpp"
#include "system/events/clear_screen_event.hpp"
#include "system/events/paint_event.hpp"
#include "system/events/deferred_delete_event.hpp"
#include "system/system.hpp"
#include "system/focus.hpp"
#include "widget/border.hpp"
#include "widget/coordinates.hpp"
#include <signals/signal.hpp>
#include <cstddef>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {
using namespace cppurses;

void clear_screen(Widget& w) {
    System::post_event<Clear_screen_event>(&w);
}

}  // namespace

namespace cppurses {

Widget::Widget(std::string name) : name_{std::move(name)} {
    this->update();
}

Widget::~Widget() {
    if (Focus::focus_widget() == this) {
        Focus::clear_focus();
    }
    Widget* parent = this->parent();
    if (parent != nullptr) {
        System::send_event(Child_removed_event{parent, this});
    }
}

void Widget::set_name(std::string name) {
    name_ = std::move(name);
    name_changed(name_);
}

std::string Widget::name() const {
    return name_;
}

void Widget::set_parent(Widget* parent) {
    parent_ = parent;
}

Widget* Widget::parent() const {
    return parent_;
}

void Widget::add_child(std::unique_ptr<Widget> child) {
    children_.emplace_back(std::move(child));
    children_.back()->set_parent(this);
    System::post_event<Child_added_event>(this, children_.back().get());
}

bool Widget::contains_child(Widget* child) {
    for (const auto& w_ptr : children_) {
        if (w_ptr.get() == child || w_ptr->contains_child(child)) {
            return true;
        }
    }
    return false;
}

std::vector<Widget*> Widget::children() const {
    std::vector<Widget*> ret;
    std::transform(std::begin(children_), std::end(children_),
                   std::back_inserter(ret), [](auto& up) { return up.get(); });
    return ret;
}

std::size_t Widget::x() const {
    Widget* parent = this->parent();
    if (parent == nullptr) {
        return this->position_.x;
    }
    return this->position_.x + parent->x();
}

std::size_t Widget::y() const {
    Widget* parent = this->parent();
    if (parent == nullptr) {
        return this->position_.y;
    }
    return this->position_.y + parent->y();
}

std::size_t Widget::width() const {
    std::size_t width_border_offset =
        west_border_offset(this->border) + east_border_offset(this->border);
    if (width_border_offset > width_) {
        return 0;
    }
    return width_ - width_border_offset;
}

std::size_t Widget::height() const {
    std::size_t height_border_offset =
        north_border_offset(this->border) + south_border_offset(this->border);
    if (height_border_offset > height_) {
        return 0;
    }
    return height_ - height_border_offset;
}

bool Widget::cursor_visible() const {
    return show_cursor_;
}

void Widget::show_cursor(bool show) {
    show_cursor_ = show;
}

void Widget::hide_cursor(bool hide) {
    show_cursor_ = !hide;
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

std::size_t Widget::cursor_x() const {
    return cursor_position_.x;
}

std::size_t Widget::cursor_y() const {
    return cursor_position_.y;
}

Coordinates Widget::cursor_coordinates() const {
    return cursor_position_;
}

bool Widget::visible() const {
    return visible_;
}

void Widget::update() {
    clear_screen(*this);
    System::post_event<Paint_event>(this);
}

bool Widget::close_event() {
    System::post_event<Deferred_delete_event>(this);
    return true;
}

bool Widget::focus_in_event() {
    System::paint_buffer()->move(this->x() + this->cursor_x(),
                                 this->y() + this->cursor_y());
    focused_in();
    return true;
}

bool Widget::focus_out_event() {
    focused_out();
    return true;
}

bool Widget::paint_event() {
    if (border.enabled) {
        Painter p{this};
        p.border(border);
    }
    // Might not need below if focus widget sets this afterwards, on no focus?
    System::paint_buffer()->move(this->x() + this->cursor_x(),
                                 this->y() + this->cursor_y());
    return true;
}

bool Widget::clear_screen_event() {
    Painter p{this};
    p.clear_screen();
    return true;
}

bool Widget::deferred_delete_event(Event_handler* to_delete) {
    this->delete_child(static_cast<Widget*>(to_delete));
    return true;
}

bool Widget::child_added_event(Event_handler* child) {
    this->update();
    return true;
}

bool Widget::child_removed_event(Event_handler* child) {
    this->update();
    return true;
}

bool Widget::child_polished_event(Event_handler* child) {
    this->update();
    return true;
}

bool Widget::show_event() {
    this->set_visible(true);
    this->update();
    return true;
}

bool Widget::hide_event() {
    this->set_visible(false);
    this->update();
    return true;
}

bool Widget::move_event(std::size_t new_x,
                        std::size_t new_y,
                        std::size_t old_x,
                        std::size_t old_y) {
    this->set_x(new_x);
    this->set_y(new_y);
    this->update();
    return true;
}

bool Widget::resize_event(std::size_t new_width,
                          std::size_t new_height,
                          std::size_t old_width,
                          std::size_t old_height) {
    width_ = new_width;
    height_ = new_height;
    this->update();
    return true;
}

void Widget::set_visible(bool visible) {
    this->visible_ = visible;
    for (Widget* c : this->children()) {
        c->set_visible(visible);
    }
}

void Widget::delete_child(Widget* child) {
    auto end_iter = std::end(children_);
    auto at = std::find_if(std::begin(children_), end_iter,
                           [child](auto& c) { return c.get() == child; });
    if (at != end_iter) {
        children_.erase(at);
    }
}

void Widget::set_x(std::size_t global_x) {
    auto parent = this->parent();
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
    auto parent = this->parent();
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

// - - - - - - - - - - - - - - Free Functions - - - - - - - - - - - - - - - - -

bool has_border(const Widget& w) {
    return w.border.enabled;
}

void enable_border(Widget& w) {
    w.border.enabled = true;
    System::post_event<Child_polished_event>(w.parent(), &w);
}

void disable_border(Widget& w) {
    w.border.enabled = false;
    System::post_event<Child_polished_event>(w.parent(), &w);
}

bool has_coordinates(Widget& w, std::size_t global_x, std::size_t global_y) {
    if (!w.enabled() || !w.visible()) {
        return false;
    }
    bool within_west = global_x >= (w.x() + west_border_offset(w.border));
    bool within_east =
        global_x < (w.x() + w.width() + west_border_offset(w.border));
    bool within_north = global_y >= (w.y() + north_border_offset(w.border));
    bool within_south =
        global_y < (w.y() + w.height() + north_border_offset(w.border));
    return within_west && within_east && within_north && within_south;
}

void move_cursor(Widget& w, Coordinates c) {
    move_cursor(w, c.x, c.y);
}

void move_cursor(Widget& w, std::size_t x, std::size_t y) {
    w.move_cursor_x(x);
    w.move_cursor_y(y);
}

void set_background(Widget& w, Color c) {
    w.brush.set_background(c);
    w.update();
}

void set_foreground(Widget& w, Color c) {
    w.brush.set_foreground(c);
    w.update();
}

bool has_focus(const Widget& w) {
    return Focus::focus_widget() == &w;
}

}  // namespace cppurses
