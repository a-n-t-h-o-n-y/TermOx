#ifndef EVENT_LOOP_HPP
#define EVENT_LOOP_HPP

#include "object.hpp"

namespace mcurses {

class Event_loop : public Object {
   public:
    int run();
    void exit(int return_code);
    bool process_events();

   private:
    int return_code_ = 0;
    bool exit_ = false;
};

}  // namespace mcurses
#endif  // EVENT_LOOP_HPP
