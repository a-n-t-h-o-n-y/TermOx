#pragma once

#include <functional>
#include <thread>
#include <variant>

#include <esc/terminal.hpp>

#include <termox/common.hpp>
#include <termox/events.hpp>
#include <termox/terminal.hpp>

namespace ox {

/**
 * @brief The main application class.
 *
 * An instance of this class will initialize the terminal display upon
 * construction and uninitialize the terminal display upon destruction. The main
 * event loop is run by calling the run() member function.
 */
class Application {
   public:
    /**
     * @brief Constructs the EventQueue and initializes the terminal display.
     */
    explicit Application(esc::Mouse_mode mouse_mode = esc::Mouse_mode::Basic,
                         esc::Key_mode key_mode     = esc::Key_mode::Normal,
                         esc::Signals signals       = esc::Signals::On)
        : event_queue_{},
          terminal_input_thread_{Terminal::run_read_loop<EventQueue>,
                                 std::ref(event_queue_)}
    {
        esc::initialize_interactive_terminal(mouse_mode, key_mode, signals);
    }

    /**
     * @brief Uninitializes the terminal display.
     */
    ~Application() { esc::uninitialize_terminal(); }

   public:
    /**
     * @brief Runs the main event loop.
     *
     * This will block until the application is quit. The application can be
     * quit by responding to an Event handler with a QuitRequest object.
     *
     * @param event_handler The handler that all events will be sent to.
     * @return The return code of the application, passed in via QuitRequest.
     */
    template <typename T>
    auto run(T& event_handler) -> int
    {
        while (true) {
            auto const event = event_queue_.pop();  // Blocking Call
            auto const quit  = apply_event(event, event_handler);

            if (quit.has_value()) {
                terminal_input_thread_.request_stop();
                return quit->return_code;
            }
            else {
                Terminal::commit_changes();
            }
        }
    }

   private:
    EventQueue event_queue_;
    std::jthread terminal_input_thread_;
};

}  // namespace ox