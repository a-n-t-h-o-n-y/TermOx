#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <exception>
#include <iostream>
#include <optional>
#include <random>
#include <string>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include <termox/core.hpp>

using namespace ox;

constexpr auto logo = std::string_view{
    R"('||''|.   ..|''||   '|.   '|'  ..|'''.|
 ||   || .|'    ||   |'|   |  .|'     '
 ||...|' ||      ||  | '|. |  ||    ....
 ||      '|.     ||  |   |||  '|.    ||
.||.      ''|...|'  .|.   '|   ''|...'|
)"};

// This is a std::variant based state machine, one struct per screen type with
// paint() to put the display for each state type and key_press() and timer()
// to handle events for each state.

// -----------------------------------------------------------------------------

struct SplashScreen {
    std::uint16_t hue{0};
    std::string_view display{logo};
    Timer timer{std::chrono::milliseconds{30}, true};
};

struct MainMenu {
    std::vector<std::string> options = {
        "Play",
        "How-To",
        "Quit",
    };
    std::size_t selected = 0;
};

struct Game {
    static constexpr auto game_space = Area{.width = 81, .height = 25};

    struct AI {
        int action_interval;
        float reaction_threshold;
        float velocity;
    };

    struct Player {
        std::string name;
        std::optional<AI> ai;
    };

    struct Location {
        float x;
        float y;
    };

    struct Paddle {
        Location top;
        float dy;
        static constexpr auto height = 4.f;
    };

    struct {
        std::size_t score = 0;
        Player player;
        Paddle paddle{
            .top = {.x = 1, .y = (game_space.height - Paddle::height) / 2},
            .dy  = 0,
        };
    } left;

    struct {
        std::size_t score = 0;
        Player player;
        Paddle paddle{
            .top = {.x = game_space.width - 2,
                    .y = (game_space.height - Paddle::height) / 2},
            .dy  = 0,
        };
    } right;

    struct Ball {
        Location at = {.x = game_space.width / 2, .y = game_space.height / 2};
        struct Velocity {
            float dx = 0;
            float dy = 0;
        } velocity;
    } ball{};

    Timer timer{std::chrono::milliseconds{30}, true};
};

struct PlayerSelectMenu {
    std::string title{"Select Players"};

    Game::Player left_player = {
        .name = "Human",
        .ai   = std::nullopt,
    };
    Game::Player right_player = {
        .name = "Human",
        .ai   = std::nullopt,
    };

    bool left_selected{true};

    std::vector<Game::Player> options{
        {
            "Slow AI",
            Game::AI{
                .action_interval    = 7,
                .reaction_threshold = 60.f,
                .velocity           = 0.1f,
            },
        },
        {
            "Medium AI",
            Game::AI{
                .action_interval    = 5,
                .reaction_threshold = 30.f,
                .velocity           = 0.2f,
            },
        },
        {
            "Fast AI",
            Game::AI{
                .action_interval    = 3,
                .reaction_threshold = 25.f,
                .velocity           = 0.25f,
            },
        },
        {
            "Aggressive AI",
            Game::AI{
                .action_interval    = 1,
                .reaction_threshold = 10.f,
                .velocity           = 0.75f,
            },
        },
        {
            "Human",
            std::nullopt,
        },
    };
    std::size_t selected{0};
};

struct HowTo {
    std::string title{"How to Play"};
    std::vector<std::array<std::string, 2>> instructions{
        {"Up/Down Arrow Keys", "Move the right paddle"},
        {"w/s Keys", "Move the left paddle"},
        {"Esc Key", "Return to the main menu"},
        {"Enter Key", "Start next round"},
    };
};

using State =
    std::variant<SplashScreen, MainMenu, PlayerSelectMenu, Game, HowTo>;

// -----------------------------------------------------------------------------

auto paint(SplashScreen const& x, Canvas const& c) -> void
{
    auto p   = Painter{c};
    auto hsl = HSL{.hue = x.hue, .saturation = 90, .lightness = 70};

    auto offset = [&]() -> Point {
        auto const text_width = (int)x.display.find('\n');
        return {
            .x = std::max((c.size.width - text_width) / 2, 0),
            .y = 6,
        };
    }();

    auto display = x.display;
    for (auto nl_at = display.find('\n'); nl_at != std::string::npos;
         nl_at      = display.find('\n')) {
        p[offset] << (display.substr(0, nl_at) | Trait::Bold | fg(TColor{hsl}));
        display = display.substr(nl_at + 1);
        hsl.hue = (std::uint16_t)((hsl.hue + 20) % 360);
        offset.y += 1;
    }

    auto const enter = U"Press Enter to continue" | Trait::Dim;
    p[{
        .x = (c.size.width - (int)enter.size()) / 2,
        .y = c.size.height - 1,
    }] << enter;
}

