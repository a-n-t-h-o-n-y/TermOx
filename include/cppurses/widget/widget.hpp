#ifndef CPPURSES_WIDGET_WIDGET_HPP
#define CPPURSES_WIDGET_WIDGET_HPP
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <optional/optional.hpp>
#include <signals/signal.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/key.hpp>
#include <cppurses/system/keyboard_data.hpp>
#include <cppurses/system/mouse_data.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/cursor_data.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>

namespace cppurses {
struct Area;

class Widget {
   public:
    explicit Widget(std::string name = "");
    Widget(const Widget&) = delete;
    Widget(Widget&&) = delete;
    Widget& operator=(const Widget&) = delete;
    Widget& operator=(Widget&&) = delete;
    virtual ~Widget();

    /// Return the name of the Widget.
    std::string name() const;

    /// Set the identifying name of the Widget.
    void set_name(std::string name);

    /// Posts an Enable_event to this widget, and all descendants. Will only
    /// post a Child_polished_event to the parent if requested. Useful for
    /// enabling a child Widget from a parent's Child_polished_event handler.
    virtual void enable(bool enable = true,
                        bool post_child_polished_event = true);

    /// Posts a Disable_event to this widget, and all descendants.
    void disable(bool disable = true, bool post_child_polished_event = true);

    /// Check whether the Widget is enabled.
    bool enabled() const;

    /// Posts a Delete_event to this.
    void close();

    // Parent
    Widget* parent() const;

    /// Create a Widget and append it to the list of children. Returns a
    /// reference to this newly created Widget.
    template <typename Widg_t, typename... Args>
    Widg_t& make_child(Args&&... args);

    /// Searches children by name and Widget type, returning a pointer to the
    /// given type if found, or nullptr.
    template <typename Widg_t = Widget>
    Widg_t* find_child(const std::string& name) const;

    /// Searches by name and Widget type for a specific descendant Widget,
    /// breadth first ordering, returning a pointer to the given type if found,
    /// or nullptr.
    template <typename Widg_t = Widget>
    Widg_t* find_descendant(const std::string& name) const;

    /// Top left point of the widget with respect to the top left point of the
    /// screen, not accounting for border space.
    std::size_t x() const;
    std::size_t y() const;

    /// Top left point of the widget with respect to the top left point of the
    /// screen, accounting for border space.
    std::size_t inner_x() const;
    std::size_t inner_y() const;

    // Dimensions, not including border space.
    std::size_t width() const;
    std::size_t height() const;

    /// Dimensions, including border space.
    std::size_t outer_width() const;
    std::size_t outer_height() const;

    virtual void update();

    // Events
    // event_filter.install();
    // event_filter.remove();
    // event_filter.get();
    void install_event_filter(Widget* filter);
    void remove_event_filter(Widget* filter);
    const std::vector<Widget*>& get_event_filters() const;

    // Animation
    void enable_animation(Animation_engine::Period_t period);
    void enable_animation(
        const std::function<Animation_engine::Period_t()>& period_func);
    void disable_animation();

    // Public Objects
    Border border;
    Children_data children{this};
    Cursor_data cursor{this};
    Size_policy width_policy{this};
    Size_policy height_policy{this};
    Focus_policy focus_policy{Focus_policy::None};
    opt::Optional<Glyph> wallpaper;
    Brush brush{background(Color::Black), foreground(Color::White)};

    /// If true, the brush will paint on the wallpaper.
    bool brush_paints_wallpaper() const;

    /// Set if the brush is applied to the wallpaper.
    void set_brush_paints_wallpaper(bool alters = true);

    /// Returns the wallpaper Glyph merged with brush, if enabled.
    Glyph generate_wallpaper() const;

    // Signals
    sig::Signal<void(const std::string&)> name_changed;
    sig::Signal<void(std::size_t, std::size_t)> resized;
    sig::Signal<void(Point)> moved;
    sig::Signal<void(Widget*)> child_added;
    sig::Signal<void(Widget*)> child_removed;
    sig::Signal<void()> focused_in;
    sig::Signal<void()> focused_out;
    sig::Signal<void(Color)> background_color_changed;
    sig::Signal<void(Color)> foreground_color_changed;
    sig::Signal<void(Widget*)> destroyed;
    sig::Signal<void(Point)> clicked;
    sig::Signal<void(std::size_t, std::size_t)> clicked_xy;
    sig::Signal<void(Point)> click_released;
    sig::Signal<void(std::size_t, std::size_t)> click_released_xy;
    sig::Signal<void(Point)> double_clicked;
    sig::Signal<void(std::size_t, std::size_t)> double_clicked_xy;
    sig::Signal<void(Key)> key_pressed;
    sig::Signal<void(Key)> key_released;

    detail::Screen_state& screen_state();
    const detail::Screen_state& screen_state() const;

    // TODO move this once set_parent is in a sub-object
    friend class Children_data;

    friend class Resize_event;
    friend class Move_event;

    // - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - -
    virtual bool enable_event();
    virtual bool disable_event();
    virtual bool child_added_event(Widget* child);
    virtual bool child_removed_event(Widget* child);
    virtual bool child_polished_event(Widget* child);
    virtual bool move_event(Point new_position, Point old_position);
    virtual bool resize_event(Area new_size, Area old_size);
    virtual bool mouse_press_event(const Mouse_data& mouse);
    virtual bool mouse_release_event(const Mouse_data& mouse);
    virtual bool mouse_double_click_event(const Mouse_data& mouse);
    virtual bool mouse_wheel_event(const Mouse_data& mouse);
    virtual bool mouse_move_event(const Mouse_data& mouse);
    virtual bool key_press_event(const Keyboard_data& keyboard);
    virtual bool key_release_event(const Keyboard_data& keyboard);
    virtual bool focus_in_event();
    virtual bool focus_out_event();
    virtual bool delete_event();
    virtual bool paint_event();
    virtual bool timer_event();

    // - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - -
    virtual bool child_added_event_filter(Widget* receiver, Widget* child);
    virtual bool child_removed_event_filter(Widget* receiver, Widget* child);
    virtual bool child_polished_event_filter(Widget* receiver, Widget* child);
    virtual bool enable_event_filter(Widget* receiver);
    virtual bool disable_event_filter(Widget* receiver);
    virtual bool move_event_filter(Widget* receiver,
                                   Point new_position,
                                   Point old_position);
    virtual bool resize_event_filter(Widget* receiver,
                                     Area new_size,
                                     Area old_size);
    virtual bool mouse_press_event_filter(Widget* receiver,
                                          const Mouse_data& mouse);
    virtual bool mouse_release_event_filter(Widget* receiver,
                                            const Mouse_data& mouse);
    virtual bool mouse_double_click_event_filter(Widget* receiver,
                                                 const Mouse_data& mouse);
    virtual bool mouse_wheel_event_filter(Widget* receiver,
                                          const Mouse_data& mouse);
    virtual bool mouse_move_event_filter(Widget* receiver,
                                         const Mouse_data& mouse);
    virtual bool key_press_event_filter(Widget* receiver,
                                        const Keyboard_data& keyboard);
    virtual bool key_release_event_filter(Widget* receiver,
                                          const Keyboard_data& keyboard);
    virtual bool focus_in_event_filter(Widget* receiver);
    virtual bool focus_out_event_filter(Widget* receiver);
    virtual bool delete_event_filter(Widget* receiver);
    virtual bool paint_event_filter(Widget* receiver);
    virtual bool timer_event_filter(Widget* receiver);

   protected:
    void enable_and_post_events(bool enable, bool post_child_polished_event);

   private:
    std::string name_;
    Widget* parent_{nullptr};
    bool enabled_{false};
    bool brush_paints_wallpaper_{true};
    detail::Screen_state screen_state_;

    std::vector<Widget*> event_filters_;

    // Top left point of *this, relative to the top left of the screen. Does not
    // account for borders.
    Point top_left_position_{0, 0};

    std::size_t outer_width_{width_policy.hint()};
    std::size_t outer_height_{height_policy.hint()};

    void set_x(std::size_t global_x);
    void set_y(std::size_t global_y);
    void set_parent(Widget* parent);
};

// - - - - - - - - - - - - - - Free Functions - - - - - - - - - - - - - - - - -
bool has_border(const Widget& w);
void enable_border(Widget& w);
void disable_border(Widget& w);

bool has_coordinates(Widget& w, std::size_t global_x, std::size_t global_y);

// TODO You should no longer need a repaint event for these functions, if the
// background changes in a noticable way, it will be repainted by flush()
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

// - - - - - - - - - - - - Template Implementations - - - - - - - - - - - - - -

template <typename Widg_t, typename... Args>
Widg_t& Widget::make_child(Args&&... args) {
    this->children.add(std::make_unique<Widg_t>(std::forward<Args>(args)...));
    return static_cast<Widg_t&>(*(this->children.get().back()));
}

template <typename Widg_t>
Widg_t* Widget::find_child(const std::string& name) const {
    for (const std::unique_ptr<Widget>& widg : this->children.get()) {
        if (widg->name() == name &&
            dynamic_cast<Widg_t*>(widg.get()) != nullptr) {
            return widg.get();
        }
    }
    return nullptr;
}

template <typename Widg_t>
Widg_t* Widget::find_descendant(const std::string& name) const {
    for (Widget* widg : this->children.get_descendants()) {
        if (widg->name() == name && dynamic_cast<Widg_t*>(widg != nullptr)) {
            return widg;
        }
    }
    return nullptr;
}

template <typename... Attrs>
void add_attributes(Widget& w, Attrs&&... attrs) {
    for (Attribute a : {attrs...}) {
        if (!w.brush.has_attribute(a)) {
            w.brush.add_attributes(std::forward<Attrs>(attrs)...);
            w.update();
            return;
        }
    }
}

template <typename... Attrs>
void remove_attributes(Widget& w, Attrs&&... attrs) {
    bool repaint{false};
    for (const auto& a : {attrs...}) {
        if (w.brush.has_attribute(a)) {
            w.brush.remove_attribute(a);
            repaint = true;
        }
    }
    if (repaint) {
        w.update();
    }
}

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_HPP
