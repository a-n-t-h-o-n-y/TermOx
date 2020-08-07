# Events

- An Event is an action that happens to a specific Widget.

- Event handlers are automatically invoked by CPPurses in response to Events.

- An Event can be 'handled' by overriding the cooresponding virtual function in
  the `Widget` class for that Event type.

- Custom Events can be created for situations where Signals are not a good fit.

- Most Events have a cooresponding signal that might be more convinient that
  overriding a virtual function, but it does not allow you to override a base
  class implementation. It is emitted after the event handler has been invoked.

- Event Filters can be installed to intercept events from other Widgets.

## Event Handlers

Event handlers are virtual functions localed in the `Widget` base class. They
are named after the Event class that they handle. For instance, a `Mouse::Press`
is handled by overriding the `virtual bool mouse_press_event(Mouse::State const&
mouse)` function.  Parameters depend on the event type that is being processed.
If the event handler returns true, that tells the event system that the event
was successfully processed. In certain situations an unsuccessful event will be
sent to its parent for processing.

Most event handlers should call down to the base class' implementation before
returning, unless that is not the desired behavior.

## Event class

Below is the basic outline of the `Event` base class.

```cpp
class Event {
   public:
    enum Type;

    Event(Type type, Widget& receiver);

    auto type() const -> Type;

    auto receiver() const -> Widget&;

   protected:
    virtual auto send() const -> bool = 0;

    virtual auto filter_send(Widget& filter) const -> bool = 0;
};
```

The `Type` enum is used to describe the event type, it's used internally for
optimizing the Event Queue, and for debugging.

[Source File](../../include/cppurses/system/event.hpp)

## Event Filters

A Widget can act as an Event filter on another Widget by installing itself as a
filter on the reciever of the Events that need to be intercepted. Once
installed, all events bound for the receiving Widget will first be sent to the
Event filter Widget via its `..._filter()` virtual function, depending on the
event type.

The filter function is passed a reference to the original receiver of the Event,
as well as any event specific parameters that would normally be passed in. If
the filter function returns true, then the event will not be sent to the
recieving Widget, or any other Event filters.

Any number of Widgets can be installed as filters on a single Widget, they are
sent the Event in the order that the Widgets were installed as filters.

## Custom Events

The `Event` class can be extended to create new event types. This is done by
inheriting publicly from the `Event` class and overriding the two virtual
functions.

The new Event type's constructor will need to call down to the `Event` base
class constructor, passing in the `Event::Type::Custom` enum and a reference to
the receiving Widget.

The `send` virtual override will be called when the event it processed, this
should call some member function of the receivier to handle the response to the
Event.

The `Event::receiver()` method can be used to get a reference to the
receiver widget, but this is a reference to a `Widget` class object, if you need
a more concrete type than this, then capture the object by its concrete type in
the event's constructor and store it within the new event class.

Events can be posted to the Event Queue by calling on:

```cpp
class System {
    ...
   public:
    template <typename T, typename... Args>
    static void post_event(Args&&... args);
    ...
};
```

This will construct an event of type `T` from `args...` and append it to the
Event Queue, to be processed next time the Event Loop is processed. There is
also an overload that takes a `std::unique_ptr<Event>` and appends it.

If an Event needs to be sent right away, the `System::send_event(Event const&)`
method will immediatly send the event to its receiver.

## Event List

- [Child Widget Events](events/child-widget-events.md)
- [Focus Events](events/focus-events.md)
- [Mouse Events](events/mouse-events.md)
- [Key Press Event](events/key-press-event.md)
- [Delete Event](events/delete-event.md)
- [Disable Event](events/disable-event.md)
- [Enable Event](events/enable-event.md)
- [Move Event](events/move-event.md)
- [Resize Event](events/resize-event.md)
- [Paint Event](events/paint-event.md)
- [Timer Event](events/timer-event.md)

## See Also
- [Event Loop](event-loop.md)
- [System](system.md)