auto paint(MainMenu const& x, Canvas const& c) -> void
{
    auto p = Painter{c};

    {  // Logo
        auto offset = [&]() -> Point {
            auto const width_text = (int)logo.find('\n');
            return {
                .x = std::max((c.size.width - width_text) / 2, 0),
                .y = 6,
            };
        }();

        auto display = logo;
        for (auto nl_at = display.find('\n'); nl_at != std::string::npos;
             nl_at      = display.find('\n')) {
            p[offset] << (display.substr(0, nl_at) | Trait::Dim);
            display = display.substr(nl_at + 1);
            offset.y += 1;
        }
    }

    {  // Menu Items
        for (std::size_t i = 0; i < x.options.size(); ++i) {
            auto cursor = p[{
                .x = (c.size.width - (int)x.options[i].size()) / 2,
                .y = 14 + (int)i,
            }];
            if (i == x.selected) {
                cursor << (x.options[i] | Trait::Standout);
            }
            else {
                cursor << x.options[i];
            }
        }
    }
}

auto paint(HowTo const& x, Canvas const& c) -> void
{
    auto p = Painter{c};

    {  // Border
        p[{0, 0}] << Painter::RoundedBox{
            .area  = c.size,
            .brush = {.foreground = XColor::Blue, .traits = Trait::Dim},
        };
    }
    {  // Title
        auto const at =
            Point{.x = (c.size.width - (int)x.title.size()) / 2, .y = 1};
        p[at] << (x.title | Trait::Bold | Trait::Underline);
    }
    {  // Instructions
        for (std::size_t i = 0; i < x.instructions.size(); ++i) {
            auto const at = Point{
                .x = (c.size.width - 40) / 2,
                .y = 4 + (int)i,
            };
            p[at] << (x.instructions[i][0] | Trait::Bold);
            p[{
                .x = (c.size.width + 8) / 2,
                .y = 4 + (int)i,
            }] << x.instructions[i][1];
        }
    }
    {  // Footer
        p[{.x = 2, .y = c.size.height - 1}]
            << (U"Press Esc to return to the main menu" | Trait::Dim);
    }
}

auto paint(PlayerSelectMenu const& x, Canvas const& c) -> void
{
    auto p = Painter{c};

    {  // Title
        auto const at =
            Point{.x = (c.size.width - (int)x.title.size()) / 2, .y = 1};
        p[at] << x.title;
    }
    {  // Left Player
        auto const at = Point{.x = c.size.width / 2 - 25, .y = 4};
        if (x.left_selected) {
            p[at] << "Left Player: " << (x.left_player.name | Trait::Standout);
        }
        else {
            p[at] << "Left Player: " << x.left_player.name;
        }
    }
    {  // Right Player
        auto const at = Point{.x = c.size.width / 2 + 10, .y = 4};
        if (!x.left_selected) {
            p[at] << "Right Player: "
                  << (x.right_player.name | Trait::Standout);
        }
        else {
            p[at] << "Right Player: " << x.right_player.name;
        }
    }
    {  // Options
        for (std::size_t i = 0; i < x.options.size(); ++i) {
            auto const at = Point{
                .x = (c.size.width - (int)x.options[i].name.size()) / 2,
                .y = 6 + (int)i,
            };

            if (i == x.selected) {
                p[at] << (x.options[i].name | Trait::Standout);
            }
            else {
                p[at] << x.options[i].name;
            }
        }
    }
    {  // Footer
        p[{.x = 0, .y = c.size.height - 1}]
            << (U"Press Esc to return to the main menu" | Trait::Dim);
    }
}

