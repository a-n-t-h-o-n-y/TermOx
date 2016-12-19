#include <painter_module/paint_engine.hpp>
#include <system_module/detail/thread_data.hpp>
#include <system_module/event_loop.hpp>
// #include <system_module/system.hpp>

namespace mcurses {

int Event_loop::run() {
    auto& data = detail::Thread_data::current();
    data.event_loops.push(this);

    while (!exit_) {
        this->process_events(); // all paints are made to the buffer
        // System::paint_engine()->flush(); 
        
        // all edits above are flushed to screen
        // so what is happening is that process_events takes user input last and
        // all it does is post the user event, the event is not sent yet.
        // then the screen if flushed without the user input visible yet.
        // the loop starts over, processs_events() processes the events from last time
        // this does not mean they are put to the screen, they are put to the buffer
        // once the process_events() function has exited, then the buffer is flushed
        // and you get the last events. The problem is that the process events
        // just waits for the loop to go around again to print the events, but
        // the print doesn't happen until the process events function leaves again.
        // so you need two send_posted_events() functions in the process_events
        // loop, just like in that another look at events article.. :)
        // so it will take user input, process that input to the buffer, and 
        // possibly post more events, process those events and put their screen
        // changes to the buffer, and then if will flush the buffer to the screen.
        // Then it will send posted events(probably get rid of this) then block
        // for user input, then process that input, then flush the screen.
        // Two solutions, don't send events until after you have taken user input
        //      problem here is that events that are posted before the ui program
        //      starts will not be sent until after the first user interaction. not acceptable.
        // The second solution is to have the flush call, and then the process events.
        // but this is wrong. you need to have the flush inside the process events
        // function. Can you do that? it is through the system header, so it should work.
        // you process user input, you flush it to the screen, then you wait for user
        // input, once you get it you post and event, then send events(to buffer) then
        // you flush the buffer.
    }

    data.event_loops.pop();

    return return_code_;
}

void Event_loop::exit(int return_code) {
    return_code_ = return_code;
    exit_ = true;
    detail::Thread_data::current().dispatcher().interrupt();
}

bool Event_loop::process_events() {
    auto& data = detail::Thread_data::current();
    return data.dispatcher().process_events();
}

}  // namespace mcurses
