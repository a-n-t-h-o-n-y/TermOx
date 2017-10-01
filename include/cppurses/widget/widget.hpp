#ifndef WIDGET_WIDGET_HPP
#define WIDGET_WIDGET_HPP
#include "painter/brush.hpp"
#include "painter/color.hpp"
#include "painter/glyph.hpp"
#include "system/event_handler.hpp"
#include "system/key.hpp"
#include "widget/border.hpp"
#include "widget/coordinates.hpp"
#include "widget/focus_policy.hpp"
#include "widget/size_policy.hpp"
#include <signals/signal.hpp>
#include <algorithm>
#include <cstddef>
#include <memory>
#include <queue>
#include <string>
#include <utility>
#include <vector>

namespace cppurses {

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
    std::vector<Widget*> children() const;
    bool contains_child(Widget* child);

    template <typename T, typename... Args>
    T& make_child(Args&&... args);

    template <typename T>
    T* find_child(const std::string& name);

    template <typename T>
    const T* find_child(const std::string& name) const;

    // Global Coordinates(Including Border)
    std::size_t x() const;
    std::size_t y() const;

    // Dimensions(Not Including Border)
    std::size_t width() const;
    std::size_t height() const;

    // Cursor
    bool cursor_visible() const;
    void show_cursor(bool show = true);
    void hide_cursor(bool hide = true);
    void move_cursor_x(std::size_t x);
    void move_cursor_y(std::size_t y);
    std::size_t cursor_x() const;
    std::size_t cursor_y() const;
    Coordinates cursor_coordinates() const;

    bool visible() const;
    void update();

    // Public Objects
    Border border;
    Size_policy width_policy{this};
    Size_policy height_policy{this};
    Focus_policy focus_policy{Focus_policy::None};
    Glyph background_tile{" "};
    Brush brush{background(Color::Black), foreground(Color::White)};

    // Signals
    sig::Signal<void(const std::string&)> name_changed;
    sig::Signal<void(std::size_t, std::size_t)> resized;
    sig::Signal<void(Coordinates)> moved;
    sig::Signal<void(std::size_t, std::size_t)> moved_xy;
    sig::Signal<void(Widget*)> child_added;
    sig::Signal<void(Widget*)> child_removed;
    sig::Signal<void()> focused_in;
    sig::Signal<void()> focused_out;
    sig::Signal<void(Coordinates)> cursor_moved;
    sig::Signal<void(std::size_t, std::size_t)> cursor_moved_xy;
    sig::Signal<void(Color)> background_color_changed;
    sig::Signal<void(Color)> foreground_color_changed;

   protected:
    bool paint_event() override;
    bool close_event() override;
    bool focus_in_event() override;
    bool focus_out_event() override;
    bool clear_screen_event() override;
    bool deferred_delete_event(Event_handler* to_delete) override;
    bool child_added_event(Widget* child) override;
    bool child_removed_event(Widget* child) override;
    bool child_polished_event(Widget* child) override;
    bool show_event() override;
    bool hide_event() override;
    bool move_event(std::size_t new_x,
                    std::size_t new_y,
                    std::size_t old_x,
                    std::size_t old_y) override;
    bool resize_event(std::size_t new_width,
                      std::size_t new_height,
                      std::size_t old_width,
                      std::size_t old_height) override;

   private:
    std::string name_;
    Widget* parent_ = nullptr;
    std::vector<std::unique_ptr<Widget>> children_;
    bool visible_{true};

    Coordinates cursor_position_;
    bool show_cursor_{false};

    // Top left corner, relative to parent's coordinates.
    Coordinates position_;

    std::size_t width_{width_policy.hint()};
    std::size_t height_{height_policy.hint()};

    void delete_child(Widget* child);
    void set_visible(bool visible);
    void set_x(std::size_t global_x);
    void set_y(std::size_t global_y);

    template <typename T, typename U>
    static auto find_child_impl(U u, const std::string& name)
        -> decltype(u->template find_child<T>(name));
};

// - - - - - - - - - - - - - - Free Functions - - - - - - - - - - - - - - - - -

bool has_border(const Widget& w);
void enable_border(Widget& w);
void disable_border(Widget& w);

bool has_coordinates(Widget& w, std::size_t global_x, std::size_t global_y);

void move_cursor(Widget& w, Coordinates c);
void move_cursor(Widget& w, std::size_t x, std::size_t y);

void set_background(Widget& w, Color c);
void set_foreground(Widget& w, Color c);

bool has_focus(const Widget& w);

// - - - - - - - - - - - - Template Implementations - - - - - - - - - - - - - -

template <typename T, typename... Args>
T& Widget::make_child(Args&&... args) {
    this->add_child(std::make_unique<T>(std::forward<Args>(args)...));
    return static_cast<T&>(*children_.back());
}

template <typename T>
T* Widget::find_child(const std::string& name) {
    return this->find_child_impl<T>(this, name);
}

template <typename T>
const T* Widget::find_child(const std::string& name) const {
    return this->find_child_impl<T>(this, name);
}

// Breadth First Search
template <typename T, typename U>
auto Widget::find_child_impl(U u, const std::string& name)
    -> decltype(u->template find_child<T>(name)) {
    std::queue<U> search_queue;
    search_queue.push(u);
    while (!search_queue.empty()) {
        auto current = search_queue.front();
        search_queue.pop();
        auto widg_T_ptr = dynamic_cast<T*>(current);
        if (widg_T_ptr != nullptr && current->name() == name) {
            return widg_T_ptr;
        }
        auto children = current->children();
        std::for_each(std::begin(children), std::end(children),
                      [&search_queue](Widget* c) { search_queue.push(c); });
    }
    return nullptr;
}

}  // namespace cppurses
#endif  // WIDGET_WIDGET_HPP