auto paint(Game const& x, Canvas const& c) -> void
{
    static constexpr auto display_space = Game::game_space;

    if (c.size.width < display_space.width ||
        c.size.height < display_space.height) {
        Painter{c}[{0, 0}] << U"Terminal too small to display game";
        return;
    }

    auto game_canvas = Canvas{
        .at   = {.x = c.at.x + (c.size.width - display_space.width) / 2,
                 .y = c.at.y + (c.size.height - display_space.height) / 2},
        .size = display_space,
    };

    {  // Net
        auto p = Painter{game_canvas};
        p[{.x = display_space.width / 2, .y = 0}] << Painter::VLine{
            .length = display_space.height,
            .glyph  = U'╳' | Trait::Dim,
        };
    }

    {  // Ball
        auto const color = [](auto velocity) -> TColor {
            auto const speed = std::sqrt(velocity.dx * velocity.dx +
                                         velocity.dy * velocity.dy);
            return HSL{
                .hue        = (std::uint16_t)((90 + (int)(speed * 160)) % 360),
                .saturation = 80,
                .lightness  = 70,
            };
        }(x.ball.velocity);

        auto const glyph = [](float y) {
            auto const i = (std::size_t)((y - std::floor(y)) * 8);
            return U"▄▃▂▁█▇▆▅"[i];
        }(x.ball.at.y) | fg(color);

        auto const at   = Point{.x = (int)std::round(x.ball.at.x),
                                .y = (int)std::round(x.ball.at.y)};
        game_canvas[at] = glyph | Trait::Inverse;
        game_canvas[{.x = at.x, .y = at.y - 1}] = glyph;
    }

    {  // Paddles
        auto const paint_paddle = [&](Game::Paddle const& paddle) {
            auto const edge = [](float y) -> Glyph {
                auto const i = (std::size_t)((y - std::floor(y)) * 8);
                return {U"█▇▆▅▄▃▂▁"[i]};
            }(paddle.top.y);

            auto at = Point{.x = (int)std::floor(paddle.top.x),
                            .y = (int)std::floor(paddle.top.y)};

            game_canvas[at] = edge;

            for (int i = 1; i < Game::Paddle::height; ++i) {
                at.y += 1;
                game_canvas[at] = U'█' | Trait::None;
            }

            at.y += 1;
            game_canvas[at] = edge | Trait::Inverse;
        };

        paint_paddle(x.left.paddle);
        paint_paddle(x.right.paddle);
    }

    {  // Left Score
        Painter{game_canvas}[{0, -2}] << x.left.player.name << ": "
                                      << std::to_string(x.left.score);
    }

    {  // Right Score
        auto const text =
            x.right.player.name + ": " + std::to_string(x.right.score);
        auto const at = Point{
            .x = display_space.width - (int)text.size() - 1,
            .y = -2,
        };
        Painter{game_canvas}[at] << text;
    }

    {  // Border
        Painter{c}[game_canvas.at + Point{-1, -1}] << Painter::RoundedBox{
            .area  = {.width  = display_space.width + 2,
                      .height = display_space.height + 2},
            .brush = {.foreground = XColor::Blue, .traits = Trait::Dim},
        };
    }

    {  // Footer
        auto p = Painter{c};
        p[{.x = 0, .y = c.size.height - 1}]
            << (U"Press Esc to return to the main menu" | Trait::Dim);

        auto const enter_text =
            std::u32string{U"Press Enter to start next round"};
        p[{
            .x = (c.size.width - (int)enter_text.size()),
            .y = c.size.height - 1,
        }] << (enter_text | Trait::Dim);
    }
}

/**
 * Paints the state to the screen.
 */
auto paint(State const& x) -> void
{
    std::visit([&](auto const& s) { paint(s, Canvas{}); }, x);
}

// -----------------------------------------------------------------------------

/**
 * QuitRequest means the app should quit.
 * State means the app has potentially changed state and should be painted.
 */
using PongEventResponse = std::variant<QuitRequest, State>;

auto key_press(SplashScreen x, Key k) -> PongEventResponse
{
    switch (k) {
        case Key::Enter: return MainMenu{};
        default: return x;
    }
}

auto key_press(MainMenu x, Key k) -> PongEventResponse
{
    switch (k) {
        case Key::ArrowDown:
        case Key::j: x.selected = (x.selected + 1) % x.options.size(); return x;

        case Key::ArrowUp:
        case Key::k:
            x.selected = (x.selected + x.options.size() - 1) % x.options.size();
            return x;

        case Key::Enter:
            switch (x.selected) {
                case 0: return PlayerSelectMenu{};
                case 1: return HowTo{};
                case 2: return QuitRequest{};
            }
            return x;

        default: return x;
    }
}

