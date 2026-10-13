#include <algorithm>
#include <array>
#include <chrono>
#include <deque>
#include <random>
#include <set>

#include <ox/ox.hpp>
#include <signals_light/signal.hpp>

using namespace ox;
using namespace std::chrono_literals;

// This example Uses `SnakeEngine` to run the game logic and `SnakeGameWidget` to
// display and modify (via keyboard and timer) the game state.

class SnakeEngine {
   public:
    constexpr static auto size = Area{100, 27};
    constexpr static auto speed = 300ms;
    constexpr static auto apple_rate = 40;

    struct State {
        // first is tail, last is head
        std::deque<Point> snake = {Point{size.width / 2, size.height / 2}};

        std::set<Point> apple_field = gen_apples(snake, {}, size, 20);

        int score = 0;

        Point next_direction = {.x = 1, .y = 0};

        int frame = 0;

        int level = 6;  // Goes from 6 to 1 (slowest to fastest)
    } state;

   public:
    /// Returns false if game is over, otherwise true.
    [[nodiscard]] auto step() -> bool
    {
        ++state.frame;
        if (state.level != 1 && state.frame % 777 == 0) { --state.level; }
        if (state.frame % state.level != 0) { return true; }

        // Next Head
        if (state.next_direction + last_direction_ == Point{0, 0}) {
            state.next_direction = last_direction_;
        }
        last_direction_ = state.next_direction;

        auto const next_head = state.snake.back() + last_direction_;

        // End Condition
        if (std::ranges::find(state.snake, next_head) != std::cend(state.snake) ||
            next_head.x >= size.width || next_head.y >= size.height ||
            next_head.x < 0 || next_head.y < 0) {
            return false;
        }

        // Increment Snake Head and Decrement Tail
        if (state.apple_field.contains(next_head)) {
            state.apple_field.erase(next_head);
            ++state.score;
        }
        else {
            state.snake.pop_front();
        }
        state.snake.push_back(next_head);

        // Generate New Apple (if there is room).
        if (state.frame % apple_rate == 0 &&
            state.snake.size() < size.width * size.height) {
            auto new_apple = gen_apples(state.snake, state.apple_field, this->size, 1);
            state.apple_field.insert(std::cbegin(new_apple), std::cend(new_apple));
        }

        return true;
    }

    void reset()
    {
        state = State{};
        last_direction_ = {.x = 1, .y = 0};
    }

   private:
    [[nodiscard]] static auto gen_apples(std::deque<Point> const& snake,
                                         std::set<Point> const& apples,
                                         Area bounds,
                                         int count) -> std::set<Point>
    {
        auto result = std::set<Point>{};
        while (count > 0) {
            auto const new_apple = generate_random_point(bounds);
            if (std::ranges::find(snake, new_apple) == std::cend(snake) &&
                not apples.contains(new_apple)) {
                result.insert(new_apple);
                --count;
            }
        }
        return result;
    }

    [[nodiscard]] static auto generate_random_point(Area area) -> Point
    {
        static thread_local auto rd = std::random_device{};
        static thread_local auto generator = std::mt19937{rd()};

        auto x_dist = std::uniform_int_distribution{0, area.width - 1};
        auto y_dist = std::uniform_int_distribution{0, area.height - 1};

        return {.x = x_dist(generator), .y = y_dist(generator)};
    }

   private:
    Point last_direction_{.x = 1, .y = 0};
};

class SnakeGameWidget : public Widget {
   public:
    sl::Signal<void(int)> score_update;

   public:
    SnakeGameWidget() : Widget{FocusPolicy::Strong, SizePolicy::flex()} {}

   public:
    void key_press(KeyEvent k) override
    {
        if (k.matches(U's', {}, k.action)) {
            // Start the game
            if (not timer_.is_running()) {
                engine_.reset();
                timer_.start();
            }
        }
        else if (k.matches(U'j', {}, k.action) ||
                 k.matches(KeyCode::ArrowDown, {}, k.action)) {
            engine_.state.next_direction = {.x = 0, .y = 1};
        }
        else if (k.matches(U'k', {}, k.action) ||
                 k.matches(KeyCode::ArrowUp, {}, k.action)) {
            engine_.state.next_direction = {.x = 0, .y = -1};
        }
        else if (k.matches(U'l', {}, k.action) ||
                 k.matches(KeyCode::ArrowRight, {}, k.action)) {
            engine_.state.next_direction = {.x = 1, .y = 0};
        }
        else if (k.matches(U'h', {}, k.action) ||
                 k.matches(KeyCode::ArrowLeft, {}, k.action)) {
            engine_.state.next_direction = {.x = -1, .y = 0};
        }
    }

    void timer() override
    {
        if (not engine_.step()) { timer_.stop(); }
        this->score_update(engine_.state.score);
    }

    void paint(Canvas c) override
    {
        // If Too Small
        if (engine_.size.width > this->size.width ||
            engine_.size.height > this->size.height) {
            put(c, {0, 0}, U"Terminal Screen is Too Small");
            return;
        }

        // Paint Background
        fill(c, U' ' | bg(colors::black));

        // Paint apples
        for (auto const pt : engine_.state.apple_field) {
            c[pt] = U' ' | bg(colors::bright_red);
        }

        // Paint Snake
        for (auto const pt : engine_.state.snake) {
            c[pt] = U' ' | bg(colors::bright_green);
        }

        // Paint Tail
        auto const tail = std::array{U'░', U'▒', U'▓'};
        for (auto i = 0; i < std::ssize(tail); ++i) {
            if (i + 1 < std::ssize(engine_.state.snake)) {
                c[engine_.state.snake[i]] =
                    tail[i] | fg(colors::bright_green) | bg(colors::bright_black);
            }
        }

        // Press 's' To Start
        if (not timer_.is_running()) {
            auto const text = esc::glyphs("Press 's' to Start") | Trait::Bold;
            auto const center = Point{
                .x = engine_.size.width / 2 - (int)(text.size() / 2),
                .y = engine_.size.height / 2,
            };
            put(c, center, text);
        }
    }

   private:
    SnakeEngine engine_;
    Timer timer_{*this, engine_.speed / 6};
};

int main()
{
    auto const score_style = Style{
        .background = colors::white,
        .foreground = colors::black,
        .traits = Trait::Bold,
    };

    auto head = Column{
        Suspended{
            SnakeGameWidget{} | SizePolicy::suspended(SnakeEngine::size),
            {(U'▚' | fg(IndexedColor{235}) | bg(IndexedColor{234}))},
        },
        Suspended{
            Row{
                Label{{
                    .text = "Score: ",
                    .style = score_style,
                }},
                IntegerLabel{{
                    .value = 0,
                    .align = Align::Right,
                    .style = score_style,
                }},
            } | SizePolicy::suspended({.width = 14, .height = 1}),
            {U' ' | score_style},
        } | SizePolicy::fixed(1),
    };

    Connection{
        .signal = get_child(get_child<0>(head)).score_update,
        .slot = [](int score, auto& int_label) { int_label.value = score; },
    }(get_child<1>(get_child(get_child<1>(head))));

    return Application{head}.run();
}