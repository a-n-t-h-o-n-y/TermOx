# Event Loop

An Event Loop is responsible for waiting on an event and then processing all
events on the Event Queue. Each Event Loop is run on its own thread.

## User Input Event Loop

The `User_input_event_loop` is the main event loop, it is run on the main
thread. It's `loop_function` is to wait on user input, then to post the input as
an event to the correct Widget.

## Creating New Event Loops

A new Event Loop can be created by publicly inheriting from the `Event_loop`
class and overriding the `virtual bool loop_function()` method. This overridden
method should wait for some sort of event to occur, once it does, it can post
that event with the `System::post_event(...)` method, and then return `true`. If
no event occurs, but the loop function still needs to return, it can return
`false` so that the event queue is not processed. The new Event Loop can be
owned by a Widget, the `void Event_loop::run_async()` method can be used to
launch the loop in a new thread.

Only one Event Loop is able to process its Event Queue at a time. This provides
thread safely, as event handlers are able to modify Widgets.
