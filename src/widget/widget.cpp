#include <cppurses/widget/widget.hpp>

#include <cstddef>
#include <iterator>
#include <memory>
#include <string>
#include <utility>

#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/add_default_attributes.hpp>
// #include <cppurses/painter/detail/flush.hpp>
#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/paint_buffer.hpp>
#include <cppurses/painter/painter.hpp>
#include <cppurses/system/events/child_event.hpp>
#include <cppurses/system/events/deferred_delete_event.hpp>
#include <cppurses/system/events/on_tree_event.hpp>
#include <cppurses/system/events/paint_event.hpp>
#include <cppurses/system/events/repaint_event.hpp>
#include <cppurses/system/focus.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/point.hpp>

#include <vector>

namespace cppurses {

Widget::Widget(std::string name) : name_{std::move(name)} {
    this->update();
}

Widget::~Widget() {
    if (Focus::focus_widget() == this) {
        Focus::clear_focus();
    }
    // Prevents the animation engine from sending events to destroyed widgets.
    // But that should be taken care of by the slots tracking the lifetime of
    // the objects.
    // if (System::head() == this) {
    //     System::animation_engine().shutdown();
    // }
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

void Widget::insert_child(std::unique_ptr<Widget> child, std::size_t index) {
    children_.insert(std::begin(children_) + index, std::move(child));
    children_[index]->set_parent(this);
    System::post_event<Child_added_event>(this, children_[index].get());
    System::post_event<On_tree_event>(children_[index].get(), this->on_tree());
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
    // TODO look into send_event()s below, is post_event() possible?
    System::send_event(Child_removed_event{this, child});
    System::send_event(On_tree_event{removed.get(), false});
    return removed;
}

std::unique_ptr<Widget> Widget::remove_child(const std::string& name) {
    return this->remove_child(this->find_child<Widget>(name));
}

std::size_t Widget::x() const {
    return top_left_position_.x;
}

std::size_t Widget::y() const {
    return top_left_position_.y;
}

std::size_t Widget::inner_x() const {
    return top_left_position_.x + west_border_offset(*this);
}

std::size_t Widget::inner_y() const {
    return top_left_position_.y + north_border_offset(*this);
}

// std::size_t Widget::x() const {
//     Widget* parent = this->parent();
//     if (parent == nullptr) {
//         return this->position_.x + west_border_offset(*this);
//     }
//     return this->position_.x + parent->x() + west_border_offset(*this);
// }

// std::size_t Widget::y() const {
//     Widget* parent = this->parent();
//     if (parent == nullptr) {
//         return this->position_.y + north_border_offset(*this);
//     }
//     return this->position_.y + parent->y() + north_border_offset(*this);
// }

std::size_t Widget::width() const {
    return width_;
}

std::size_t Widget::height() const {
    return height_;
}

std::size_t Widget::outer_width() const {
    return this->width() + west_border_offset(*this) +
           east_border_offset(*this);
}

std::size_t Widget::outer_height() const {
    return this->height() + north_border_offset(*this) +
           south_border_offset(*this);
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

Point Widget::cursor_coordinates() const {
    return cursor_position_;
}

bool Widget::visible() const {
    return visible_;
}

bool Widget::on_tree() const {
    return on_tree_;
}

void Widget::set_background_tile(const Glyph& tile) {  // TODO Prob not needed.
    this->set_background_tile(opt::Optional<Glyph>{tile});
}

void Widget::set_background_tile(opt::Optional<Glyph> tile) {
    background_tile_ = tile;
    System::post_event<Repaint_event>(this);
}

const opt::Optional<Glyph>& Widget::background_tile() const {
    return background_tile_;
}

bool Widget::brush_alters_background() const {
    return brush_alters_background_;
}

void Widget::set_brush_alters_background(bool alters) {
    brush_alters_background_ = alters;
    System::post_event<Repaint_event>(this);
}

Glyph Widget::find_background_tile() const {
    Glyph background{this->background_tile()
                         ? *(this->background_tile())
                         : System::paint_buffer().get_global_background_tile()};
    if (this->brush_alters_background()) {
        detail::add_default_attributes(background, this->brush);
    }
    return background;
}

// Called by Paint_event::send()
// void Widget::repaint_background() {
//     Glyph background;
//     if (background_tile_) {
//         background = *background_tile_;
//     } else {
//         background = System::paint_buffer().get_global_background_tile();
//     }
//     Painter p{this};
//     if (brush_alters_background_) {
//         background = p.add_default_attributes(background);
//     }
//     p.fill(background, 0, 0, this->width(), this->height(), true);
// }

void Widget::update() {
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
    System::paint_buffer().move_cursor(this->inner_x() + this->cursor_x(),
                                       this->inner_y() + this->cursor_y());
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

bool Widget::repaint_event() {
    // detail::Staged_changes changes;
    // Painter p{this, changes};
    // Glyph bg{this->find_background_tile()};
    // p.fill(bg, 0, 0, this->width() + east_border_offset(*this),
    //        this->height() + south_border_offset(*this));
    // detail::flush(changes, true);
    // this->update();
    // return true;
    System::find_event_loop().staged_changes()[this].repaint = true;
    this->update();
    // System::send_event(Paint_event{this});
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
    this->set_visible(on_tree);
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

bool Widget::move_event(Point new_position, Point old_position) {
    this->set_x(new_position.x);
    this->set_y(new_position.y);
    moved(new_position);
    moved_xy(new_position.x, new_position.y);
    System::post_event<Repaint_event>(this);
    return true;
}

bool Widget::resize_event(Area new_size, Area old_size) {
    east_border_disqualified_ = false;
    west_border_disqualified_ = false;
    north_border_disqualified_ = false;
    south_border_disqualified_ = false;

    // If new_size.width is too small for widget and borders, disqualify borders
    if (new_size.width == 2) {
        if (west_border_offset(*this) == 0) {
            west_border_disqualified_ = true;
        } else {
            east_border_disqualified_ = true;
        }
    }
    if (new_size.width <= 1) {
        east_border_disqualified_ = true;
        west_border_disqualified_ = true;
    }

    if (new_size.height == 2) {
        if (north_border_offset(*this) == 0) {
            north_border_disqualified_ = true;
        } else {
            south_border_disqualified_ = true;
        }
    }
    if (new_size.height <= 1) {
        north_border_disqualified_ = true;
        south_border_disqualified_ = true;
    }

    width_ =
        new_size.width - east_border_offset(*this) - west_border_offset(*this);
    height_ = new_size.height - north_border_offset(*this) -
              south_border_offset(*this);

    resized(width_, height_);
    System::post_event<Repaint_event>(this);
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
    for (Widget* c : this->children()) {
        c->set_visible(visible, recursive);
    }
}

void Widget::set_x(std::size_t global_x) {
    top_left_position_.x = global_x;
}

void Widget::set_y(std::size_t global_y) {
    top_left_position_.y = global_y;
}

// void Widget::set_x(std::size_t global_x) {
//     auto parent = this->parent();
//     auto screen_width = System::max_width();
//     if (global_x >= screen_width && screen_width > 0) {
//         global_x = screen_width - 1;
//     } else if (screen_width == 0) {
//         global_x = 0;
//     }
//     if (parent != nullptr) {
//         if (global_x >= parent->x()) {
//             position_.x = global_x - parent->x();
//         } else {
//             position_.x = 0;
//         }
//     } else {
//         position_.x = global_x;
//     }
// }

// void Widget::set_y(std::size_t global_y) {
//     auto parent = this->parent();
//     auto screen_height = System::max_height();
//     if (global_y >= screen_height && screen_height > 0) {
//         global_y = screen_height - 1;
//     } else if (screen_height == 0) {
//         global_y = 0;
//     }
//     if (parent != nullptr) {
//         if (global_y >= parent->y()) {
//             position_.y = global_y - parent->y();
//         } else {
//             position_.y = 0;
//         }
//     } else {
//         position_.y = global_y;
//     }
// }

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
    bool within_west = global_x >= w.inner_x();
    bool within_east = global_x < (w.inner_x() + w.width());
    bool within_north = global_y >= w.inner_y();
    bool within_south = global_y < (w.inner_y() + w.height());
    return within_west && within_east && within_north && within_south;
}

void move_cursor(Widget& w, Point c) {
    move_cursor(w, c.x, c.y);
}

void move_cursor(Widget& w, std::size_t x, std::size_t y) {
    w.move_cursor_x(x);
    w.move_cursor_y(y);
    w.cursor_moved(Point{x, y});
    w.cursor_moved_xy(x, y);
}

void set_background(Widget& w, Color c) {
    w.brush.set_background(c);
    // w.repaint_background();
    System::post_event<Repaint_event>(&w);
    w.background_color_changed(c);
    // w.update();
}

void set_foreground(Widget& w, Color c) {
    w.brush.set_foreground(c);
    // w.repaint_background();
    System::post_event<Repaint_event>(&w);
    w.foreground_color_changed(c);
    // w.update();
}

void clear_attributes(Widget& w) {
    w.brush.clear_attributes();
    // w.repaint_background();
    System::post_event<Repaint_event>(&w);
    // attribute changed signal
    // w.update();
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

detail::Screen_state& Widget::screen_state() {
    return screen_state_;
}

const detail::Screen_state& Widget::screen_state() const {
    return screen_state_;
}

}  // namespace cppurses
