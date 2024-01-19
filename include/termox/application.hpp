#pragma once

#include <thread>
#include <variant>

#include <esc/io.hpp>
#include <esc/terminal.hpp>

#include <termox/common.hpp>
#include <termox/events.hpp>
#include <termox/widget.hpp>

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
          quit_{false},
          event_thread_{[](std::stop_token st, EventQueue& queue) {
                            while (!st.stop_requested()) {
                                auto const event = esc::read(16);
                                if (event.has_value()) {
                                    queue.append(esc_to_ox_event(*event));
                                }
                            }
                        },
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
     * @param head_widget The Widget that will be sent events to process.
     */
    template <Widget T>
    auto run(T& head_widget) -> void
    {
        quit_ = false;

        while (!quit_) {
            // Blocking Call
            auto const event = event_queue_.pop();

            auto const result = apply_event(event, head_widget);

            if (result.has_value()) {
                std::visit(
                    Overload{
                        [this](QuitRequest) {
                            event_thread_.request_stop();
                            quit_ = true;
                        },
                        [](Canvas const&) { Terminal::commit_changes(); },
                    },
                    *result);
            }
        }
    }

   private:
    static auto esc_to_ox_event(esc::Event const& ev) -> ox::Event
    {
        using namespace ox::event;
        return std::visit(Overload{
                              [](esc::Mouse_press const& e) -> ox::Event {
                                  return MousePress{e.state};
                              },
                              [](esc::Mouse_release const& e) -> ox::Event {
                                  return MouseRelease{e.state};
                              },
                              [](esc::Scroll_wheel const& e) -> ox::Event {
                                  return MouseWheel{e.state};
                              },
                              [](esc::Mouse_move const& e) -> ox::Event {
                                  return MouseMove{e.state};
                              },
                              [](esc::Key_press const& e) -> ox::Event {
                                  return KeyPress{e.key};
                              },
                              [](esc::Key_release const& e) -> ox::Event {
                                  return KeyRelease{e.key};
                              },
                              [](esc::Window_resize const& e) -> ox::Event {
                                  return Resize{e.new_dimensions};
                              },
                          },
                          ev);
    }

   private:
    EventQueue event_queue_;
    bool quit_;
    std::jthread event_thread_;
};

}  // namespace ox