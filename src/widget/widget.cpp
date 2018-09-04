#include <cppurses/widget/widget.hpp>

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/add_default_attributes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/delete_event.hpp>
#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/events/enable_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/area.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/cursor_data.hpp>
#include <cppurses/widget/detail/border_offset.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {
namespace detail {
class Screen_state;
}

Widget::Widget(std::string name) : name_{std::move(name)} {}

Widget::~Widget() {
    if (Focus::focus_widget() == this) {
        Focus::clear_focus();
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

void Widget::enable(bool enable, bool post_child_polished_event) {
    this->enable_and_post_events(enable, post_child_polished_event);
    for (std::unique_ptr<Widget>& w : this->children.children_) {
        w->enable(enable, post_child_polished_event);
    }
}

void Widget::disable(bool disable, bool post_child_polished_event) {
    this->enable(!disable, post_child_polished_event);
}

void Widget::close() {
    std::unique_ptr<Widget> removed;
    if (this->parent() == nullptr) {
        // Can't delete a widget that is not owned by the system.
        return;
    } else {
        removed = this->parent()->children.remove(this);
    }
    System::post_event<Delete_event>(this, std::move(removed));
}

std::size_t Widget::x() const {
    return top_left_position_.x;
}

std::size_t Widget::y() const {
    return top_left_position_.y;
}

std::size_t Widget::inner_x() const {
    return top_left_position_.x + detail::Border_offset::west(*this);
}

std::size_t Widget::inner_y() const {
    return top_left_position_.y + detail::Border_offset::north(*this);
}

std::size_t Widget::width() const {
    return this->outer_width() - detail::Border_offset::east(*this) -
           detail::Border_offset::west(*this);
}

std::size_t Widget::height() const {
    return this->outer_height() - detail::Border_offset::north(*this) -
           detail::Border_offset::south(*this);
}

std::size_t Widget::outer_width() const {
    return outer_width_;
}

std::size_t Widget::outer_height() const {
    return outer_height_;
}

bool Widget::brush_paints_wallpaper() const {
    return brush_paints_wallpaper_;
}

void Widget::set_brush_paints_wallpaper(bool paints) {
    brush_paints_wallpaper_ = paints;
    this->update();
}

// TODO should have a mutex on it so events can't change background tile while
// flushing the screen. or wallpaper(?) should have a mutex, or anything
// that modifies the Glyph.
Glyph Widget::generate_wallpaper() const {
    Glyph background{this->wallpaper ? *(this->wallpaper)
                                     : System::terminal.background_tile()};
    if (this->brush_paints_wallpaper()) {
        detail::add_default_attributes(background, this->brush);
    }
    return background;
}

void Widget::update() {
    System::post_event<Paint_event>(this);
}

void Widget::enable_animation(Animation_engine::Period_t period) {
    System::animation_engine().register_widget(*this, period);
}

void Widget::enable_animation(
    const std::function<Animation_engine::Period_t()>& period_func) {
    System::animation_engine().register_widget(*this, period_func);
}

void Widget::disable_animation() {
    System::animation_engine().unregister_widget(*this);
}

bool Widget::focus_in_event() {
    focused_in();
    return true;
}

bool Widget::focus_out_event() {
    focused_out();
    return true;
}

bool Widget::paint_event() {
    Painter p{this};
    p.border(border);  // this checks if border and widget are enabled, etc..
    return true;
}

bool Widget::child_added_event(Widget* child) {
    child_added(child);
    this->update();
    return true;
}

bool Widget::child_removed_event(Widget* child) {
    child_removed(child);
    this->update();
    return true;
}

bool Widget::child_polished_event(Widget* child) {
    this->update();
    return true;
}

bool Widget::move_event(Point new_position, Point old_position) {
    moved(new_position);
    this->update();
    // this->screen_state().tiles.clear();
    return true;
}

bool Widget::resize_event(Area new_size, Area old_size) {
    resized(outer_width_, outer_height_);
    this->update();
    return true;
}

bool Widget::timer_event() {
    this->update();
    return true;
}

bool Widget::disable_event() {
    return Event_handler::disable_event();
}

void Widget::enable_and_post_events(bool enable,
                                    bool post_child_polished_event) {
    if (enabled_ != enable) {
        if (!enable) {
            System::post_event<Disable_event>(this);
        }
        enabled_ = enable;
        if (enable) {
            System::post_event<Enable_event>(this);
        }
        if (post_child_polished_event && this->parent() != nullptr) {
            System::post_event<Child_polished_event>(this->parent(), this);
        }
        this->update();
    }
}

void Widget::set_x(std::size_t global_x) {
    top_left_position_.x = global_x;
}

void Widget::set_y(std::size_t global_y) {
    top_left_position_.y = global_y;
}

// - - - - - - - - - - - - - - Free Functions - - - - - - - - - - - - - - - - -

bool has_border(const Widget& w) {
    return w.border.enabled;
}

void enable_border(Widget& w) {
    w.border.enabled = true;
    // System::post_event<Child_polished_event>(w.parent(), &w);
}

void disable_border(Widget& w) {
    w.border.enabled = false;
    // System::post_event<Child_polished_event>(w.parent(), &w);
}

bool has_coordinates(Widget& w, std::size_t global_x, std::size_t global_y) {
    if (!w.enabled()) {
        return false;
    }
    bool within_west = global_x >= w.inner_x();
    bool within_east = global_x < (w.inner_x() + w.width());
    bool within_north = global_y >= w.inner_y();
    bool within_south = global_y < (w.inner_y() + w.height());
    return within_west && within_east && within_north && within_south;
}

void set_background(Widget& w, Color c) {
    w.brush.set_background(c);
    w.update();
    w.background_color_changed(c);
}

void set_foreground(Widget& w, Color c) {
    w.brush.set_foreground(c);
    w.update();
    w.foreground_color_changed(c);
}

void clear_attributes(Widget& w) {
    w.brush.clear_attributes();
    w.update();
}

void set_background_recursive(Widget& w, Color c, bool single_level) {
    set_background(w, c);
    for (const std::unique_ptr<Widget>& child : w.children.get()) {
        if (single_level) {
            set_background(*child, c);
        } else {
            set_background_recursive(*child, c, single_level);
        }
    }
}

void set_foreground_recursive(Widget& w, Color c, bool single_level) {
    set_foreground(w, c);
    for (const std::unique_ptr<Widget>& child : w.children.get()) {
        if (single_level) {
            set_foreground(*child, c);
        } else {
            set_foreground_recursive(*child, c, single_level);
        }
    }
}

bool has_focus(const Widget& w) {
    return Focus::focus_widget() == &w;
}

detail::Screen_state& Widget::screen_state() {
    return screen_state_;
}

const detail::Screen_state& Widget::screen_state() const {
    return screen_state_;
}

}  // namespace cppurses
