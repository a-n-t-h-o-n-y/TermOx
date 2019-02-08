#ifndef CPPURSES_SYSTEM_EVENT_HPP
#define CPPURSES_SYSTEM_EVENT_HPP

namespace cppurses {
class Widget;

/// Base class for types passed around by the Event system.
/** Events encapsulate a behavior to apply to a Widget. Events are created and
 *  passed to the Event_queue to be processed at a later time. */
class Event {
   public:
    /// Event type descriptors for derived types.
    /** Event_queue relies on these for optimizations and Event_invoker relies
     *  on these for filtering of Events it will process. */
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

    Event(const Event&) = delete;
    Event& operator=(const Event&) = delete;
    Event(Event&&) = delete;
    Event& operator=(Event&&) = delete;
    virtual ~Event() = default;

    /// Return a Type enum describing the derived type of the Event.
    Type type() const { return type_; }

    /// Return a pointer to the Widget that will receiver the Event.
    Widget& receiver() const { return receiver_; }

    /// Call filter_send() on each installed event filter object in receiver_.
    /** Event filters can be set up with Widget::install_event_filter(). Filters
     *  are used to intercept Events on other Widgets. The first filter to
     *  accept the event by returning true from filter_send stops any other
     *  object from receiving the Event. */
    bool send_to_all_filters() const;

    /// Call the appropriate event function on the receiver.
    /** Override in derived classes to call on the appropriate member function
     *  event handler on receiver. */
    virtual bool send() const = 0;

    /// Receive Event calls originally destined for another Widget.
    /** Override in derived classes to call on the appropriate event handler
     *  filter on \p filter, passing in receiver_ as argument. */
    virtual bool filter_send(Widget& filter) const = 0;

    /// Equality on the type_ and recevier_ member objects.
    /** Used to optimize away duplicate events in Event_queue::append() */
    bool operator==(const Event& other) const {
        return (this->type_ == other.type_) &&
               (&(this->receiver_) == &(other.receiver_));
    }

   protected:
    Type type_;
    Widget& receiver_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENT_HPP
