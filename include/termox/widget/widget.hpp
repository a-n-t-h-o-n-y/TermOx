#ifndef TERMOX_WIDGET_WIDGET_HPP
#define TERMOX_WIDGET_WIDGET_HPP
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <set>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <signals_light/signal.hpp>

#include <termox/common/fps.hpp>
#include <termox/common/transform_view.hpp>
#include <termox/painter/brush.hpp>
#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/painter.hpp>
#include <termox/system/key.hpp>
#include <termox/system/mouse.hpp>
#include <termox/widget/area.hpp>
#include <termox/widget/cursor.hpp>
#include <termox/widget/focus_policy.hpp>
#include <termox/widget/point.hpp>
#include <termox/widget/size_policy.hpp>

namespace ox {

class Widget {
   public:
    struct Parameters {
        std::string name            = std::string{};
        Focus_policy focus_policy   = Focus_policy::None;
        Size_policy width_policy    = Size_policy{};
        Size_policy height_policy   = Size_policy{};
        Brush brush                 = Brush{};
        Glyph wallpaper             = Glyph{U' '};
        bool brush_paints_wallpaper = true;
        Cursor cursor               = Cursor{};
    };

   private:
    template <typename Signature>
    using Signal = sl::Signal<Signature>;

   public:
    // Event Signals - Alternatives to overriding virtual event handlers.
    /* Called after event handlers are invoked. Parameters are in same order as
     * matching event handler function's parameters. */
    Signal<void()> enabled;
    Signal<void()> disabled;
    Signal<void(Widget&)> child_added;
    Signal<void(Widget&)> child_removed;
    Signal<void(Widget&)> child_polished;
    Signal<void(Point, Point)> moved;
    Signal<void(Area, Area)> resized;
    Signal<void(Mouse const&)> mouse_pressed;
    Signal<void(Mouse const&)> mouse_released;
    Signal<void(Mouse const&)> mouse_wheel_scrolled;
    Signal<void(Mouse const&)> mouse_moved;
    Signal<void(Key)> key_pressed;
    Signal<void()> focused_in;
    Signal<void()> focused_out;
    Signal<void()> deleted;
    Signal<void(Painter&)> painted;
    Signal<void()> timer;

    // Event filter Signals. The first parameter is the original receiver.
    Signal<bool(Widget&)> enabled_filter;
    Signal<bool(Widget&)> disabled_filter;
    Signal<bool(Widget&, Widget&)> child_added_filter;
    Signal<bool(Widget&, Widget&)> child_removed_filter;
    Signal<bool(Widget&, Widget&)> child_polished_filter;
    Signal<bool(Widget&, Point, Point)> moved_filter;
    Signal<bool(Widget&, Area, Area)> resized_filter;
    Signal<bool(Widget&, Mouse const&)> mouse_pressed_filter;
    Signal<bool(Widget&, Mouse const&)> mouse_released_filter;
    Signal<bool(Widget&, Mouse const&)> mouse_wheel_scrolled_filter;
    Signal<bool(Widget&, Mouse const&)> mouse_moved_filter;
    Signal<bool(Widget&, Key)> key_pressed_filter;
    Signal<bool(Widget&)> focused_in_filter;
    Signal<bool(Widget&)> focused_out_filter;
    Signal<bool(Widget&)> deleted_filter;
    Signal<bool(Widget&, Painter&)> painted_filter;
    Signal<bool(Widget&)> timer_filter;

   public:
    /// Describes how focus is given to this Widget.
    Focus_policy focus_policy;

    /// Provides information on where the cursor is and if it is enabled.
    Cursor cursor;

    /// Describes how the width of this Widget should be modified by a Layout.
    Size_policy width_policy;

    /// Describes how the height of this Widget should be modified by a Layout.
    Size_policy height_policy;

    /// A Brush that is applied to every Glyph painted by this Widget.
    Brush brush;

    /// Slots can track this object's lifetime to disable Slot invocations.
    sl::Lifetime lifetime;

   public:
    /// Create an empty Widget.
    explicit Widget(std::string name            = "",
                    Focus_policy focus_policy_  = Focus_policy::None,
                    Size_policy width_policy_   = Size_policy{},
                    Size_policy height_policy_  = Size_policy{},
                    Brush brush_                = Brush{},
                    Glyph wallpaper             = U' ',
                    bool brush_paints_wallpaper = true,
                    Cursor cursor               = Cursor{});

    /// Create an empty Widget.
    explicit Widget(Parameters p);

    virtual ~Widget() = default;

    // Widgets are exclusively owned by std::unique_ptrs and sl::Slots often
    // depend on Widget references to remain valid, copying and moving would
    // invalidate those references.
    Widget(Widget const&) = delete;
    Widget(Widget&&)      = delete;
    Widget& operator=(Widget const&) = delete;
    Widget& operator=(Widget&&) = delete;

   public:
    /// Set the identifying name of the Widget.
    void set_name(std::string name);

    /// Return the name of the Widget.
    [[nodiscard]] auto name() const -> std::string const&;

