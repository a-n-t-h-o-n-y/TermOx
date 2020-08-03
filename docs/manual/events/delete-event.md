# Delete Event

A Delete Event is sent to a Widget when it has been removed from its parent and
is about to be deleted. The widget will remain alive through the function call,
but is not part of the Widget tree. All child Widgets of this Widget will also
be deleted and be sent this event. Widgets are deleted by calling on
`Widget::Children::erase(...)`.

## Event Handler

```cpp
class Widget {
    ...
   protected:
    virtual bool delete_event();
    ...
};
```

## Signal

```cpp
class Widget {
    ...
   public:
    Signal<void()> destroyed;
    ...
};
```

## Event Filter

```cpp
class Widget {
    ...
   protected:
    virtual bool delete_event_filter(Widget& receiver);
    ...
};
```

## See Also

[Layout](../layout.md)
