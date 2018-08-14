#ifndef CPPURSES_WIDGET_WIDGET_HPP
#define CPPURSES_WIDGET_WIDGET_HPP
#include <cstddef>
#include <functional>
#include <memory>
#include <string>
#include <utility>

#include <optional/optional.hpp>
#include <signals/signal.hpp>

#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/event_handler.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/cursor_data.hpp>
#include <cppurses/widget/focus_policy.hpp>
#include <cppurses/widget/point.hpp>
#include <cppurses/widget/size_policy.hpp>

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

    // Enable
    void enable(bool enable = true);
    void disable(bool disable = true);

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

    void set_visible(bool visible, bool recursive = true);
    bool visible() const;
    void set_background_tile(const Glyph& tile);
    void set_background_tile(opt::Optional<Glyph> tile);
    const opt::Optional<Glyph>& background_tile() const;

    virtual void update();

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
    sig::Signal<void(Event_handler*)> child_added;
    sig::Signal<void(Event_handler*)> child_removed;
    sig::Signal<void()> focused_in;
    sig::Signal<void()> focused_out;
    sig::Signal<void(Color)> background_color_changed;
    sig::Signal<void(Color)> foreground_color_changed;

    detail::Screen_state& screen_state();
    const detail::Screen_state& screen_state() const;

    // TODO move this once set_parent is in a sub-object
    friend class Children_data;

   protected:
    bool paint_event() override;
    bool close_event() override;
    bool focus_in_event() override;
    bool focus_out_event() override;
    bool deferred_delete_event(Event_handler* to_delete) override;
    bool child_added_event(Widget* child) override;
    bool child_removed_event(Widget* child) override;
    bool child_polished_event(Widget* child) override;
    bool show_event() override;
    bool hide_event() override;
    bool move_event(Point new_position, Point old_position) override;
    bool resize_event(Area new_size, Area old_size) override;
    bool animation_event() override;

   private:
    std::string name_;
    Widget* parent_{nullptr};
    bool visible_{true};

    opt::Optional<Glyph> background_tile_;
    bool brush_alters_background_{true};

    detail::Screen_state screen_state_;

    // Top left point of *this, relative to the top left of the screen. Does not
    // account for borders.
    Point top_left_position_;

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

// You should no longer need a repaint event for these functions, if the
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
