# Child Widget Events

Various events related to child Widgets. These should be handled by Layouts
only.

## Event Handlers

```cpp
class Widget {
   protected:
    ...
    // Called on...

    // ... a child Widget is added to the Layout.
    virtual bool child_added_event(Widget& child);

    // ... a child Widget is removed from the Layout.
    virtual bool child_removed_event(Widget& child);

    // ... a child Widget's Size_policy is altered.
    virtual bool child_polished_event(Widget& child);
    ...
};
```

## Signals

```cpp
class Widget {
    ...
   public:
    Signal<void(Widget&)> child_added;
    Signal<void(Widget&)> child_removed;
    Signal<void(Widget&)> child_polished;
    ...
};
```

## Event Filters

```cpp
class Widget {
    ...
   protected:
    virtual bool child_added_event_filter(Widget& receiver, Widget& child);

    virtual bool child_removed_event_filter(Widget& receiver, Widget& child);

    virtual bool child_polished_event_filter(Widget& receiver, Widget& child);
    ...
};
```

## See Also

[Layout](../layout.md)
