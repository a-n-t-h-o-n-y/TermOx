# Focus Events

Focus determines which Widget receives Keyboard Events.

## Event Handlers

```cpp
class Widget {
   protected:
    ...
    // Called when...

    // ... Focus has been given to this Widget.
    virtual bool focus_in_event();

    // ... Focus has been taken away from this Widget.
    virtual bool focus_out_event();
    ...
};
```

## Signals

```cpp
class Widget {
    ...
   public:
    Signal<void()> focused_in;
    Signal<void()> focused_out;
    ...
};
```

## Event Filters

```cpp
class Widget {
    ...
   protected:
    virtual bool focus_in_event_filter(Widget& receiver);

    virtual bool focus_out_event_filter(Widget& receiver);
    ...
};
```

## See Also

[Focus Policy](../focus-policy.md)
