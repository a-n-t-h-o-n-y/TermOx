#include <termox/core.hpp>

#include <sstream>
#include <string>

using namespace ox;

int main()
{
    auto term = Terminal{};

    class {
       public:
        // auto handle_mouse_press(Mouse const& m) -> EventResponse
        // {
        //     if (m.button == Mouse::Button::Left) {
        //         Painter{}[m.at] = U'X' | Trait::Bold | Trait::Italic |
        //                           fg(ColorIndex::Red) |
        //                           bg(TrueColor{0x8bb14e});
        //         Terminal::cursor = m.at;
        //     }
        //     return {};
        // }

        auto handle_key_press(Key k) -> EventResponse
        {
            if (k == Key::n) {
                timer_.is_running() ? timer_.stop() : timer_.start();
            }
            else if (k == Key::q) {
                return QuitRequest{0};
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

            for (auto i = 0uL; i < text.size(); ++i) {
                p[{.x = (int)i, .y = 0}] = text[i] | fg(ColorIndex::BrightBlue);
            }
        }

       private:
        Timer timer_ = Timer{std::chrono::milliseconds{500}};
        int count_   = 0;
        int id_      = -1;
    } w;

    return process_events(term, w);
}