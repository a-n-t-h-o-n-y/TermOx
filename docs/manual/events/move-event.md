# Move Event

A Move Event is sent when the top left corner of the Widget has been moved to a
new location on the terminal screen. The `Point` coordinate parameters are
global, with respect to the top left of the terminal screen.

## Event Handler

```cpp
class Widget {
   protected:
    ...
    virtual bool move_event(Point new_position, Point old_position);
    ...
};
```

## Signal

```cpp
class Widget {
    ...
   public:
    Signal<void(Point const&, Point const&)> moved;
    ...
};
```

## Event Filter

```cpp
class Widget {
    ...
   protected:
    virtual bool move_event_filter(Widget& receiver, Point new_position, Point old_position);
    ...
};
```

## See Also

[Point](../point.md)
