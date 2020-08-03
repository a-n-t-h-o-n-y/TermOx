# Enable Event

An Enable Event is sent when a Widget has been enabled, this means the Widget
will display on screen and have events sent to it.

## Event Handler

```cpp
class Widget {
   protected:
    ...
    virtual bool enable_event();
    ...
};
```

## Signal

```cpp
class Widget {
    ...
   public:
    Signal<void()> enabled;
    ...
};
```

## Event Filter

```cpp
class Widget {
    ...
   protected:
    virtual bool enable_event_filter(Widget& receiver);
    ...
};
```
