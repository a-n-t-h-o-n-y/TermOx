# Event Loop

An Event Loop is responsible for waiting on an event and then processing all
events on the Event Queue. Each Event Loop is run on its own thread.

## User Input Event Loop

The `User_input_event_loop` is the main event loop, it is run on the main
thread. It's `loop_function` is to wait on user input, then to post the input as
an event to the correct Widget.

## Creating New Event Loops

A new Event Loop can be created by publicly inheriting from the `Event_loop`
class and overriding the `virtual void loop_function()` method. This overridden
method should wait for some sort of event to occur, once it does, it can post
that event with the `System::post_event(...)` method. The new Event Loop can be
owned by a Widget, the `void Event_loop::run_async()` method can be used to
launch the loop in a new thread.

All Event Loops post their events to the same queue, the main thread is the only
thread that processes the Event Queue.
