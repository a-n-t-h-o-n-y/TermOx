#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/clear_screen_event.hpp>
#include <cppurses/system/events/deferred_delete_event.hpp>
#include <cppurses/system/events/on_tree_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/coordinates.hpp>
#include <cppurses/widget/widget.hpp>

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
    System::post_event<On_tree_event>(children_.back().get(), this->on_tree());
}

std::vector<Widget*> Widget::children() const {
    std::vector<Widget*> ret;
    std::transform(std::begin(children_), std::end(children_),
                   std::back_inserter(ret), [](auto& up) { return up.get(); });
    return ret;
}

bool Widget::contains_child(Widget* child) {
    for (const auto& w_ptr : children_) {
        if (w_ptr.get() == child || w_ptr->contains_child(child)) {
            return true;
        }
    }
    return false;
}

std::unique_ptr<Widget> Widget::remove_child(Widget* child) {
    auto end_iter = std::end(children_);
    auto at = std::find_if(std::begin(children_), end_iter,
                           [child](auto& c) { return c.get() == child; });
    if (at == end_iter) {
        return nullptr;
    }
    std::unique_ptr<Widget> removed = std::move(*at);
    children_.erase(at);
    removed->set_parent(nullptr);
    System::send_event(Child_removed_event{this, child});
    System::send_event(On_tree_event{removed.get(), false});
    return removed;
}

std::unique_ptr<Widget> Widget::remove_child(const std::string& name) {
    return this->remove_child(this->find_child<Widget>(name));
}

std::size_t Widget::x() const {
    Widget* parent = this->parent();
    if (parent == nullptr) {
        return this->position_.x + west_border_offset(*this);
    }
    return this->position_.x + parent->x() + west_border_offset(*this);
}

std::size_t Widget::y() const {
    Widget* parent = this->parent();
    if (parent == nullptr) {
        return this->position_.y + north_border_offset(*this);
    }
    return this->position_.y + parent->y() + north_border_offset(*this);
}

std::size_t Widget::width() const {
    return width_;
}

std::size_t Widget::height() const {
    return height_;
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

bool Widget::on_tree() const {
    return on_tree_;
}

void Widget::update() {
    clear_screen(*this);
    System::post_event<Paint_event>(this);
}

bool Widget::east_border_disqualified() const {
    return east_border_disqualified_;
}

bool Widget::west_border_disqualified() const {
    return west_border_disqualified_;
}

bool Widget::north_border_disqualified() const {
    return north_border_disqualified_;
}

bool Widget::south_border_disqualified() const {
    return south_border_disqualified_;
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
    this->remove_child(static_cast<Widget*>(to_delete));
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

bool Widget::show_event() {
    this->set_visible(true);
    this->update();
    return true;
}

bool Widget::on_tree_event(bool on_tree) {
    on_tree_ = on_tree;
    for (auto& child : children_) {
        System::post_event<On_tree_event>(child.get(), on_tree_);
    }
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
    moved(Coordinates{new_x, new_y});
    moved_xy(new_x, new_y);
    this->update();
    return true;
}

bool Widget::resize_event(std::size_t new_width,
                          std::size_t new_height,
                          std::size_t old_width,
                          std::size_t old_height) {
    east_border_disqualified_ = false;
    west_border_disqualified_ = false;
    north_border_disqualified_ = false;
    south_border_disqualified_ = false;

    // If new_width is too small for widget and borders, disqualify borders
    if (new_width == 2) {
        if (west_border_offset(*this) == 0) {
            west_border_disqualified_ = true;
        } else {
            east_border_disqualified_ = true;
        }
    }
    if (new_width <= 1) {
        east_border_disqualified_ = true;
        west_border_disqualified_ = true;
    }

    if (new_height == 2) {
        if (north_border_offset(*this) == 0) {
            north_border_disqualified_ = true;
        } else {
            south_border_disqualified_ = true;
        }
    }
    if (new_height <= 1) {
        north_border_disqualified_ = true;
        south_border_disqualified_ = true;
    }

    width_ = new_width - east_border_offset(*this) - west_border_offset(*this);
    height_ =
        new_height - north_border_offset(*this) - south_border_offset(*this);

    resized(width_, height_);
    this->update();
    return true;
}

void Widget::set_visible(bool visible, bool recursive) {
    visible_ = visible;
    if (!recursive) {
        return;
    }
    for (Widget* c : this->children()) {
        c->set_visible(visible, recursive);
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

std::size_t west_border_offset(const Widget& w) {
    const Border& b{w.border};
    if (b.enabled && !w.west_border_disqualified() &&
        (b.west_enabled || b.north_west_enabled || b.south_west_enabled)) {
        return 1;
    }
    return 0;
}

std::size_t east_border_offset(const Widget& w) {
    const Border& b{w.border};
    if (b.enabled && !w.east_border_disqualified() &&
        (b.east_enabled || b.north_east_enabled || b.south_east_enabled)) {
        return 1;
    }
    return 0;
}

std::size_t north_border_offset(const Widget& w) {
    const Border& b{w.border};
    if (b.enabled && !w.north_border_disqualified() &&
        (b.north_enabled || b.north_east_enabled || b.north_west_enabled)) {
        return 1;
    }
    return 0;
}

std::size_t south_border_offset(const Widget& w) {
    const Border& b{w.border};
    if (b.enabled && !w.south_border_disqualified() &&
        (b.south_enabled || b.south_east_enabled || b.south_west_enabled)) {
        return 1;
    }
    return 0;
}

bool has_coordinates(Widget& w, std::size_t global_x, std::size_t global_y) {
    if (!w.enabled() || !w.visible()) {
        return false;
    }
    bool within_west = global_x >= w.x();
    bool within_east = global_x < (w.x() + w.width());
    bool within_north = global_y >= w.y();
    bool within_south = global_y < (w.y() + w.height());
    return within_west && within_east && within_north && within_south;
}

void move_cursor(Widget& w, Coordinates c) {
    move_cursor(w, c.x, c.y);
}

void move_cursor(Widget& w, std::size_t x, std::size_t y) {
    w.move_cursor_x(x);
    w.move_cursor_y(y);
    w.cursor_moved(Coordinates{x, y});
    w.cursor_moved_xy(x, y);
}

void set_background(Widget& w, Color c) {
    w.brush.set_background(c);
    w.background_color_changed(c);
    w.update();
}

void set_foreground(Widget& w, Color c) {
    w.brush.set_foreground(c);
    w.foreground_color_changed(c);
    w.update();
}

void set_background_recursive(Widget& w, Color c, bool single_level) {
    set_background(w, c);
    for (Widget* child : w.children()) {
        if (single_level) {
            set_background(*child, c);
        } else {
            set_background_recursive(*child, c, single_level);
        }
    }
}

void set_foreground_recursive(Widget& w, Color c, bool single_level) {
    set_foreground(w, c);
    for (Widget* child : w.children()) {
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

void toggle_cursor(Widget& w) {
    w.show_cursor(!w.cursor_visible());
}

}  // namespace cppurses
