#ifndef CPPURSES_WIDGET_WIDGET_HPP
#define CPPURSES_WIDGET_WIDGET_HPP
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <set>
#include <string>
#include <utility>

#include <optional/optional.hpp>
#include <signals/signal.hpp>

#include <cppurses/painter/attribute.hpp>
#include <cppurses/painter/brush.hpp>
#include <cppurses/painter/color.hpp>
#include <cppurses/painter/detail/screen_state.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/system/animation_engine.hpp>
#include <cppurses/system/events/key.hpp>
#include <cppurses/system/events/mouse.hpp>
#include <cppurses/widget/border.hpp>
#include <cppurses/widget/children_data.hpp>
#include <cppurses/widget/cursor_data.hpp>
#include <cppurses/widget/detail/border_offset.hpp>
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
    Widget(Widget&&)      = delete;
    Widget& operator=(const Widget&) = delete;
    Widget& operator=(Widget&&) = delete;
    virtual ~Widget();

    /// Return the name of the Widget.
    std::string name() const { return name_; }

    /// Return the ID number unique to this Widget.
    std::uint16_t unique_id() const { return unique_id_; }

    /// Set the identifying name of the Widget.
    void set_name(std::string name);

    /// Post an Enable_event to this widget, and all descendants.
    /** Will only post a Child_polished_event to the parent if requested. Useful
     *  for enabling a child Widget from a parent's Child_polished_event
     *  handler. This function can be overridden to change the implementation of
     *  what it means to enable a particular Widget type. For instance, if you
     *  have a Widget that has multiple child Widgets but only wants to display
     *  one at a time, then the enable() function can be overridden to enable
     *  itself and then only pass on enable() calls to the children that it
     *  wants to enable. */
    virtual void enable(bool enable                    = true,
                        bool post_child_polished_event = true);

    /// Post a Disable_event to this widget, and all descendants.
    /** Will only post a Child_polished_event to the parent if requested. Useful
     *  for disabling a child Widget from a parent's Child_polished_event
     *  handler. */
    void disable(bool disable = true, bool post_child_polished_event = true)
    {
        this->enable(!disable, post_child_polished_event);
    }

    /// Check whether the Widget is enabled.
    bool enabled() const { return enabled_; }

    /// Post a Delete_event to this, deleting the object when safe to do so.
    /** This Widget is immediately removed from its parent's Children_data
     *  object. The Widget is owned by a Delete_event object until it can be
     *  safely removed without leaving dangling references in event system. */
    void close();

    /// Return the Widget's parent pointer.
    /** The parent is the Widget that owns *this, it  is in charge of
     *  positioning and resizing this Widget. */
    Widget* parent() const { return parent_; }

    /// Create a Widget and append it to the list of children.
    /** Return a reference to this newly created Widget. */
    template <typename Widg_t, typename... Args>
    Widg_t& make_child(Args&&... args)
    {
        this->children.append(
            std::make_unique<Widg_t>(std::forward<Args>(args)...));
        return static_cast<Widg_t&>(*(this->children.get().back()));
    }

    /// Search children by name and Widget type.
    /** Return a pointer to the given type, if found, or nullptr. */
    template <typename Widg_t = Widget>
    Widg_t* find_child(const std::string& name) const
    {
        for (const std::unique_ptr<Widget>& widg : this->children.get()) {
            if (widg->name() == name &&
                dynamic_cast<Widg_t*>(widg.get()) != nullptr) {
                return widg.get();
            }
        }
        return nullptr;
    }

    /// Search matching on \p name and Widg_t type for a descendant Widget.
    /** Search with breadth first ordering over the 'Widget tree'. Return a
     *  Widg_t* if found, otherwise a nullptr is returned. Return the first
     *  matching descendant. */
    template <typename Widg_t = Widget>
    Widg_t* find_descendant(const std::string& name) const
    {
        for (Widget* widg : this->children.get_descendants()) {
            if (widg->name() == name &&
                dynamic_cast<Widg_t*>(widg != nullptr)) {
                return widg;
            }
        }
        return nullptr;
    }

    /// x coordinate for the top left point of this Widget.
    /** Given with relation to the top left of the terminal screen. */
    std::size_t x() const { return top_left_position_.x; }

    /// y coordinate for the top left point of this Widget.
    /** Given with relation to the top left of the terminal screen. */
    std::size_t y() const { return top_left_position_.y; }

    /// x coordinate for the top left point of this Widget, beyond the Border.
    /** Given with relation to the top left of the terminal screen. This is the
     *  coordinate that marks the beginning of the space that is available for
     *  use by the Widget. */
    std::size_t inner_x() const
    {
        return top_left_position_.x + detail::Border_offset::west(*this);
    }

    /// y coordinate for the top left point of this Widget, beyond the Border.
    /** Given with relation to the top left of the terminal screen. This is the
     *  coordinate that marks the beginning of the space that is available for
     *  use by the Widget. */
    std::size_t inner_y() const
    {
        return top_left_position_.y + detail::Border_offset::north(*this);
    }

    /// Return the inner width dimension, this does not include Border space.
    std::size_t width() const
    {
        return this->outer_width() - detail::Border_offset::east(*this) -
               detail::Border_offset::west(*this);
    }

    /// Return the inner height dimension, this does not include Border space.
    std::size_t height() const
    {
        return this->outer_height() - detail::Border_offset::north(*this) -
               detail::Border_offset::south(*this);
    }

    /// Return the width dimension, this includes Border space.
    std::size_t outer_width() const { return outer_width_; }

    /// Return the height dimension, this includes Border space.
    std::size_t outer_height() const { return outer_height_; }

    /// Post a paint event to this Widget.
    /** Useful to prompt an update of the Widget when the state of the Widget
     *  has changed. */
    virtual void update();

    /// Install another Widget as an Event filter.
    /** The installed Widget will get the first go at processing the event with
     *  its filter event handler function. Widgets are installed in the order
     *  that calls to this function are made. They are handed the Event in that
     *  same order. If one Widget indicates that it has handled the event it can
     *  return true and no other Widget, including *this, will get the Event. */
    void install_event_filter(Widget& filter);

    /// Remove a Widget from the Event filter list.
    /** No-op if \p filter is not already installed. */
    void remove_event_filter(Widget& filter);

    /// Return the list of Event filter Widgets.
    auto get_event_filters() const -> const std::set<Widget*>&
    {
        return event_filters_;
    }

    /// Enable animation on this Widget.
    /** Animated widgets receiver a Timer_event every \p period. This Timer
     *  Event should be used to update the state of the Widget. The animation
     *  system will also post a paint event to this Widget so the Widget can
     *  update itself. This is all handled on a separate thread from the main
     *  user input thread, and therefore has its own staged_changes object that
     *  it paints to to avoid shared data issues. */
    void enable_animation(Animation_engine::Period_t period);

    /// Enable variable animation on this Widget.
    /** Animated widgets receiver a Timer_event every \p period_func(). This
     *  enables a variable rate animation. */
    void enable_animation(
        const std::function<Animation_engine::Period_t()>& period_func);

    /// Turn off animation, no more Timer_events will be sent to this Widget.
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

    /// If true, the brush will apply to the wallpaper Glyph.
    bool brush_paints_wallpaper() const { return brush_paints_wallpaper_; }

    /// Set if the brush is applied to the wallpaper Glyph.
    void set_brush_paints_wallpaper(bool paints = true)
    {
        brush_paints_wallpaper_ = paints;
        this->update();
    }

    /// Return the wallpaper Glyph.
    /** The Glyph has the brush applied to it, if brush_paints_wallpaper is set
     *  to true. */
    Glyph generate_wallpaper() const;

    /// Return the current Screen_state of this Widget, as it appears.
    detail::Screen_state& screen_state() { return screen_state_; }

    /// Return the current Screen_state of this Widget, as it appears.
    const detail::Screen_state& screen_state() const { return screen_state_; }

    // Signals
    sig::Signal<void(const std::string&)> name_changed;
    sig::Signal<void(std::size_t, std::size_t)> resized;
    sig::Signal<void(Point)> moved;
    sig::Signal<void(Widget*)> child_added;
    sig::Signal<void(Widget*)> child_removed;
    sig::Signal<void()> focused_in;
    sig::Signal<void()> focused_out;
    sig::Signal<void(Widget&)> destroyed;
    sig::Signal<void(Point)> clicked;
    sig::Signal<void(std::size_t, std::size_t)> clicked_xy;
    sig::Signal<void(Point)> click_released;
    sig::Signal<void(std::size_t, std::size_t)> click_released_xy;
    sig::Signal<void(Point)> double_clicked;
    sig::Signal<void(std::size_t, std::size_t)> double_clicked_xy;
    sig::Signal<void(Key::Code)> key_pressed;
    sig::Signal<void(Key::Code)> key_released;

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
    virtual bool child_added_event(Widget& child);

    /// Handles Child_removed_event objects.
    virtual bool child_removed_event(Widget& child);

    /// Handles Child_polished_event objects.
    virtual bool child_polished_event(Widget& child);

    /// Handles Move_event objects.
    virtual bool move_event(Point new_position, Point old_position);

    /// Handles Resize_event objects.
    virtual bool resize_event(Area new_size, Area old_size);

    /// Handles Mouse::Press objects.
    virtual bool mouse_press_event(const Mouse::State& mouse);

    /// Handles Mouse::Release objects.
    virtual bool mouse_release_event(const Mouse::State& mouse);

    /// Handles Mouse::Double_click objects.
    virtual bool mouse_double_click_event(const Mouse::State& mouse);

    /// Handles Mouse::Wheel objects.
    virtual bool mouse_wheel_event(const Mouse::State& mouse);

    /// Handles Mouse::Move objects.
    virtual bool mouse_move_event(const Mouse::State& mouse);

    /// Handles Key::Press objects.
    virtual bool key_press_event(const Key::State& keyboard);

    /// Handles Key::Release objects.
    virtual bool key_release_event(const Key::State& keyboard);

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
    virtual bool child_added_event_filter(Widget& receiver, Widget& child);

    /// Handles Child_removed_event objects filtered from other Widgets.
    virtual bool child_removed_event_filter(Widget& receiver, Widget& child);

    /// Handles Child_polished_event objects filtered from other Widgets.
    virtual bool child_polished_event_filter(Widget& receiver, Widget& child);

    /// Handles Enable_event objects filtered from other Widgets.
    virtual bool enable_event_filter(Widget& receiver);

    /// Handles Disable_event objects filtered from other Widgets.
    virtual bool disable_event_filter(Widget& receiver);

    /// Handles Move_event objects filtered from other Widgets.
    virtual bool move_event_filter(Widget& receiver,
                                   Point new_position,
                                   Point old_position);

    /// Handles Resize_event objects filtered from other Widgets.
    virtual bool resize_event_filter(Widget& receiver,
                                     Area new_size,
                                     Area old_size);

    /// Handles Mouse::Press objects filtered from other Widgets.
    virtual bool mouse_press_event_filter(Widget& receiver,
                                          const Mouse::State& mouse);

    /// Handles Mouse::Release objects filtered from other Widgets.
    virtual bool mouse_release_event_filter(Widget& receiver,
                                            const Mouse::State& mouse);

    /// Handles Mouse::Double_click objects filtered from other Widgets.
    virtual bool mouse_double_click_event_filter(Widget& receiver,
                                                 const Mouse::State& mouse);

    /// Handles Mouse::Wheel objects filtered from other Widgets.
    virtual bool mouse_wheel_event_filter(Widget& receiver,
                                          const Mouse::State& mouse);

    /// Handles Mouse::Move objects filtered from other Widgets.
    virtual bool mouse_move_event_filter(Widget& receiver,
                                         const Mouse::State& mouse);

    /// Handles Key::Press objects filtered from other Widgets.
    virtual bool key_press_event_filter(Widget& receiver,
                                        const Key::State& keyboard);

    /// Handles Key::Release objects filtered from other Widgets.
    virtual bool key_release_event_filter(Widget& receiver,
                                          const Key::State& keyboard);

    /// Handles Focus_in_event objects filtered from other Widgets.
    virtual bool focus_in_event_filter(Widget& receiver);

    /// Handles Focus_out_event objects filtered from other Widgets.
    virtual bool focus_out_event_filter(Widget& receiver);

    /// Handles Delete_event objects filtered from other Widgets.
    virtual bool delete_event_filter(Widget& receiver);

    /// Handles Paint_event objects filtered from other Widgets.
    virtual bool paint_event_filter(Widget& receiver);

    /// Handles Timer_event objects filtered from other Widgets.
    virtual bool timer_event_filter(Widget& receiver);

   protected:
    /// Enable this Widget and possibly notify the parent of the change.
    /** This function is useful if you want to override enable() function within
     *  your own derived Widget class. In those cases you could use this
     *  function to enable that Widget and then call enable() on only the
     *  children Widgets that you want enabled. */
    void enable_and_post_events(bool enable, bool post_child_polished_event);

   private:
    std::string name_;
    const std::uint16_t unique_id_;
    Widget* parent_{nullptr};
    bool enabled_{false};
    bool brush_paints_wallpaper_{true};
    detail::Screen_state screen_state_;
    std::set<Widget*> event_filters_;

    // Top left point of *this, relative to the top left of the screen. Does not
    // account for borders.
    Point top_left_position_{0, 0};

    std::size_t outer_width_{width_policy.hint()};
    std::size_t outer_height_{height_policy.hint()};

    void set_x(std::size_t global_x) { top_left_position_.x = global_x; }

    void set_y(std::size_t global_y) { top_left_position_.y = global_y; }

    void set_parent(Widget* parent) { parent_ = parent; }
};

}  // namespace cppurses
#endif  // CPPURSES_WIDGET_WIDGET_HPP
