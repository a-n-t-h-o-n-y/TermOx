# Timer Event

A Timer event is sent to a Widget if it has enabled animation. The Animation
Engine will send Timer Events to the Widget at the requested rate.  This is
useful to update/step the state of the Widget and then repainted it by calling
`Widget::update()` from within the Timer Event handler.

## Event Handler

```cpp
class Widget {
   protected:
    ...
    virtual bool timer_event();
    ...
};
```

## Signal

```cpp
class Widget {
    ...
   public:
    Signal<void()> timer;
    ...
};
```

## Event Filter

```cpp
class Widget {
    ...
   protected:
    virtual bool timer_event_filter(Widget& receiver);
    ...
};
```

## See Also

[Animation](../animation.md)
