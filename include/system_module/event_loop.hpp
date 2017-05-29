#ifndef SYSTEM_MODULE_EVENT_LOOP_HPP
#define SYSTEM_MODULE_EVENT_LOOP_HPP

#include "system_module/object.hpp"

namespace cppurses {

class Event_loop : public Object {
   public:
    int run();
    void exit(int return_code);
    bool process_events();

   private:
    int return_code_ = 0;
    bool exit_ = false;
};

}  // namespace cppurses
#endif  // SYSTEM_MODULE_EVENT_LOOP_HPP