    /// Return the ID number unique to this Widget.
    [[nodiscard]] auto unique_id() const -> std::uint16_t;

    /// Used to fill in empty space that is not filled in by paint_event().
    void set_wallpaper(Glyph g);

    /// Return the currently in use wallpaper or std::nullopt if none.
    [[nodiscard]] auto get_wallpaper() const -> Glyph;

    /// Enable this Widget and send an Enable_event to itself.
    /** If \p enable is false, Widget is Disabled. */
    void enable(bool enable = true);

    /// Disable this Widget and send a Disable_event to itself.
    /** If \p disable is false, Widget is Enabled. */
    void disable(bool disable = true);

    /// Check whether the Widget is enabled.
    [[nodiscard]] auto is_enabled() const -> bool;

    /// Return the Widget's parent pointer.
    /** The parent is the Widget that owns *this, it  is in charge of
     *  positioning and resizing this Widget. */
    [[nodiscard]] auto parent() const -> Widget*;

    /// Return the global top left corner of this widget.
    [[nodiscard]] auto top_left() const -> Point;

    /// Return the area the widget occupies.
    [[nodiscard]] auto area() const -> Area;

    /// Post a paint event to this Widget.
    virtual void update();

    /** Used by is_paintable to decide whether or not to send a Paint_event.
     *  This is a type parameter, Layout is the only thing that can't paint. */
    [[nodiscard]] virtual auto is_layout_type() const -> bool;

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
    [[nodiscard]] auto get_event_filters() const -> std::set<Widget*> const&;

    /// Enable animation on this Widget.
    /** Animated widgets receiver a Timer_event every \p period. This Timer
     *  Event should be used to update the state of the Widget. This is all
     *  handled on a separate thread from the main user input thread, and has
     *  its own staged_changes object that it paints to to avoid shared data
     *  issues. */
    void enable_animation(std::chrono::milliseconds interval);

    /// Enable animation with a frames-per-second value.
    void enable_animation(FPS fps);

    /// Turn off animation, no more Timer_events will be sent to this Widget.
    /** This Widget will be unregistered from the Animation_engine held by
     *  System. */
    void disable_animation();

    /// Return true if this Widget has animation enabled.
    [[nodiscard]] auto is_animated() const -> bool;

    /// Get a range containing Widget& to each child.
    [[nodiscard]] auto get_children()
    {
        auto constexpr dereference = [](auto& widg_ptr) -> Widget& {
            return *widg_ptr;
        };
        return Transform_view(children_, dereference);
    }

    /// Get a const range containing Widget& to each child.
    [[nodiscard]] auto get_children() const
    {
        auto constexpr dereference = [](auto const& widg_ptr) -> Widget const& {
            return *widg_ptr;
        };
        return Transform_view(children_, dereference);
    }

    /// Return container of all descendants of self_.
    [[nodiscard]] auto get_descendants() const -> std::vector<Widget*>;

    /// Set if the brush is applied to the wallpaper Glyph.
    void paint_wallpaper_with_brush(bool paints = true);

    /// If true, the brush will apply to the wallpaper Glyph.
    [[nodiscard]] auto paints_wallpaper_with_brush() const -> bool;

    /// Return the wallpaper Glyph.
    /** The Glyph has the brush applied to it, if brush_paints_wallpaper is set
     *  to true. */
    [[nodiscard]] auto generate_wallpaper() const -> Glyph;

    /// Return the index of the first child displayed by this Widget.
    [[nodiscard]] auto get_child_offset() const -> std::size_t;

    /// Return the number of children held by this Widget.
    [[nodiscard]] auto child_count() const -> std::size_t;

    // - - - - - - - - - - - - - Event Handlers - - - - - - - - - - - - - - - -
    /// Handles Enable_event objects.
    virtual auto enable_event() -> bool;

    /// Handles Disable_event objects.
    virtual auto disable_event() -> bool;

    /// Handles Child_added_event objects.
    virtual auto child_added_event(Widget& child) -> bool;

    /// Handles Child_removed_event objects.
    virtual auto child_removed_event(Widget& child) -> bool;

    /// Handles Child_polished_event objects.
    virtual auto child_polished_event(Widget& child) -> bool;

    /// Handles Move_event objects.
    virtual auto move_event(Point new_position, Point old_position) -> bool;

    /// Handles Resize_event objects.
    virtual auto resize_event(Area new_size, Area old_size) -> bool;

    /// Handles Mouse_press_event objects.
    virtual auto mouse_press_event(Mouse const& m) -> bool;

    /// Handles Mouse_release_event objects.
    virtual auto mouse_release_event(Mouse const& m) -> bool;

    /// Handles Mouse_wheel_event objects.
    virtual auto mouse_wheel_event(Mouse const& m) -> bool;

    /// Handles Mouse_move_event objects.
    virtual auto mouse_move_event(Mouse const& m) -> bool;

    /// Handles Key_press_event objects.
    virtual auto key_press_event(Key k) -> bool;

