# Paint Event

A Paint Event is sent whenever there is a need to re-draw the Widget's display.
This could be from a Resize Event, Move Event, user code calling
`Widget::update()`, or for some other reason. The implementation of this event
handler should create a `Painter` object, which can then be used to put
`Glyph`s to the screen.

## Event Handler

```cpp
class Widget {
   protected:
    ...
    virtual bool paint_event();
    ...
};
```

## Signal

```cpp
class Widget {
    ...
   public:
    Signal<void()> painted;
    ...
};
```

## Event Filter

```cpp
class Widget {
    ...
   protected:
    virtual bool paint_event_filter(Widget& receiver);
    ...
};
```

## See Also

[Painter](../painter.md)
