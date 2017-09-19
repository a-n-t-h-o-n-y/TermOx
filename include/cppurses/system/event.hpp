#ifndef SYSTEM_EVENT_HPP
#define SYSTEM_EVENT_HPP

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
        ClearScreen,
        Paint,
        Move,
        Resize,
        Show,
        Hide,
        Close,
        ChildAdded,
        ChildRemoved,
        ChildPolished,
        Enable,
        Disable,
        DeferredDelete
        // Timer,
        // Enter,
        // Leave,
        // Create,
        // Destroy
    };

    // Special Member Functions
    explicit Event(Type type, Widget* receiver);
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
    virtual bool filter_send(Widget* filter_widget) const = 0;

   protected:
    const Type type_;
    Widget* receiver_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENT_HPP