auto key_press(PlayerSelectMenu x, Key k) -> PongEventResponse
{
    switch (k) {
        case Key::Escape: return MainMenu{};

        case Key::ArrowUp:
        case Key::k: {
            x.selected = (x.selected + x.options.size() - 1) % x.options.size();
            return x;
        }

        case Key::ArrowDown:
        case Key::j: {
            x.selected = (x.selected + 1) % x.options.size();
            return x;
        }

        case Key::Enter: {
            if (x.left_selected) {
                x.left_player   = x.options[x.selected];
                x.left_selected = false;
                return x;
            }
            else {
                x.right_player = x.options[x.selected];
                return Game{
                    .left  = {.player = x.left_player},
                    .right = {.player = x.right_player},
                };
            }
        }
        default: return x;
    }
}

auto key_press(Game x, Key k) -> PongEventResponse
{
    auto const generate_initial_velocity = []() -> Game::Ball::Velocity {
        using int_dist   = std::uniform_int_distribution<int>;
        using float_dist = std::uniform_real_distribution<float>;

        auto rng = std::mt19937{std::random_device{}()};
        auto dx  = float_dist{0.5, 0.8}(rng);
        auto dy  = float_dist{0.1, 0.4}(rng);
        if (int_dist{0, 1}(rng) == 0) {
            dx = -dx;
        }
        if (int_dist{0, 1}(rng) == 0) {
            dy = -dy;
        }
        return {dx, dy};
    };

    constexpr auto speed_increment = 0.5f;

    switch (k) {
        case Key::Escape: return MainMenu{};

        case Key::Enter: {
            auto& ball = x.ball;
            if (ball.velocity.dx == 0 && ball.velocity.dy == 0) {
                ball.velocity = generate_initial_velocity();
            }
            return x;
        }

        case Key::ArrowUp: {
            x.right.paddle.dy -= speed_increment;
            return x;
        }
        case Key::ArrowDown: {
            x.right.paddle.dy += speed_increment;
            return x;
        }

        case Key::w: {
            x.left.paddle.dy -= speed_increment;
            return x;
        }
        case Key::s: {
            x.left.paddle.dy += speed_increment;
            return x;
        }

        default: return x;
    }
}

auto key_press(HowTo x, Key k) -> PongEventResponse
{
    switch (k) {
        case Key::Escape: return MainMenu{};
        default: return x;
    }
}

/**
 * Returns the new state after the key press, or std::nullopt if the app
 * should quit.
 */
auto key_press(State s, Key k) -> PongEventResponse
{
    return std::visit([k](auto x) { return key_press(std::move(x), k); },
                      std::move(s));
}

// -----------------------------------------------------------------------------

auto timer(SplashScreen x, int id) -> PongEventResponse
{
    if (x.timer.id() != id) {
        return x;
    }
    return SplashScreen{
        .hue     = (std::uint16_t)((x.hue + 2) % 360),
        .display = x.display,
        .timer   = std::move(x.timer),
    };
}

