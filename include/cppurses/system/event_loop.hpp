#ifndef SYSTEM_EVENT_LOOP_HPP
#define SYSTEM_EVENT_LOOP_HPP
// #include "system/object.hpp"
#include "system/detail/event_invoker.hpp"
#include "system/detail/event_queue.hpp"

namespace cppurses {

class Event_loop {  //: public Object {
   public:
    int run();
    void exit(int return_code);
    detail::Event_queue event_queue;

   private:
    void process_events();

    int return_code_ = 0;
    bool exit_ = false;
    detail::Event_invoker invoker_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENT_LOOP_HPP
