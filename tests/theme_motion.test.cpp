#include <cmath>
#include <memory>
#include <ox/ox.hpp>
#include <zzz/test.hpp>
using namespace ox;
using namespace std::chrono_literals;
TEST(theme_roles_preserve_explicit_colors_and_validate_motion)
{
    auto theme = Theme::dark();
    auto style = theme_style({}, theme, Surface::Raised);
    ASSERT(style.background == theme.palette.surface &&
           style.foreground == theme.palette.foreground);
    auto custom = Style{
        .background = colors::blue, .foreground = colors::red, .traits = Trait::Bold};
    ASSERT(theme_style(custom, theme) == custom);
    ASSERT(theme_style({}, Theme::plain()) == Style{});
    theme.motion.frames_per_second = 0;
    ASSERT_THROWS(validate(theme), std::invalid_argument);
    theme.motion.frames_per_second = 60;
    theme.motion.feedback = -1ms;
    ASSERT_THROWS(validate(theme), std::invalid_argument);
    ASSERT(std::get<RGB>(mix_color(RGB{0, 0, 0}, RGB{100, 200, 50}, .5)) ==
           RGB(50, 100, 25));
    ASSERT(mix_color(colors::black, colors::yellow, .1) == Color{colors::yellow});
}
TEST(tween_uses_elapsed_time_retargets_continuously_and_settles_exactly)
{
    auto t = Tween{};
    auto now = Tween::Clock::now();
    t.to(1, {100ms, Ease::Linear}, now);
    t.sample(now + 25ms);
    ASSERT(t.value() == .25 && t.active());
    t.to(0, {100ms, Ease::OutCubic}, now + 25ms);
    ASSERT(t.value() == .25);
    t.sample(now + 125ms);
    ASSERT(t.value() == 0 && !t.active());
    for (auto ease : {Ease::Linear, Ease::OutCubic, Ease::InOutCubic, Ease::Spring}) {
        t.reset(0);
        t.to(1, {100ms, ease}, now);
        t.sample(now + 50ms);
        ASSERT(t.value() > 0 && t.value() < 1);
        t.sample(now + 2s);
        ASSERT(t.value() == 1 && !t.active());
    }
    ASSERT_THROWS(t.to(INFINITY), std::invalid_argument);
    ASSERT_THROWS(t.to(1, {-1ms}), std::invalid_argument);
    t.to(5, {0ms}, now);
    ASSERT(t.value() == 5 && !t.active());
}
namespace {
struct Moving : Widget {
    Animation x{*this};
    auto paint(Canvas c) -> Cursor override
    {
        if (c.size().width) c.write({0, 0}, U"x", appearance({}));
        return {};
    }
};
}  // namespace
TEST(shared_motion_clock_caps_frames_follows_moves_and_sleeps_when_settled)
{
    auto moving = Moving{};
    auto app = Application{moving};
    auto theme = Theme::dark();
    theme.motion.frames_per_second = 30;
    app.set_theme(theme);
    app.dispatch(esc::Resize{{8, 2}});
    app.paint();
    auto now = Tween::Clock::now();
    moving.x.to(1, {100ms, Ease::Linear}, now);
    app.step(now + 20ms);
    ASSERT(moving.x.value() == .2);
    app.paint();
    app.step(now + 25ms);
    ASSERT(moving.x.value() == .2);
    auto moved = std::move(moving);
    app.step(now + 100ms);
    ASSERT(moved.x.value() == 1 && !moved.x.active());
    app.paint();
    ASSERT(app.loop().wait_duration(now + 100ms) == std::chrono::milliseconds::max());
    moved.x.to(0, {100ms, Ease::Linear}, now + 100ms);
    theme.motion.enabled = false;
    app.set_theme(theme);
    app.step(now + 110ms);
    ASSERT(moved.x.value() == 0 && !moved.x.active());
    app.paint();
    ASSERT(app.loop().wait_duration(now + 110ms) == std::chrono::milliseconds::max());
}
TEST(animation_callbacks_can_destroy_their_owner_without_touching_freed_storage)
{
    auto widget = std::make_unique<Moving>();
    auto app = Application{*widget};
    auto now = Tween::Clock::now();
    widget->x.on_frame([&](double) {
        widget.reset();
        return EventResponse{};
    });
    widget->x.to(1, {100ms}, now);
    app.step(now + 50ms);
    ASSERT(!widget);
}
TEST(themed_widgets_keep_immediate_values_and_animate_only_their_presentation)
{
    auto progress = ProgressBar{{.show_percentage = false}};
    auto app = Application{progress};
    app.set_theme(Theme::dark());
    app.dispatch(esc::Resize{{8, 1}});
    app.paint();
    int calls = 0;
    progress.on_change.connect([&](double v) {
        ASSERT(v == 100);
        ++calls;
    });
    progress.set_value(100);
    ASSERT(progress.value() == 100 && calls == 1);
    auto now = Tween::Clock::now();
    app.step(now + 1s);
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].text == U"█"));
    auto t = Theme::terminal();
    t.motion.enabled = false;
    app.set_theme(t);
    app.step(now + 2s);
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].text == U"#"));
}
TEST(fractional_meters_clip_quantize_and_keep_their_style)
{
    auto f = Frame{{2, 1}};
    auto s = Style{.foreground = colors::red};
    paint_meter(Canvas{f}, .0625, s, {});
    ASSERT((f[Point{0, 0}].text == U"▏" && f[Point{0, 0}].style == s));
    ASSERT((f[Point{1, 0}].text == U"─"));
    paint_meter(Canvas{f}, 2, s, {}, false, true);
    ASSERT((f[Point{1, 0}].text == U"#"));
    auto vertical = Frame{{1, 2}};
    paint_meter(Canvas{vertical}, .25, s, {}, true);
    ASSERT((vertical[Point{0, 1}].text == U"▄"));
    ASSERT_THROWS(paint_meter(Canvas{f}, NAN, s, {}), std::invalid_argument);
}
TEST(theme_changes_reflow_tabs_and_hidden_pages_inherit_the_application_theme)
{
    auto tabs = Tabs{std::tuple{LineEdit{}, LineEdit{}},
                     std::vector<std::string>{"One", "Two"}};
    auto app = Application{tabs};
    app.dispatch(esc::Resize{{20, 5}});
    auto t = Theme::dark();
    t.motion.enabled = false;
    app.set_theme(t);
    app.step();
    app.paint();
    ASSERT(std::get<0>(tabs.pages.children).at().y == 0);
    ASSERT(tabs.pages.at().y == 2);
    tabs.select(1);
    app.step();
    app.paint();
    ASSERT(std::get<1>(tabs.pages.children).theme().palette.accent == t.palette.accent);
    ASSERT((app.loop().screen[Point{5, 1}].text == U"╺"));
    app.set_theme(Theme::plain());
    app.step();
    ASSERT(tabs.pages.at().y == 1);
}
TEST(key_help_and_dispatch_share_bindings_and_ignore_repeats_and_disabled_actions)
{
    int calls = 0;
    auto help = KeyHelp{
        {Shortcut{.keys = {{KeyCode::F2, {}}}, .description = "theme", .action = [&] {
                      ++calls;
                      return EventResponse{};
                  }}}};
    auto app = Application{help};
    app.dispatch(esc::Resize{{30, 2}});
    app.dispatch(KeyEvent{.key = KeyCode::F2});
    ASSERT(calls == 1);
    app.dispatch(KeyEvent{.key = KeyCode::F2, .action = KeyAction::Repeat});
    ASSERT(calls == 1);
    help.bindings[0].enabled = false;
    app.dispatch(KeyEvent{.key = KeyCode::F2});
    ASSERT(calls == 1);
    ASSERT(key_label({U'p', {.ctrl = true}}, true) == "ctrl+p");
    ASSERT(key_label({KeyCode::ArrowUp, {}}, false) == "↑");
}
TEST(animation_frame_callbacks_can_reenter_the_event_loop)
{
    auto widget = Moving{};
    auto app = Application{widget};
    auto now = Tween::Clock::now();
    int frames = 0;
    widget.x.on_frame([&](double) {
        ++frames;
        app.step(now + 50ms);
        return EventResponse{};
    });
    widget.x.to(1, {100ms, Ease::Linear}, now);
    app.step(now + 50ms);
    ASSERT(frames == 1 && widget.x.value() == .5);
}
TEST(reduced_motion_stops_spinner_frames_but_preserves_application_timers)
{
    auto spinner = Spinner{};
    auto app = Application{spinner};
    auto theme = Theme::dark();
    theme.motion.enabled = false;
    app.set_theme(theme);
    app.dispatch(esc::Resize{{5, 1}});
    app.step();
    app.paint();
    ASSERT(spinner.is_running());
    ASSERT(app.loop().wait_duration() == std::chrono::milliseconds::max());
    int ticks = 0;
    auto timer = Timer{spinner, 10ms,
                       [&](Spinner&) {
                           ++ticks;
                           return EventResponse{};
                       },
                       true};
    app.step(Tween::Clock::now() + 30ms);
    ASSERT(ticks == 1);
    timer.stop();
    app.paint();
    ASSERT(app.loop().wait_duration() == std::chrono::milliseconds::max());
}
TEST(themed_list_gutter_survives_horizontal_scrolling)
{
    auto list = ListView{{.items = {{"abcdefghijk"}}}};
    auto app = Application{list};
    auto theme = Theme::dark();
    theme.motion.enabled = false;
    app.set_theme(theme);
    app.dispatch(esc::Resize{{6, 1}});
    list.select(0);
    app.step();
    list.set_scroll_offset({3, 0});
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].text == U"╺"));
    ASSERT((app.loop().screen[Point{1, 0}].text == U" "));
    ASSERT((app.loop().screen[Point{2, 0}].text == U"d"));
}
TEST(themed_menu_background_covers_unused_space_with_the_canvas_color)
{
    auto menu = MenuBar{};
    auto app = Application{menu};
    auto theme = Theme::light();
    app.set_theme(theme);
    app.dispatch(esc::Resize{{30, 1}});
    app.paint();
    ASSERT(
        (app.loop().screen[Point{29, 0}].style.background == theme.palette.background));
}
namespace {
struct Backdrop : Widget {
    auto paint(Canvas c) -> Cursor override
    {
        fill(c, U'x' | appearance({}));
        return {};
    }
};
}  // namespace
TEST(dropdowns_paint_a_distinct_solid_surface_and_restore_content_when_closed)
{
    auto check = [](auto& control, Theme theme) {
        auto backdrop = Backdrop{};
        auto root = Column{std::tie(control, backdrop)};
        auto app = Application{root};
        theme.motion.enabled = false;
        app.set_theme(theme);
        app.dispatch(esc::Resize{{30, 10}});
        app.paint();
        control.open();
        app.step();
        app.paint();
        auto bounds = control.popup.child.global_bounds();
        auto at = Point{bounds.at.x, bounds.at.y + bounds.size.height - 1};
        ASSERT((app.loop().screen[at].style.background == theme.palette.surface));
        ASSERT((app.loop().screen[at].style.background != theme.palette.background));
        ASSERT((app.loop().screen[at].text == U" "));
        control.popup.child.style.background = colors::blue;
        app.request_repaint();
        app.paint();
        ASSERT((app.loop().screen[at].style.background == Color{colors::blue}));
        control.popup.hide();
        app.step();
        app.paint();
        ASSERT((app.loop().screen[at].text == U"x"));
    };
    for (auto theme : {Theme::dark(), Theme::light()}) {
        auto select =
            Select{{.items = {{"Dark"}, {"Light"}, {"Terminal"}}, .selected = 0}};
        check(select, theme);
        auto combo = ComboBox{{.items = {{"Dark"}, {"Light"}, {"Terminal"}}}};
        check(combo, theme);
    }
}
TEST(menus_and_submenus_use_the_raised_surface_role)
{
    auto menu = Menu{{{.text = "Child", .children = {{.text = "Leaf"}}}}};
    auto app = Application{menu};
    auto theme = Theme::dark();
    app.set_theme(theme);
    app.dispatch(esc::Resize{{20, 5}});
    app.paint();
    ASSERT((app.loop().screen[Point{0, 0}].style.background == theme.palette.surface));
    menu.select(0);
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    app.step();
    app.paint();
    for (auto const& node : widget_tree(menu))
        if (auto* child = dynamic_cast<Menu*>(node.widget.try_get()))
            ASSERT(child->role == Surface::Raised);
}