auto timer(Game state, int id) -> PongEventResponse
{
    if (state.timer.id() != id) {
        return state;
    }

    auto& ball  = state.ball;
    auto& left  = state.left;
    auto& right = state.right;

    // paddle/ball collisions
    if (ball.velocity.dx < 0.f && ball.at.x < 2.f &&
        ball.at.y + 0.5f >= left.paddle.top.y &&
        ball.at.y - 0.5f <= left.paddle.top.y + Game::Paddle::height) {
        ball.velocity.dx *= -1.07;
        ball.velocity.dy += left.paddle.dy * 0.25f;
        ball.at.x = 2.f;
    }
    else if (ball.velocity.dx > 0.f &&
             ball.at.x > Game::game_space.width - 3.f &&
             ball.at.y + 0.5f >= right.paddle.top.y &&
             ball.at.y - 0.5f <= right.paddle.top.y + Game::Paddle::height) {
        ball.velocity.dx *= -1.07;
        ball.velocity.dy += right.paddle.dy * 0.25f;
        ball.at.x = Game::game_space.width - 3.f;
    }

    // move paddles
    left.paddle.top.y += left.paddle.dy;
    right.paddle.top.y += right.paddle.dy;

    // move ball
    ball.at.x += ball.velocity.dx;
    ball.at.y += ball.velocity.dy;

    // slow down paddles
    left.paddle.dy *= 0.9;
    right.paddle.dy *= 0.9;

    // slow down ball
    ball.velocity.dx *= 0.9999;
    ball.velocity.dy *= 0.999;

    // check for ball/wall collisions
    if (ball.at.y <= 0.5) {
        ball.velocity.dy = -ball.velocity.dy;
        ball.at.y        = 0.51;
    }
    else if (ball.at.y >= Game::game_space.height - 0.5) {
        ball.velocity.dy = -ball.velocity.dy;
        ball.at.y        = Game::game_space.height - 0.51;
    }

    constexpr auto init_ball = Game::Ball{
        .at       = {.x = Game::game_space.width / 2,
                     .y = Game::game_space.height / 2},
        .velocity = {0, 0},
    };

    // check for score and reset
    if (ball.at.x < 0) {
        right.score++;
        ball = init_ball;
    }
    else if (ball.at.x > Game::game_space.width - 1) {
        left.score++;
        ball = init_ball;
    }

    // check for paddle/wall collisions
    if (left.paddle.top.y < 0) {
        left.paddle = {
            .top = {.x = 1, .y = 0},
            .dy  = 0,
        };
    }
    else if (left.paddle.top.y >
             Game::game_space.height - Game::Paddle::height) {
        left.paddle = {
            .top = {.x = 1,
                    .y = Game::game_space.height - Game::Paddle::height},
            .dy  = 0,
        };
    }

    if (right.paddle.top.y < 0) {
        right.paddle = {
            .top = {.x = Game::game_space.width - 2, .y = 0},
            .dy  = 0,
        };
    }
    else if (right.paddle.top.y >
             Game::game_space.height - Game::Paddle::height) {
        right.paddle = {
            .top = {.x = Game::game_space.width - 2,
                    .y = Game::game_space.height - Game::Paddle::height},
            .dy  = 0,
        };
    }

    // AI Left Player
    if (left.player.ai.has_value()) {
        static auto frame_count = 0;

        auto& ai     = *left.player.ai;
        auto& paddle = left.paddle;

        if (ball.velocity.dx < 0.f && ball.at.x < ai.reaction_threshold) {
            if (frame_count % ai.action_interval == 0) {
                auto const paddle_center = paddle.top.y + paddle.height / 2.f;
                auto const distance      = ball.at.y - paddle_center;
                paddle.dy += ai.velocity * std::min(distance, 5.f) /
                             ((ball.at.x / ai.reaction_threshold) + 1);
            }
            ++frame_count;
        }
        else {
            frame_count = 0;
        }
    }

    // AI Right Player
    if (right.player.ai.has_value()) {
        static auto frame_count = 0;

        auto& ai     = *right.player.ai;
        auto& paddle = right.paddle;

        if (ball.velocity.dx > 0.f &&
            ball.at.x > Game::game_space.width - ai.reaction_threshold) {
            if (frame_count % ai.action_interval == 0) {
                auto const paddle_center = paddle.top.y + paddle.height / 2.f;
                auto const distance      = ball.at.y - paddle_center;
                paddle.dy += ai.velocity * std::min(distance, 5.f) /
                             (((Game::game_space.width - ball.at.x) /
                               ai.reaction_threshold) +
                              1);
            }
            ++frame_count;
        }
        else {
            frame_count = 0;
        }
    }

    return state;
}

template <typename T>
auto timer(T x, int /* id */) -> PongEventResponse
{
    return x;
}

auto timer(State s, int id) -> PongEventResponse
{
    return std::visit([&](auto x) { return timer(std::move(x), id); },
                      std::move(s));
}

// -----------------------------------------------------------------------------

class Pong {
   public:
    auto handle_key_press(Key k) -> EventResponse
    {
        return std::visit(
            [this](auto result) -> EventResponse {
                if constexpr (std::is_same_v<decltype(result), QuitRequest>) {
                    return result;
                }
                else {
                    state_ = std::move(result);
                    paint(state_);
                    return {};
                }
            },
            key_press(std::move(state_), k));
    }

    auto handle_timer(int id) -> EventResponse
    {
        return std::visit(
            [this](auto result) -> EventResponse {
                if constexpr (std::is_same_v<decltype(result), QuitRequest>) {
                    return result;
                }
                else {
                    state_ = std::move(result);
                    paint(state_);
                    return {};
                }
            },
            timer(std::move(state_), id));
    }

    auto handle_resize(Area) -> EventResponse
    {
        paint(state_);
        return {};
    }

   private:
    State state_ = SplashScreen{};
};

// -----------------------------------------------------------------------------

int main()
{
    try {
        auto term = Terminal{};
        auto pong = Pong{};

        return process_events(term, pong);
    }
    catch (std::exception const& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown error\n";
        return 1;
    }
}