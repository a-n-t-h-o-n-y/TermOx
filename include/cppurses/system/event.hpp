#ifndef CPPURSES_SYSTEM_EVENT_HPP
#define CPPURSES_SYSTEM_EVENT_HPP

namespace cppurses {
class Widget;

class Event {
   public:
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
        // Enter,
        // Leave,
        // Create,
        // Destroy // TODO Possibly change Delete event to Destroy event?
    };

    // Special Member Functions
    Event(Type type, Widget* receiver);
    Event(const Event&) = default;
    Event& operator=(const Event&) = default;
    Event(Event&&) = default;
    Event& operator=(Event&&) = default;
    virtual ~Event() = default;

    Type type() const;
    Widget* receiver() const;
    void set_receiver(Widget* receiver);

    bool send_to_all_filters() const;
    virtual bool send() const = 0;
    virtual bool filter_send(Widget* filter) const = 0;

    bool operator==(const Event& other) const;

   protected:
    Type type_;
    Widget* receiver_;
};

}  // namespace cppurses
#endif  // CPPURSES_SYSTEM_EVENT_HPP
