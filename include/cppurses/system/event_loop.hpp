#ifndef SYSTEM_EVENT_LOOP_HPP
#define SYSTEM_EVENT_LOOP_HPP
#include <functional>
#include <thread>

#include <cppurses/painter/detail/staged_changes.hpp>
#include <cppurses/system/detail/event_invoker.hpp>
#include <cppurses/system/detail/event_queue.hpp>

namespace cppurses {

class Event_loop {
   public:
    explicit Event_loop(const std::function<void()>& loop_function);

    int run();
    void exit(int return_code);

    std::thread::id get_thread_id() const;

    const detail::Staged_changes& staged_changes() const;
    detail::Staged_changes& staged_changes();

    detail::Event_queue event_queue;

   private:
    void process_events();

    int return_code_{0};
    bool exit_{false};
    bool running_{false};
    detail::Event_invoker invoker_;
    detail::Staged_changes staged_changes_;
    std::thread::id thread_id_;
    std::function<void()> loop_func_;
};

}  // namespace cppurses
#endif  // SYSTEM_EVENT_LOOP_HPP
