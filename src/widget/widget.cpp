#include <cppurses/widget/widget.hpp>

#include <cstddef>
#include <iterator>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/add_default_attributes.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/deferred_delete_event.hpp>
#include <cppurses/system/events/disable_event.hpp>
#include <cppurses/system/events/enable_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/detail/border_offset.hpp>
#include <cppurses/widget/point.hpp>

namespace cppurses {

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
    if (parent != nullptr) {
        System::post_event<Child_added_event>(parent, this);
    } else if (this->parent() != nullptr) {
        System::send_event(Child_removed_event{this->parent(), this});
    }
    parent_ = parent;
}

Widget* Widget::parent() const {
    return parent_;
}

void Widget::enable(bool enable) {
    if (enable) {
        System::post_event<Enable_event>(this);
        for (Widget* w : this->children.get_descendants()) {
            System::post_event<Enable_event>(w);
        }
    } else {
        // System::send_event(Disable_event{this}); // TODO think on this?
        // why do you have to send this in children_data::remove?
        System::post_event<Disable_event>(this);
        for (Widget* w : this->children.get_descendants()) {
            System::post_event<Disable_event>(w);
        }
    }
}

void Widget::disable(bool disable) {
    this->enable(!disable);
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

bool Widget::visible() const {
    return visible_;
}

void Widget::set_background_tile(const Glyph& tile) {  // TODO Prob not needed.
    this->set_background_tile(opt::Optional<Glyph>{tile});
}

void Widget::set_background_tile(opt::Optional<Glyph> tile) {
    background_tile_ = tile;
    this->update();
}

const opt::Optional<Glyph>& Widget::background_tile() const {
    return background_tile_;
}

bool Widget::brush_alters_background() const {
    return brush_alters_background_;
}

void Widget::set_brush_alters_background(bool alters) {
    brush_alters_background_ = alters;
    this->update();
}

// TODO should have a mutex on it so events can't change background tile while
// flushing the screen. or set_background_tile should have a mutex, or anything
// that modifies the Glyph.
Glyph Widget::find_background_tile() const {
    Glyph background{this->background_tile()
                         ? *(this->background_tile())
                         : System::paint_buffer().get_global_background_tile()};
    if (this->brush_alters_background()) {
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

bool Widget::close_event() {
    System::post_event<Deferred_delete_event>(this);
    return true;
}

bool Widget::focus_in_event() {
    // TODO obsolete? flush takes care of this cursor movement
    System::paint_buffer().move_cursor(this->inner_x() + this->cursor.x(),
                                       this->inner_y() + this->cursor.y());
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
    return true;
}

bool Widget::deferred_delete_event(Event_handler* to_delete) {
    this->children.remove(static_cast<Widget*>(to_delete));
    return true;
}

bool Widget::child_added_event(Widget* child) {
    child_added(child);
    this->screen_state().child_event_happened = true;
    this->update();
    child->enable(this->enabled());
    return true;
}

bool Widget::child_removed_event(Widget* child) {
    child_removed(child);
    this->screen_state().child_event_happened = true;
    this->update();
    // System::post_event<Disable_event>(child);
    System::send_event(Disable_event{child});
    return true;
}

bool Widget::child_polished_event(Widget* child) {
    this->update();
    this->screen_state().child_event_happened = true;
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

bool Widget::move_event(Point new_position, Point old_position) {
    old_position = top_left_position_;
    this->set_x(new_position.x);
    this->set_y(new_position.y);
    if (old_position != top_left_position_) {
        moved(new_position);
        this->update();
        this->screen_state().move_happened = true;
    }
    return true;
}

bool Widget::resize_event(Area new_size, Area old_size) {
    old_size.width = outer_width_;
    old_size.height = outer_height_;

    this->outer_width_ = new_size.width;
    this->outer_height_ = new_size.height;

    if (old_size.width != outer_width_ || old_size.height != outer_height_) {
        resized(outer_width_, outer_height_);  // TODO what w/h do you report?
        this->update();
        this->screen_state().resize_happened = true;
    }
    return true;
}

bool Widget::animation_event() {
    this->update();
    return true;
}

void Widget::set_visible(bool visible, bool recursive) {
    visible_ = visible;
    if (!recursive) {
        return;
    }
    // TODO remove the recursion from here, and instead have it from call site.
    for (const std::unique_ptr<Widget>& c : this->children.get()) {
        c->set_visible(visible, recursive);
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
