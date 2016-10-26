#ifndef EVENT_HPP
#define EVENT_HPP

#include "object.hpp"

namespace mcurses
{

class Event {
public:
	enum class Type {
		None,
		Timer,
		MouseButtonPress,
		MouseButtonRelease,
		MouseButtonDblClick,
		MouseButtonTplClick,
		MouseMove,
		KeyPress,
		KeyRelease,
		FocusIn,
		FocusOut,
		Enter,
		Leave,
		Paint,
		Move,
		Resize,
		Create,
		Destroy,
		Show,
		Hide,
		Close,
		Quit,
		ParentChange,
		ParentAboutToChange,
		ThreadChange,
		WindowActive,
		WindowDeactive,
		ShowToParent,
		HideToParent,
		Wheel,
		WindowTitleChange,
		WindowIconChange,
		ApplicationWindowIconChange,
		ApplicationFontChange,
		ApplicationLayoutDirectionChange,
		ApplicationPaletteChange,
		PaletteChange,
		ClipBoard,
		Speech,
		SockAct,
		WinEventAct,
		DeferredDelete,
		DragEnter,
		DragMove,
		DragLeave,
		Drop,
		DragResponse,
		ChildAdded,
		ChildPolished,
		ChildRemoved,
		ShowWindowRequest,
		PolishRequest,
		Polish,
		LayoutRequest,
		UpdateRequest,
		UpdateLater,
		
		ContextMenu,
		InputMethod,
		AccessibilityPrepare,
		LocaleChange,
		LanguageChange,
		LauoutDirectionChange,
		Style,
		OkRequest,
		HelpRequest,

		IconDrag,

		FontChange,
		EnabledChange,
		ActivationChange,
		StyleChange,
		IconTextChange,
		ModifiedChange,
		MouseTrackingChange,
		ToolTip,
		WhatsThis,
		StatusTip,

		ActionChanges,
		ActionAdded,
		ActionRemoved,

		FileOpen,

		Shortcut,
		ShortcutOverride,

		WhatsThisClicked,

		ToolBarChange,

		ApplicationActivate,
		ApplicationDeactivate,

		QueryWhatsThis,
		EnterWhatsThisMode,
		LeaveWhatsThisMode,

		ZOrderChange,

		HoverEnter,
		HoverLeave,
		HoverMove,

		AccessibilityHelp,
		AccessibilityDescription
	};

	Event(Type type);

	void accept();
	void ignore();
	bool is_accepted() const;
	void set_accepted(bool accept);
	Type type() const;

protected:
	Type type_;
	bool accepted_ = true;
	
};

} // namespace mcurses
#endif // EVENT_HPP