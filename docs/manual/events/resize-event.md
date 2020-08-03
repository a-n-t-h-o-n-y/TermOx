# Resize Event

A Resize Event is sent to a Widget when it's width and/or height have changed.

## Event Handler

```cpp
class Widget {
   protected:
    ...
    virtual bool resize_event(Area new_size, Area old_size);
    ...
};
```

## Signal

```cpp
class Widget {
    ...
   public:
    Signal<void(Area const&, Area const&)> resized;
    ...
};
```

## Event Filter

```cpp
class Widget {
    ...
   protected:
    virtual bool resize_event_filter(Widget& receiver, Area new_size, Area old_size);
    ...
};
```

## See Also

[Area](../area.md)
