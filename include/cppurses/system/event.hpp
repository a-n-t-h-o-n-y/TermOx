#ifndef SYSTEM_EVENT_HPP
#define SYSTEM_EVENT_HPP

namespace cppurses {
class Widget;

class Event {
   public:
    enum Type {
        None,
        // Timer,
        MouseButtonPress,
        MouseButtonRelease,
        MouseButtonDblClick,
        MouseWheel,
        MouseMove,
        KeyPress,
        KeyRelease,
        // FocusIn,
        // FocusOut,
        // Enter,
        // Leave,
        // ClearScreen,
        // Paint,
        // Move,
        // Resize,
        // Create,
        // Destroy,
        // Show,
        // Hide,
        // Close,
        // ChildAdded,
        // ChildPolished,
        // ChildRemoved,
        // EnabledChange,
        // DeferredDelete
    };

    // Special Member Functions
    explicit Event(Type type, Widget* reciever);
    Event(const Event&) = default;
    Event& operator=(const Event&) = default;
    Event(Event&&) = default;
    Event& operator=(Event&&) = default;
    virtual ~Event() = default;

    Type type() const;

    virtual void send() const = 0;

   protected:
    const Type type_;
    Widget* reciever_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENT_HPP
