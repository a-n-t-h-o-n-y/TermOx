#ifndef CPPURSES_SYSTEM_EVENT_HPP
#define CPPURSES_SYSTEM_EVENT_HPP
#include <string>

namespace cppurses {
class Widget;

/// Base class for types passed around by the Event system.
/** Events encapsulate a behavior to apply to a Widget. Events are created and
 *  passed to the Event_queue to be processed at a later time. */
class Event {
   public:
    /// Event type descriptors for derived types.
    /** Event_queue relies on these for optimizations */
    enum Type {
        None,
        MouseButtonPress,
        MouseButtonRelease,
        MouseButtonDblClick,
        MouseWheel,
        MouseMove,
        KeyPress,
        KeyRelease,
        FocusIn,
        FocusOut,
        Paint,
        Move,
        Resize,
        TerminalResize,
        ChildAdded,
        ChildRemoved,
        ChildPolished,
        Enable,
        Disable,
        Delete,
        Timer,
        Custom
    };

    /// Initializes the \p type and the \p receiver of the Event.
    Event(Type type, Widget& receiver) : type_{type}, receiver_{receiver} {}

    virtual ~Event() = default;

    /// Return a Type enum describing the derived type of the Event.
    auto type() const -> Type { return type_; }

    /// Return a pointer to the Widget that will receiver the Event.
    auto receiver() const -> Widget& { return receiver_; }

    /// Call filter_send() on each installed event filter object in receiver_.
    /** Event filters can be set up with Widget::install_event_filter(). Filters
     *  are used to intercept Events on other Widgets. The first filter to
     *  accept the event by returning true from filter_send stops any other
     *  object from receiving the Event. */
    auto send_to_all_filters() const -> bool;

    /// Call the appropriate event function on the receiver.
    /** Override in derived classes to call on the appropriate member function
     *  event handler on receiver. */
    virtual auto send() const -> bool = 0;

    /// Receive Event calls originally destined for another Widget.
    /** Override in derived classes to call on the appropriate event handler
     *  filter on \p filter, passing in receiver_ as argument. */
    virtual auto filter_send(Widget& filter) const -> bool = 0;

    /// Equality on the type_ and recevier_ member objects.
    /** Used to optimize away duplicate events in Event_queue::append() */
    auto operator==(const Event& other) const -> bool
    {
        return (type_ == other.type_) && (&receiver_ == &other.receiver_);
    }

   protected:
    Type type_;
    Widget& receiver_;
};

/// Convert event enum to string.
auto to_string(Event::Type type) -> std::string;

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENT_HPP
