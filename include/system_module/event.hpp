#ifndef SYSTEM_MODULE_EVENT_HPP
#define SYSTEM_MODULE_EVENT_HPP

#include "system_module/object.hpp"

namespace twf {

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

        // Quit,
        // ParentChange,
        // ParentAboutToChange,
        // ThreadChange,
        // WindowActive,
        // WindowDeactive,
        // ShowToParent,
        // HideToParent,
        // WindowTitleChange,
        // WindowIconChange,
        // ApplicationWindowIconChange,
        // ApplicationFontChange,
        // ApplicationLayoutDirectionChange,
        // ApplicationPaletteChange,
        // PaletteChange,
        // ClipBoard,
        // Speech,
        // SockAct,
        // WinEventAct,
        // DragEnter,
        // DragMove,
        // DragLeave,
        // Drop,
        // DragResponse,
        // ShowWindowRequest,
        // PolishRequest,
        // Polish,
        // LayoutRequest,
        // UpdateRequest,
        // UpdateLater,

        // ContextMenu,
        // InputMethod,
        // AccessibilityPrepare,
        // LocaleChange,
        // LanguageChange,
        // LauoutDirectionChange,
        // Style,
        // OkRequest,
        // HelpRequest,

        // IconDrag,

        // FontChange,
        // ActivationChange,
        // StyleChange,
        // IconTextChange,
        // ModifiedChange,
        // MouseTrackingChange,
        // ToolTip,
        // WhatsThis,
        // StatusTip,

        // ActionChanges,
        // ActionAdded,
        // ActionRemoved,

        // FileOpen,

        // Shortcut,
        // ShortcutOverride,

        // WhatsThisClicked,

        // ToolBarChange,

        // ApplicationActivate,
        // ApplicationDeactivate,

        // QueryWhatsThis,
        // EnterWhatsThisMode,
        // LeaveWhatsThisMode,

        // ZOrderChange,

        // HoverEnter,
        // HoverLeave,
        // HoverMove,

        // AccessibilityHelp,
        // AccessibilityDescription
    };

    explicit Event(Type type) : type_{type} {}
    Event(const Event&) = default;
    Event& operator=(const Event&) = default;
    Event(Event&&) noexcept = default;
    Event& operator=(Event&&) noexcept = default;
    virtual ~Event() = default;

    Type type() const { return type_; }

   protected:
    Type type_;
};

inline bool operator==(const Event& x, const Event& y) {
    return x.type() == y.type();
}

}  // namespace twf
#endif  // SYSTEM_MODULE_EVENT_HPP
