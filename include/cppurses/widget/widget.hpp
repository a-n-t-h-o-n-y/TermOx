#ifndef WIDGET_WIDGET_HPP
#define WIDGET_WIDGET_HPP
#include <algorithm>
#include <cstddef>
#include <functional>
#include <memory>
#include <queue>
#include <signals/signal.hpp>
#include <string>
#include <utility>
#include <vector>

#include <optional/optional.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/system/events/repaint_event.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/system.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace cppurses {
class Painter;

class Widget : public Event_handler {
   public:
    explicit Widget(std::string name = "");
    ~Widget() override;

    Widget(const Widget&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(Widget&&) = delete;

    // Name
    std::string name() const;
    void set_name(std::string name);

    // Parent
    Widget* parent() const;
    void set_parent(Widget* parent);

    // Children
    void add_child(std::unique_ptr<Widget> child);
    void insert_child(std::unique_ptr<Widget> child, std::size_t index);
    // std::vector<Widget*> children() const;
    const std::vector<std::unique_ptr<Widget>>& children() const;
    bool contains_child(Widget* child);

    template <typename T, typename... Args>
    T& make_child(Args&&... args);

    template <typename T>
    T* find_child(const std::string& name) const;

    std::unique_ptr<Widget> remove_child(Widget* child);
    std::unique_ptr<Widget> remove_child(const std::string& name);

    /// Top left point of the widget with respect to the top left point of the
    /// screen, not accounting for border space.
    std::size_t x() const;
    std::size_t y() const;

    /// Top left point of the widget with respect to the top left point of the
    // screen, accounting for border space.
    std::size_t inner_x() const;
    std::size_t inner_y() const;

    // Dimensions, not including border space.
    std::size_t width() const;
    std::size_t height() const;

    /// Dimensions, including border space.
    std::size_t outer_width() const;
    std::size_t outer_height() const;

    // Cursor
    bool cursor_visible() const;
    void show_cursor(bool show = true);
    void hide_cursor(bool hide = true);
    void move_cursor_x(std::size_t x);
    void move_cursor_y(std::size_t y);
    std::size_t cursor_x() const;
    std::size_t cursor_y() const;
    Point cursor_coordinates() const;

    void set_visible(bool visible, bool recursive = true);
    bool visible() const;
    bool on_tree() const;
    void set_background_tile(const Glyph& tile);
    void set_background_tile(opt::Optional<Glyph> tile);
    const opt::Optional<Glyph>& background_tile() const;

    virtual void update();

    bool east_border_disqualified() const;
    bool west_border_disqualified() const;
    bool north_border_disqualified() const;
    bool south_border_disqualified() const;

    // Animation
    void enable_animation(Animation_engine::Period_t period);
    void enable_animation(
        const std::function<Animation_engine::Period_t()>& period_func);
    void disable_animation();

    // Public Objects
    Border border;
    Size_policy width_policy{this};
    Size_policy height_policy{this};
    Focus_policy focus_policy{Focus_policy::None};
    Brush brush{background(Color::Black), foreground(Color::White)};

    /// If true, the brush will paint the background tiles.
    bool brush_alters_background() const;
    void set_brush_alters_background(bool alters = true);

    /// Returns the background tile used for this widget.
    Glyph find_background_tile() const;

    // Signals
    sig::Signal<void(const std::string&)> name_changed;
    sig::Signal<void(std::size_t, std::size_t)> resized;
    sig::Signal<void(Point)> moved;
    sig::Signal<void(std::size_t, std::size_t)> moved_xy;
    sig::Signal<void(Widget*)> child_added;
    sig::Signal<void(Widget*)> child_removed;
    sig::Signal<void()> focused_in;
    sig::Signal<void()> focused_out;
    sig::Signal<void(Point)> cursor_moved;
    sig::Signal<void(std::size_t, std::size_t)> cursor_moved_xy;
    sig::Signal<void(Color)> background_color_changed;
    sig::Signal<void(Color)> foreground_color_changed;

    detail::Screen_state& screen_state();
    const detail::Screen_state& screen_state() const;

   protected:
    bool paint_event() override;
    bool repaint_event() override;
    bool close_event() override;
    bool focus_in_event() override;
    bool focus_out_event() override;
    bool deferred_delete_event(Event_handler* to_delete) override;
    bool child_added_event(Widget* child) override;
    bool child_removed_event(Widget* child) override;
    bool child_polished_event(Widget* child) override;
    bool show_event() override;
    bool hide_event() override;
    bool on_tree_event(bool on_tree) override;
    bool move_event(Point new_position, Point old_position) override;
    bool resize_event(Area new_size, Area old_size) override;
    bool animation_event() override;

   private:
    std::string name_;
    Widget* parent_ = nullptr;
    std::vector<std::unique_ptr<Widget>> children_;
    bool visible_{true};
    bool on_tree_{false};

    // Local Coordinates
    Point cursor_position_;
    bool show_cursor_{false};

    opt::Optional<Glyph> background_tile_;
    bool brush_alters_background_{true};

    detail::Screen_state screen_state_;

    // Top left point of *this, relative to the top left of the screen. Does not
    // account for borders.
    Point top_left_position_;

    std::size_t width_{width_policy.hint()};
    std::size_t height_{height_policy.hint()};

    bool east_border_disqualified_{false};
    bool west_border_disqualified_{false};
    bool north_border_disqualified_{false};
    bool south_border_disqualified_{false};

    void set_x(std::size_t global_x);
    void set_y(std::size_t global_y);
};

// - - - - - - - - - - - - - - Free Functions - - - - - - - - - - - - - - - - -
bool has_border(const Widget& w);
void enable_border(Widget& w);
void disable_border(Widget& w);

std::size_t west_border_offset(const Widget& w);
std::size_t east_border_offset(const Widget& w);
std::size_t north_border_offset(const Widget& w);
std::size_t south_border_offset(const Widget& w);

bool has_coordinates(Widget& w, std::size_t global_x, std::size_t global_y);

void move_cursor(Widget& w, Point c);
void move_cursor(Widget& w, std::size_t x, std::size_t y);

// Eventually make Widget::brush private and only provide access through these
// functions that update the background accordingly, though this is restrictive.
void set_background(Widget& w, Color c);
void set_foreground(Widget& w, Color c);
template <typename... Attrs>
void add_attributes(Widget& w, Attrs&... attrs);
template <typename... Attrs>
void remove_attributes(Widget& w, Attrs&... attrs);
void clear_attributes(Widget& w);

void set_background_recursive(Widget& w, Color c, bool single_level = false);
void set_foreground_recursive(Widget& w, Color c, bool single_level = false);

bool has_focus(const Widget& w);

void toggle_cursor(Widget& w);

// - - - - - - - - - - - - Template Implementations - - - - - - - - - - - - - -

template <typename T, typename... Args>
T& Widget::make_child(Args&&... args) {
    this->add_child(std::make_unique<T>(std::forward<Args>(args)...));
    return static_cast<T&>(*children_.back());
}

// Breadth First Search
template <typename T>
T* Widget::find_child(const std::string& name) const {
    std::queue<Widget*> search_queue;
    for (const std::unique_ptr<Widget>& child : this->children()) {
        search_queue.push(child.get());
    }
    while (!search_queue.empty()) {
        Widget* current = search_queue.front();
        search_queue.pop();
        auto cast_child = dynamic_cast<T*>(current);
        if (cast_child != nullptr && current->name() == name) {
            return cast_child;
        }
        for (const std::unique_ptr<Widget>& child : current->children()) {
            search_queue.push(child.get());
        }
    }
    return nullptr;
}

template <typename... Attrs>
void add_attributes(Widget& w, Attrs&... attrs) {
    w.brush.add_attributes(std::forward<Attrs>(attrs)...);
    System::post_event<Repaint_event>(&w);
}

template <typename... Attrs>
void remove_attributes(Widget& w, Attrs&... attrs) {
    for (const auto& at : {attrs...}) {
        w.brush.remove_attribute(at);
    }
    System::post_event<Repaint_event>(&w);
}

}  // namespace cppurses
#endif  // WIDGET_WIDGET_HPP
