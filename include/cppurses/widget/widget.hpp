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
    /// Initialize with \p name.
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

    /// Posts an Enable_event to this widget, and all descendants.
    /** Will only post a Child_polished_event to the parent if requested. Useful
     *  for enabling a child Widget from a parent's Child_polished_event
     *  handler. This function can be overridden to change the implementation of
     *  what it means to enable a particular Widget type. For instance, if you
     *  have a Widget that has multiple child Widgets but only wants to display
     *  one at a time, then the enable() function can be overridden to enable
     *  itself and then only pass on enable() calls to the children that it
     *  wants to enable. */
    virtual void enable(bool enable = true,
                        bool post_child_polished_event = true);

    /// Posts a Disable_event to this widget, and all descendants.
    /** Will only post a Child_polished_event to the parent if requested. Useful
     *  for disabling a child Widget from a parent's Child_polished_event
     *  handler. */
    void disable(bool disable = true, bool post_child_polished_event = true);

    /// Check whether the Widget is enabled.
    bool enabled() const;

    /// Posts a Delete_event to this, deleting the object when safe to do so.
    void close();

    /// Returns the Widget's parent pointer.
    /** The parent is the Widget that owns *this, it  is in charge of
     *  positioning and resizing this Widget. */
    Widget* parent() const;

    /// Create a Widget and append it to the list of children.
    /** Returns a reference to this newly created Widget. */
    template <typename Widg_t, typename... Args>
    Widg_t& make_child(Args&&... args);

    /// Searches children by name and Widget type.
    /** Returns a pointer to the given type, if found, or nullptr. */
    template <typename Widg_t = Widget>
    Widg_t* find_child(const std::string& name) const;

    /// Searches matching on \p name and Widg_t type for a descendant Widget.
    /** Searches with breadth first ordering over the 'Widget tree'. Returns a
     *  Widg_t* if found, otherwise a nullptr is returned. Returns the first
     *  matching descendant. */
    template <typename Widg_t = Widget>
    Widg_t* find_descendant(const std::string& name) const;

    /// x coordinate for the top left point of this Widget.
    /** Given with relation to the top left of the terminal screen. */
    std::size_t x() const;

    /// y coordinate for the top left point of this Widget.
    /** Given with relation to the top left of the terminal screen. */
    std::size_t y() const;

    /// x coordinate for the top left point of this Widget, beyond the Border.
    /** Given with relation to the top left of the terminal screen. This is the
     *  coordinate that marks the beginning of the space that is availiable for
     *  use by the Widget. */
    std::size_t inner_x() const;

    /// y coordinate for the top left point of this Widget, beyond the Border.
    /** Given with relation to the top left of the terminal screen. This is the
     *  coordinate that marks the beginning of the space that is availiable for
     *  use by the Widget. */
    std::size_t inner_y() const;

    /// Returns the inner width dimension, this does not include Border space.
    std::size_t width() const;

    /// Returns the inner height dimension, this does not include Border space.
    std::size_t height() const;

    /// Returns the width dimension, this includes Border space.
    std::size_t outer_width() const;

    /// Returns the height dimension, this includes Border space.
    std::size_t outer_height() const;

    /// Posts a paint event to itself.
    /** Useful to prompt an update of the Widget when the state of the Widget
     *  has changed. */
    virtual void update();

    /// Install another Widget as an Event filter.
    /** The installed Widget will get the first go at processing the event with
     *  its filter event handler function. Widgets are installed in the order
     *  that calls to this function are made. They are handed the Event in that
     *  same order. If one Widget indicates that it has handled the event it can
     *  return true and no other Widget, including *this, will get the Event. */
    void install_event_filter(Widget* filter);

    /// Remove a Widget from the Event filter list.
    /** No-op if \p filter is not already installed. */
    void remove_event_filter(Widget* filter);

    /// Return the list of Event filter Widgets.
    const std::vector<Widget*>& get_event_filters() const;

    /// Enables animation on this Widget.
    /** Animated widgets receiver a Timer_event every \p period. This Timer
     *  Event should be used to update the state of the Widget. The animation
     *  system will also post a paint event to this Widget so the Widget can
     *  update itself. This is all handled on a separate thread from the main
     *  user input thread, and therefore has its own staged_changes object that
     *  it paints to to avoid shared data issues. */
    void enable_animation(Animation_engine::Period_t period);

    /// Enables variable animation on this Widget.
    /** Animated widgets receiver a Timer_event every \p period_func(). This
     *  enables a variable rate animation. */
    void enable_animation(
        const std::function<Animation_engine::Period_t()>& period_func);

    /// Turns off animation, no more Timer_events will be sent to this Widget.
    /** This Widget will be unregistered from the Animation_engine held by
     *  System. */
    void disable_animation();

    // Public Objects
    /// Describes the visual border of this Widget.
    Border border;

    /// Ownes the children of this Widget and provides access/modification.
    Children_data children{this};

    /// Provides information on where the cursor is and if it is enabled.
    Cursor_data cursor{this};

    /// Describes how the width of this Widget should be modified by a Layout.
    Size_policy width_policy{this};

    /// Describes how the height of this Widget should be modified by a Layout.
    Size_policy height_policy{this};

    /// Describes how focus is given to this Widget.
    Focus_policy focus_policy{Focus_policy::None};

    /// Used to fill in empty space that is not filled in by paint_event().
    opt::Optional<Glyph> wallpaper;

    /// A Brush that is applied to every Glyph painted by this Widget.
    Brush brush{background(Color::Black), foreground(Color::White)};

    /// If true, the brush will paint to the wallpaper Glyph.
    bool brush_paints_wallpaper() const;

    /// Set if the brush is applied to the wallpaper Glyph.
    void set_brush_paints_wallpaper(bool alters = true);

    /// Returns the wallpaper Glyph.
    /** The Glyph has the brush applied to it, if brush_paints_wallpaper is set
     *  to true. */
    Glyph generate_wallpaper() const;

    /// Returns the current Screen_state of this Widget, as it appears.
    detail::Screen_state& screen_state();

    /// Returns the current Screen_state of this Widget, as it appears.
    const detail::Screen_state& screen_state() const;

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

    // TODO move this once set_parent is in a sub-object
    friend class Children_data;

    friend class Resize_event;
    friend class Move_event;

    // - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - -
    /// Handles Enable_event objects.
    virtual bool enable_event();

    /// Handles Disable_event objects.
    virtual bool disable_event();

    /// Handles Child_added_event objects.
    virtual bool child_added_event(Widget* child);

    /// Handles Child_removed_event objects.
    virtual bool child_removed_event(Widget* child);

    /// Handles Child_polished_event objects.
    virtual bool child_polished_event(Widget* child);

    /// Handles Move_event objects.
    virtual bool move_event(Point new_position, Point old_position);

    /// Handles Resize_event objects.
    virtual bool resize_event(Area new_size, Area old_size);

    /// Handles Mouse_press_event objects.
    virtual bool mouse_press_event(const Mouse_data& mouse);

    /// Handles Mouse_release_event objects.
    virtual bool mouse_release_event(const Mouse_data& mouse);

    /// Handles Mouse_double_click_event objects.
    virtual bool mouse_double_click_event(const Mouse_data& mouse);

    /// Handles Mouse_wheel_event objects.
    virtual bool mouse_wheel_event(const Mouse_data& mouse);

    /// Handles Mouse_move_event objects.
    virtual bool mouse_move_event(const Mouse_data& mouse);

    /// Handles Key_press_event objects.
    virtual bool key_press_event(const Keyboard_data& keyboard);

    /// Handles Key_release_event objects.
    virtual bool key_release_event(const Keyboard_data& keyboard);

    /// Handles Focus_in_event objects.
    virtual bool focus_in_event();

    /// Handles Focus_out_event objects.
    virtual bool focus_out_event();

    /// Handles Delete_event objects.
    virtual bool delete_event();

    /// Handles Paint_event objects.
    virtual bool paint_event();

    /// Handles Timer_event objects.
    virtual bool timer_event();

    // - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - -
    /// Handles Child_added_event objects filtered from other Widgets.
    virtual bool child_added_event_filter(Widget* receiver, Widget* child);

    /// Handles Child_removed_event objects filtered from other Widgets.
    virtual bool child_removed_event_filter(Widget* receiver, Widget* child);

    /// Handles Child_polished_event objects filtered from other Widgets.
    virtual bool child_polished_event_filter(Widget* receiver, Widget* child);

    /// Handles Enable_event objects filtered from other Widgets.
    virtual bool enable_event_filter(Widget* receiver);

    /// Handles Disable_event objects filtered from other Widgets.
    virtual bool disable_event_filter(Widget* receiver);

    /// Handles Move_event objects filtered from other Widgets.
    virtual bool move_event_filter(Widget* receiver,
                                   Point new_position,
                                   Point old_position);

    /// Handles Resize_event objects filtered from other Widgets.
    virtual bool resize_event_filter(Widget* receiver,
                                     Area new_size,
                                     Area old_size);

    /// Handles Mouse_press_event objects filtered from other Widgets.
    virtual bool mouse_press_event_filter(Widget* receiver,
                                          const Mouse_data& mouse);

    /// Handles Mouse_release_event objects filtered from other Widgets.
    virtual bool mouse_release_event_filter(Widget* receiver,
                                            const Mouse_data& mouse);

    /// Handles Mouse_double_click_event objects filtered from other Widgets.
    virtual bool mouse_double_click_event_filter(Widget* receiver,
                                                 const Mouse_data& mouse);

    /// Handles Mouse_wheel_event objects filtered from other Widgets.
    virtual bool mouse_wheel_event_filter(Widget* receiver,
                                          const Mouse_data& mouse);

    /// Handles Mouse_move_event objects filtered from other Widgets.
    virtual bool mouse_move_event_filter(Widget* receiver,
                                         const Mouse_data& mouse);

    /// Handles Key_press_event objects filtered from other Widgets.
    virtual bool key_press_event_filter(Widget* receiver,
                                        const Keyboard_data& keyboard);

    /// Handles Key_release_event objects filtered from other Widgets.
    virtual bool key_release_event_filter(Widget* receiver,
                                          const Keyboard_data& keyboard);

    /// Handles Focus_in_event objects filtered from other Widgets.
    virtual bool focus_in_event_filter(Widget* receiver);

    /// Handles Focus_out_event objects filtered from other Widgets.
    virtual bool focus_out_event_filter(Widget* receiver);

    /// Handles Delete_event objects filtered from other Widgets.
    virtual bool delete_event_filter(Widget* receiver);

    /// Handles Paint_event objects filtered from other Widgets.
    virtual bool paint_event_filter(Widget* receiver);

    /// Handles Timer_event objects filtered from other Widgets.
    virtual bool timer_event_filter(Widget* receiver);

   protected:
    /// Enables this Widget and possibly notifies the parent of the change.
    /** This function is useful if you want to override enable() function within
     *  your own derived Widget class. In those cases you could use this
     *  function to enable that Widget and then call enable() on only the
     *  children Widgets that you want enabled. */
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
