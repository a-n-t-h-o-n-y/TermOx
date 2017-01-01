#ifndef ABSTRACT_EVENT_DISPATCHER_HPP
#define ABSTRACT_EVENT_DISPATCHER_HPP

#include "event.hpp"
#include "object.hpp"

#include <memory>

namespace twf {

class Abstract_event_dispatcher : public Object {
   public:
    virtual bool process_events();
    void interrupt();

   protected:
    virtual void post_user_input() = 0;
    bool interrupt_ = false;
};

}  // namespace twf
#endif  // ABSTRACT_EVENT_DISPATCHER_HPP
