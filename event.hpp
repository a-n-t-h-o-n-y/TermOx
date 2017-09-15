#ifndef SYSTEM_EVENT_HPP
#define SYSTEM_EVENT_HPP

namespace cppurses {
class Widget;

class Event {
   public:
    enum Type {
        None,
        Timer,
        MouseButtonPress,
        MouseButtonRelease,
        MouseButtonDblClick,
        MouseButtonTplClick,
        Wheel,
        MouseMove,
        KeyPress,
        KeyRelease,
        FocusIn,
        FocusOut,
        Enter,
        Leave,
        ClearScreen,
        Paint,
        Move,
        Resize,
        Create,
        Destroy,
        Show,
        Hide,
        Close,
        ChildAdded,
        ChildPolished,
        ChildRemoved,
        EnabledChange,
        DeferredDelete
    };
    // Special Member Functions
    explicit Event(Type type) : type_{type} {}
    virtual ~Event() = default;
    virtual void execute() = 0;

   protected:
    Type type_;
    Widget* widg_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENT_HPP
