#ifndef SYSTEM_ABSTRACT_EVENT_DISPATCHER_HPP
#define SYSTEM_ABSTRACT_EVENT_DISPATCHER_HPP

#include "system/object.hpp"

namespace cppurses {

class Abstract_event_dispatcher : public Object {
   public:
    virtual bool process_events();
    void interrupt();

   protected:
    virtual void post_user_input() = 0;
    bool interrupt_{false};
};

}  // namespace cppurses
#endif  // SYSTEM_ABSTRACT_EVENT_DISPATCHER_HPP
