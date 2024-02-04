#include <termox/core.hpp>

#include <chrono>
#include <future>
#include <sstream>
#include <string>
#include <thread>

using namespace ox;

int main()
{
    auto term = Terminal{};

    class {
       public:
        auto handle_mouse_press(Mouse const& m) -> EventResponse
        {
            if (m.button == Mouse::Button::Left) {
                Painter{}[m.at]
                    << (U'X' | Trait::Bold | Trait::Italic |
                        fg(ColorIndex::Red) | bg(TrueColor{0x8bb14e}))
                    << 'O';
                Terminal::cursor = m.at;
            }
            else if (m.button == Mouse::Button::Right) {
                auto cursor = Painter{}[m.at];
                cursor << ("Right Click" | fg(ColorIndex::BrightCyan));
                cursor << U"Right Click";
                Terminal::cursor = m.at;
            }

            Painter{}[{4, 5}] << ('O' | fg(ColorIndex::Blue))
                              << ('X' | fg(ColorIndex::Red) | Trait::Bold)
                              << 'O';
            return {};
        }

        auto handle_key_press(Key k) -> EventResponse
        {
            if (k == Key::n) {
                timer_.is_running() ? timer_.stop() : timer_.start();
            }
            else if (k == Key::q) {
                return QuitRequest{0};
            }
            else if (k == Key::c) {
                fut_ = std::async(std::launch::async, [this] {
                    std::this_thread::sleep_for(std::chrono::seconds{4});
                    Terminal::event_queue.append(
                        event::Custom{[this]() -> EventResponse {
                            if (fut_.valid()) {
                                fut_.get();
                            }
                            Terminal::event_queue.append(esc::MousePress{
                                {.at     = {.x = 8, .y = 4},
                                 .button = Mouse::Button::Left}});
                            return {};
                        }});
                });
            }
            return {};
        }

        auto handle_timer(int id) -> EventResponse
        {
            ++count_;
            id_ = id;
            this->paint();
            return {};
        }

       private:
        auto paint() -> void
        {
            auto ss = std::basic_stringstream<char32_t>{};
            ss << U"Count: " << (char32_t)(U'0' + count_) << U"  ID: "
               << (char32_t)(U'0' + id_);
            auto const text = ss.str();
            auto p          = Painter{};

            auto cursor = p[{0, 0}];
            for (auto i = 0uL; i < text.size(); ++i) {
                cursor << (text[i] | fg(ColorIndex::BrightBlue));
            }
        }

       private:
        std::future<void> fut_;
        Timer timer_ = Timer{std::chrono::milliseconds{500}};
        int count_   = 0;
        int id_      = -1;
    } w;

    return process_events(term, w);
}