# Disable Event

A Disable Event is sent when a Widget has been disabled, this means the Widget
is no longer visible on screen and will not receive most event types.

## Event Handler

```cpp
class Widget {
   protected:
    ...
    virtual bool disable_event();
    ...
};
```

## Signal

```cpp
class Widget {
    ...
   public:
    Signal<void()> disabled;
    ...
};
```

## Event Filter

```cpp
class Widget {
    ...
   protected:
    virtual bool disable_event_filter(Widget& receiver);
    ...
};
```