    /// Handles Focus_in_event objects.
    virtual auto focus_in_event() -> bool;

    /// Handles Focus_out_event objects.
    virtual auto focus_out_event() -> bool;

    /// Handles Delete_event objects.
    virtual auto delete_event() -> bool;

    /// Handles Paint_event objects.
    virtual auto paint_event(Painter& p) -> bool;

    /// Handles Timer_event objects.
    virtual auto timer_event() -> bool;

    // - - - - - - - - - - - Event Filter Handlers - - - - - - - - - - - - - - -
    /// Handles Enable_event objects filtered from other Widgets.
    virtual auto enable_event_filter(Widget& receiver) -> bool;

    /// Handles Disable_event objects filtered from other Widgets.
    virtual auto disable_event_filter(Widget& receiver) -> bool;

    /// Handles Child_added_event objects filtered from other Widgets.
    virtual auto child_added_event_filter(Widget& receiver, Widget& child)
        -> bool;

    /// Handles Child_removed_event objects filtered from other Widgets.
    virtual auto child_removed_event_filter(Widget& receiver, Widget& child)
        -> bool;

    /// Handles Child_polished_event objects filtered from other Widgets.
    virtual auto child_polished_event_filter(Widget& receiver, Widget& child)
        -> bool;

    /// Handles Move_event objects filtered from other Widgets.
    virtual auto move_event_filter(Widget& receiver,
                                   Point new_position,
                                   Point old_position) -> bool;

    /// Handles Resize_event objects filtered from other Widgets.
    virtual auto resize_event_filter(Widget& receiver,
                                     Area new_size,
                                     Area old_size) -> bool;

    /// Handles Mouse_press_event objects filtered from other Widgets.
    virtual auto mouse_press_event_filter(Widget& receiver, Mouse const& m)
        -> bool;

    /// Handles Mouse_release_event objects filtered from other Widgets.
    virtual auto mouse_release_event_filter(Widget& receiver, Mouse const& m)
        -> bool;

    /// Handles Mouse_wheel_event objects filtered from other Widgets.
    virtual auto mouse_wheel_event_filter(Widget& receiver, Mouse const& m)
        -> bool;

    /// Handles Mouse_move_event objects filtered from other Widgets.
    virtual auto mouse_move_event_filter(Widget& receiver, Mouse const& m)
        -> bool;

    /// Handles Key_press_event objects filtered from other Widgets.
    virtual auto key_press_event_filter(Widget& receiver, Key k) -> bool;

    /// Handles Focus_in_event objects filtered from other Widgets.
    virtual auto focus_in_event_filter(Widget& receiver) -> bool;

    /// Handles Focus_out_event objects filtered from other Widgets.
    virtual auto focus_out_event_filter(Widget& receiver) -> bool;

    /// Handles Delete_event objects filtered from other Widgets.
    virtual auto delete_event_filter(Widget& receiver) -> bool;

    /// Handles Paint_event objects filtered from other Widgets.
    virtual auto paint_event_filter(Widget& receiver, Painter& p) -> bool;

    /// Handles Timer_event objects filtered from other Widgets.
    virtual auto timer_event_filter(Widget& receiver) -> bool;

   private:
    bool enabled_ = false;
    bool brush_paints_wallpaper_;
    bool is_animated_ = false;

   protected:
    using Children_t = std::vector<std::unique_ptr<Widget>>;
    Children_t children_;
    std::size_t child_offset_ = 0;

   private:
    std::string name_;
    Widget* parent_ = nullptr;
    Glyph wallpaper_;
    std::set<Widget*> event_filters_;

    // Top left point of *this, relative to the top left of the screen.
    Point top_left_position_ = {0, 0};

    // The entire area of the widget.
    Area area_ = {0, 0};

    std::uint16_t const unique_id_;

   public:
    /// Should only be used by Move_event send() function.
    void set_top_left(Point p);

    /// Should only be used by Resize_event send() function.
    void set_area(Area a);

    /// Should only be used by Layout.
    void set_parent(Widget* parent);
};

/// Helper function to create a Widget instance.
[[nodiscard]] auto widget(std::string name            = "",
                          Focus_policy focus_policy   = Focus_policy::None,
                          Size_policy width_policy    = Size_policy{},
                          Size_policy height_policy   = Size_policy{},
                          Brush brush                 = Brush{},
                          Glyph wallpaper             = U' ',
                          bool brush_paints_wallpaper = true,
                          Cursor cursor = Cursor{}) -> std::unique_ptr<Widget>;

/// Helper function to create a Widget instance.
[[nodiscard]] auto widget(Widget::Parameters parameters)
    -> std::unique_ptr<Widget>;

/// Wrapper for std::make_unique
template <typename Widget_t, typename... Args>
[[nodiscard]] auto make(Args&&... args) -> std::unique_ptr<Widget_t>
{
    static_assert(std::is_base_of_v<Widget, Widget_t>,
                  "Must make a Widget derived type.");
    return std::make_unique<Widget_t>(std::forward<Args>(args)...);
}

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGET_HPP
