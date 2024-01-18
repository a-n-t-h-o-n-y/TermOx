#pragma once

#include <variant>

#include <termox/common.hpp>
#include <termox/events.hpp>
#include <termox/widget.hpp>

namespace ox {

/**
 * @brief The main application class.
 *
 * This is the main class that the user will interact with. It is responsible
 * for running the main event loop, and managing the head Widget.
 *
 * @tparam T The type of the head Widget.
 */
template <Widget T>
class Application {
   public:
    Application(T& head_widget) : head_widget_{head_widget} {}

   public:
    /**
     * @brief Runs the main event loop.
     *
     * This will block until the application is quit. The application can be
     * quit by responding to an Event handler with a QuitRequest object.
     */
    auto run() -> void
    {
        quit_ = false;

        while (!quit_) {
            // Blocking Call
            auto const event = event_queue_.pop();

            auto const result = apply_event(event, head_widget_);

            if (result.has_value()) {
                std::visit(Overload{
                               [this](QuitRequest) { quit_ = true; },
                               [this](Canvas const&) {
                                   // TODO tell terminal to paint
                               },
                           },
                           *result);
            }
        }
    }

   private:
    T& head_widget_;
    EventQueue event_queue_;
    bool quit_{false};
};

}  // namespace ox