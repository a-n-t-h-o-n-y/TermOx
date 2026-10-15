#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <format>
#include <iostream>
#include <iterator>
#include <numeric>
#include <ox/ox.hpp>
#include <string_view>
using namespace ox;
using namespace std::chrono_literals;

namespace {
using Clock = std::chrono::steady_clock;
constexpr auto names = std::array{"Logs",   "Tree",   "Table",  "Search",  "Text",
                                  "Pixels", "Layout", "Frames", "Dispatch"};
constexpr auto frame_modes =
    std::array{"unchanged", "sparse", "full-color", "Unicode-scroll"};
constexpr auto descriptions = std::array{
    "Bounded Unicode log: bulk append, eviction, selection, tail scrolling and "
    "measurement",
    "Wide tree: binary path lookup, selection and periodic collapse/reopen; arrows "
    "navigate",
    "Large table: cell mutation, selection, scrolling and periodic stable sorting",
    "Command search: ranked / equivalent / empty matches; pause to type your own query",
    "Unicode text: a very wide line, multiline wrapping, insert/undo; pause to edit or "
    "paste",
    "Large RGB image: native clipped painting and moving viewport; wheel to pan when "
    "paused",
    "Nested rows of buttons: geometry, focus traversal, clipping, signals and mutation",
    "Frame modes: unchanged / sparse / full color / Unicode scroll; Alt-M cycles modes",
    "Signal fanout, posted events, scheduled timers and shared-clock animations; "
    "counters below"};
struct Options {
    int size = 10000, widgets = 1000, pixels = 2048, batch = 8, interval = 33;
    int ticks = 30, page = 0;
    bool check = false, benchmark = false, all = false;
};
auto number(std::string_view s, int low, int high) -> int
{
    int n;
    auto [end, error] = std::from_chars(s.data(), s.data() + s.size(), n);
    if (error != std::errc{} || end != s.data() + s.size() || n < low || n > high)
        throw std::invalid_argument{"Numeric option out of range"};
    return n;
}
auto elapsed(Clock::time_point begin) -> double
{ return std::chrono::duration<double, std::micro>(Clock::now() - begin).count(); }
struct Samples {
    std::size_t count = 0;
    double last = 0, total = 0, peak = 0;
    void add(double us)
    {
        ++count;
        last = us;
        total += us;
        peak = std::max(peak, us);
    }
    auto mean() const -> double { return count ? total / count : 0; }
};
// Only the stress-specific drawing is custom; all other pages compose library widgets.
class FrameField : public Widget {
   public:
    int mode = 1;
    std::size_t phase = 0;
    auto paint(Canvas canvas) -> Cursor override
    {
        fill(canvas, U' ' | appearance({}));
        for (int y = 0; y < canvas.size().height; ++y) {
            for (int x = 0; x < canvas.size().width; ++x) {
                auto value = (unsigned)(x * 17 + y * 31 + (mode == 2 ? phase : 0));
                auto style = Style{.foreground = RGB{value * 65793u & 0xffffffu}};
                if (mode == 3) {
                    if (x % 12 == 0) {
                        auto at = (y + phase) % 3;
                        put_label(
                            canvas.subcanvas(
                                {x, y}, {std::min(12, canvas.size().width - x), 1}),
                            at == 0   ? "界 é 👩‍💻"
                            : at == 1 ? "row 🇺🇸"
                                      : "scroll →",
                            Align::Start, style);
                    }
                }
                else {
                    auto c =
                        mode == 1 && (std::size_t)(y * canvas.size().width + x) ==
                                         phase % std::max(1, canvas.size().width *
                                                                 canvas.size().height)
                            ? U'@'
                            : U'.';
                    canvas.write({x, y}, std::u32string_view{&c, 1}, style);
                }
            }
        }
        return {};
    }
};
void require(bool condition, std::string_view message)
{
    if (!condition) throw std::runtime_error{std::string{message}};
}
void usage()
{
    std::cout << "TermOx Stress Lab\n"
                 "  --size N       log lines, tree leaves, table rows, commands, "
                 "wide-line units (100..1000000)\n"
                 "  --widgets N    buttons and signal slots / timers (10..10000)\n"
                 "  --pixels N     square RGB image side (16..8192)\n"
                 "  --batch N      work per tick (1..1024); Alt +/- changes this live\n"
                 "  --interval N   tick period in ms (1..1000), default 33\n"
                 "  --page N       initial page 1..9; --all runs hidden workloads too\n"
                 "  --check        deterministic headless correctness/resize checks\n"
                 "  --benchmark    headless per-page CSV, no terminal I/O\n"
                 "  --ticks N      benchmark samples per page (1..10000)\n"
                 "Live: Alt 1..9 pages, Alt P pause, Alt A all, Alt B burst, Alt M "
                 "frame mode,\n"
                 "      Alt W wrapping, Tab focus, Ctrl-Tab next page, Ctrl-C exit.\n";
}
}  // namespace

int main(int argc, char** argv)
try {
    auto options = Options{};
    for (int i = 1; i < argc; ++i) {
        auto arg = std::string_view{argv[i]};
        if (arg == "--help") {
            usage();
            return 0;
        }
        if (arg == "--check")
            options.check = true;
        else if (arg == "--benchmark")
            options.benchmark = true;
        else if (arg == "--all")
            options.all = true;
        else {
            if (i + 1 == argc) throw std::invalid_argument{"Missing option value"};
            auto value = std::string_view{argv[++i]};
            if (arg == "--size")
                options.size = number(value, 100, 1000000);
            else if (arg == "--widgets")
                options.widgets = number(value, 10, 10000);
            else if (arg == "--pixels")
                options.pixels = number(value, 16, 8192);
            else if (arg == "--batch")
                options.batch = number(value, 1, 1024);
            else if (arg == "--interval")
                options.interval = number(value, 1, 1000);
            else if (arg == "--ticks")
                options.ticks = number(value, 1, 10000);
            else if (arg == "--page")
                options.page = number(value, 1, 9) - 1;
            else
                throw std::invalid_argument{"Unknown option"};
        }
    }
    if (options.check && options.benchmark)
        throw std::invalid_argument{"Choose --check or --benchmark"};
    auto const headless = options.check || options.benchmark;
    auto log = LogView{
        {.max_lines = (std::size_t)options.size, .editor = {.wrap = TextWrap::None}}};
    auto initial = std::string{};
    auto nodes = std::vector<TreeNode>{};
    auto rows = std::vector<std::vector<std::string>>{};
    auto commands = std::vector<std::shared_ptr<Command>>{};
    for (int i = 0; i < options.size; ++i) {
        auto index = std::to_string(i);
        initial += "Record " + index + ": worker received status 界 é 👩‍💻\n";
        nodes.push_back({"Branch " + index, true, false, {{"Leaf 界"}, {"Leaf é"}}});
        rows.push_back({index, "Worker " + index, "Ready 界", "123456.78"});
        commands.push_back(command(
            {.text = "Command " + index + " 界",
             .keywords = i % 2 ? "open workspace file" : "close workspace view"}));
    }
    log.append(initial);
    auto tree =
        TreeView{{.nodes = {{"Root", true, true, std::move(nodes)}}, .multiple = true}};
    auto table = DataTable{{.columns = {{"ID", SizePolicy::fixed(10)},
                                        {"Worker", SizePolicy::fixed(22)},
                                        {"State", SizePolicy::fixed(20)},
                                        {"Value", SizePolicy::fixed(18), Align::Right}},
                            .rows = std::move(rows)}};
    auto palette = CommandPalette{std::move(commands)};
    auto wide = std::string{};
    for (int i = 0; i < options.size; ++i)
        wide += i % 7 ? "a" : "界é";
    auto text = TextBox{{.text = wide + "\n" + initial, .wrap = TextWrap::None}};
    auto pixels = ScrollView{PixelGrid{{options.pixels, options.pixels}}};
    for (int y = 0; y < options.pixels; ++y)
        for (int x = 0; x < options.pixels; ++x)
            pixels.child.at({x, y}) = RGB{
                (unsigned)((x * 13 & 255) << 16 | (y * 7 & 255) << 8 | (x + y) % 256)};
    using ButtonRow = Row<std::vector<Button>>;
    auto button_rows = std::vector<ButtonRow>{};
    for (int begin = 0; begin < options.widgets; begin += 8) {
        auto buttons = std::vector<Button>{};
        for (int i = begin; i < std::min(begin + 8, options.widgets); ++i)
            buttons.emplace_back(
                Button::Options{.label = {.text = "Action " + std::to_string(i)}});
        button_rows.emplace_back(std::move(buttons));
        button_rows.back().height_policy = SizePolicy::fixed(2);
    }
    auto layout = ScrollView{Column{std::move(button_rows)}};
    auto field = FrameField{};
    auto dispatch_info =
        TextBox{{.text = "Signal listeners and tracked timers are ready.\n",
                 .read_only = true}};
    auto tabs = Tabs{
        std::tie(log, tree, table, palette, text, pixels, layout, field, dispatch_info),
        std::vector<std::string>{names.begin(), names.end()}};
    auto heading = Label{{.text = "TermOx Stress Lab",
                          .align = Align::Start,
                          .style = {.traits = Trait::Bold}}};
    auto controls = Label{{.text = "Alt 1..9 pages | Alt P pause | Alt +/- load | Alt "
                                   "A all | Alt B burst | Alt M frame | Alt W wrap",
                           .align = Align::Start}};
    auto detail = Label{{.align = Align::Start}};
    auto status = Label{{.align = Align::Start}};
    auto totals = Label{{.align = Align::Start}};
    auto root = Column{std::tie(heading, controls, detail, tabs, status, totals)};
    auto app = Application{root};
    auto signal = sl::Signal<void(std::size_t)>{};
    std::size_t signals = 0, events = 0, timers = 0, activations = 0, tick = 0,
                records = options.size;
    for (int i = 0; i < options.widgets; ++i)
        signal.connect([&](std::size_t n) { signals += n; });
    for (auto& row : layout.child.children)
        for (auto& button : row.children)
            button.on_activate.connect([&] { ++activations; });
    auto clocks = std::vector<Timer>{};
    for (int i = 0; i < options.widgets; ++i)
        clocks.emplace_back(root, 1000ms, [&](auto&) -> EventResponse {
            ++timers;
            return {};
        });
    auto animations = std::vector<Animation>{};
    auto animation_frames = std::size_t{0};
    for (int i = 0; i < options.widgets; ++i) {
        animations.emplace_back(root);
        animations.back().on_frame([&](double) {
            ++animation_frames;
            return EventResponse{};
        });
    }
    auto clock_running = false;
    auto stats = std::array<Samples, names.size()>{};
    auto paused = false;
    auto run_all = options.all;
    auto last_tick = Clock::now();
    auto gap = 0.0;
    auto late = std::size_t{0};
    auto animation_time = Clock::now();
    auto jobs = std::array<std::function<void()>, names.size()>{
        [&] {
            auto batch = std::string{};
            for (int i = 0; i < options.batch; ++i)
                batch += std::format("Record {}: streamed status 界 é 👩‍💻\n",
                                     records++);
            log.append(batch);
        },
        [&] {
            for (int i = 0; i < options.batch; ++i)
                tree.select(
                    {0, (tick * options.batch + i) % (std::size_t)options.size});
            if (tick % 30 == 0) {
                tree.set_expanded({0}, false);
                tree.set_expanded({0}, true);
            }
        },
        [&] {
            auto row = tick % (std::size_t)options.size;
            table.set_cell(row, 2, "Tick " + std::to_string(tick));
            table.select(row);
            if (tick % 60 == 0) table.sort_rows(1, (tick / 60) % 2 == 0);
        },
        [&] {
            auto queries = std::array{"workspace", "command 1", "command 2",
                                      "zzzz",      "open",      "close"};
            for (int i = 0; i < options.batch; ++i)
                palette.query.set_text(queries[(tick + i) % queries.size()]);
        },
        [&] {
            // Manual editing while paused may shorten the original wide line.
            auto boundaries = text.editor().boundaries();
            auto caret =
                std::upper_bound(boundaries.begin(), boundaries.end(), wide.size());
            text.set_caret(*std::prev(caret));
            for (int i = 0; i < options.batch; ++i) {
                text.insert("界é");
                text.undo();
            }
        },
        [&] {
            auto x = (int)(tick * options.batch % (std::size_t)options.pixels);
            auto y = (int)(tick % (std::size_t)options.pixels);
            pixels.child.set_pixel({x, y}, RGB{(unsigned)(tick * 7919 & 0xffffffu)});
            pixels.set_scroll_offset({x, y / 2});
        },
        [&] {
            auto& rows = layout.child.children;
            auto row = tick % rows.size();
            rows[row].gap = (int)(tick % 3);
            rows[row].refresh();
            rows[row].children[0].activate();
            layout.set_scroll_offset({0, (int)row * 2});
            app.dispatch(KeyEvent{.key = KeyCode::Tab});
        },
        [&] {
            ++field.phase;
            app.request_repaint();
        },
        [&] {
            for (int i = 0; i < options.batch; ++i) {
                signal(1);
                for (int n = 0; n < 16; ++n)
                    app.post(event::Custom{[&] {
                        ++events;
                        return EventResponse{};
                    }});
            }
            for (auto& animation : animations)
                if (!animation.active())
                    animation.to(1 - animation.target(), {1000ms, Ease::InOutCubic},
                                 headless ? animation_time : Clock::now());
            dispatch_info.set_text(std::format(
                "{} signal slots; {} timers at 1 s\nSignal deliveries: {}\nPosted "
                "events delivered: {}\nTimer firings: {}\nButton activations: "
                "{}\nAnimation frames: {}\n",
                options.widgets, options.widgets, signals, events, timers, activations,
                animation_frames));
        }};
    auto perform = [&](std::size_t page) {
        auto begin = Clock::now();
        jobs[page]();
        stats[page].add(elapsed(begin));
    };
    auto update_info = [&] {
        auto page = tabs.selected_index().value_or(0);
        detail.set_text(page == 7 ? std::string{descriptions[page]} + " [" +
                                        frame_modes[(std::size_t)field.mode] + "]"
                                  : descriptions[page]);
        auto const& s = stats[page];
        status.set_text(std::format(
            "{} | {} | batch {} | work last/mean/peak {:.2f}/{:.2f}/{:.2f} ms",
            paused ? "PAUSED" : "RUNNING", run_all ? "ALL" : "ACTIVE PAGE",
            options.batch, s.last / 1000, s.mean() / 1000, s.peak / 1000));
        totals.set_text(
            std::format("{} records | {} widgets | {}x{} pixels | tick gap {:.1f} ms / "
                        "{} ms target | late {}",
                        options.size, options.widgets, options.pixels, options.pixels,
                        gap, options.interval, late));
    };
    auto run_tick = [&] {
        auto now = Clock::now();
        gap = elapsed(last_tick) / 1000;
        last_tick = now;
        if (gap > options.interval * 1.5) ++late;
        if (!paused) {
            ++tick;
            auto page = tabs.selected_index().value_or(0);
            auto core = run_all || page == 8;
            if (core != clock_running) {
                for (auto& timer : clocks) {
                    if (core)
                        timer.start();
                    else
                        timer.stop();
                }
                if (!core)
                    for (auto& animation : animations)
                        animation.reset(0);
                clock_running = core;
            }
            if (run_all)
                for (std::size_t i = 0; i < jobs.size(); ++i)
                    perform(i);
            else
                perform(page);
            update_info();
        }
    };
    auto timer = Timer{root, std::chrono::milliseconds{options.interval},
                       [&](auto&) -> EventResponse {
                           run_tick();
                           return {.repaint = !paused};
                       }};
    tabs.select((std::size_t)options.page);
    tabs.on_select.connect([&](auto) { update_info(); });
    root.shortcuts = [&](KeyEvent const& key) -> EventResponse {
        if (key.action == KeyAction::Release) return {};
        for (int i = 0; i < 9; ++i)
            if (key.matches((char32_t)(U'1' + i), {.alt = true}, key.action)) {
                tabs.select((std::size_t)i);
                return {.handled = true, .repaint = true};
            }
        auto matches = [&](char32_t c) {
            return key.matches(c, {.alt = true}, key.action);
        };
        if (matches(U'p')) {
            paused = !paused;
            if (paused) {
                for (auto& clock : clocks)
                    clock.stop();
                for (auto& animation : animations)
                    animation.reset(0);
                clock_running = false;
            }
        }
        else if (matches(U'a'))
            run_all = !run_all;
        else if (matches(U'+') || matches(U'='))
            options.batch = std::min(1024, options.batch * 2);
        else if (matches(U'-'))
            options.batch = std::max(1, options.batch / 2);
        else if (matches(U'm'))
            field.mode = (field.mode + 1) % 4;
        else if (matches(U'w')) {
            text.wrap = text.wrap == TextWrap::None ? TextWrap::Word : TextWrap::None;
            text.refresh();
        }
        else if (matches(U'b')) {
            ++tick;
            for (std::size_t i = 0; i < jobs.size(); ++i)
                perform(i);
        }
        else
            return {};
        update_info();
        return {.handled = true, .repaint = true};
    };
    update_info();
    if (!headless) {
        timer.start();
        return app.run({.mouse_mode = MouseMode::Move});
    }

    auto profile =
        esc::resolve_profile({"xterm-256color", "truecolor", false}, true).profile;
    auto renderer = Renderer{profile};
    app.dispatch(esc::Resize{{120, 38}});
    if (options.benchmark)
        std::cout << "page,samples,update_us,step_us,paint_us,encode_us,bytes_per_"
                     "frame,cached_encode_us,cached_bytes\n";
    for (std::size_t scenario = 0; scenario < 12; ++scenario) {
        auto page = scenario < 7 ? scenario : scenario < 11 ? 7u : 8u;
        if (page == 7) field.mode = (int)scenario - 7;
        auto name = page == 7
                        ? std::string{"Frames-"} + frame_modes[(std::size_t)field.mode]
                        : std::string{names[page]};
        tabs.select(page);
        app.step();
        app.paint();
        renderer.invalidate();
        auto update = Samples{}, step = Samples{}, paint = Samples{},
             encode = Samples{}, cached = Samples{};
        auto bytes = std::size_t{0};
        auto now = Clock::now();
        for (auto& clock : clocks) {
            if (page == 8 || run_all)
                clock.start(now);
            else
                clock.stop();
        }
        if (options.check && page == 1) tick = 29;
        if (options.check && page == 2) tick = 59;
        auto count = options.check ? 3 : options.ticks;
        for (int i = -2; i < count; ++i) {
            ++tick;
            animation_time = now;
            auto begin = Clock::now();
            if (run_all)
                for (std::size_t j = 0; j < jobs.size(); ++j)
                    perform(j);
            else
                perform(page);
            auto work_us = elapsed(begin);
            begin = Clock::now();
            now += std::chrono::milliseconds{options.interval};
            app.step(now);
            auto step_us = elapsed(begin);
            app.request_repaint();
            begin = Clock::now();
            app.paint();
            auto paint_us = elapsed(begin);
            begin = Clock::now();
            auto output_size =
                renderer.render(app.loop().screen, app.loop().cursor).size();
            auto encode_us = elapsed(begin);
            begin = Clock::now();
            auto unchanged =
                renderer.render(app.loop().screen, app.loop().cursor).size();
            auto cached_us = elapsed(begin);
            require(unchanged == 0, "Unchanged frame emitted output");
            if (i >= 0) {
                update.add(work_us);
                step.add(step_us);
                paint.add(paint_us);
                encode.add(encode_us);
                cached.add(cached_us);
                bytes += output_size;
            }
        }
        if (options.benchmark)
            std::cout << std::format(
                "{},{},{:.3f},{:.3f},{:.3f},{:.3f},{:.1f},{:.3f},0\n", name, count,
                update.mean(), step.mean(), paint.mean(), encode.mean(),
                (double)bytes / count, cached.mean());
        if (options.check) {
            for (auto size :
                 {Area{0, 0}, Area{1, 1}, Area{40, 12}, Area{160, 50}, Area{120, 38}}) {
                app.dispatch(esc::Resize{size});
                app.step(now);
                app.paint();
                renderer.render(app.loop().screen, app.loop().cursor);
                require(renderer.render(app.loop().screen, app.loop().cursor).empty(),
                        "Resize frame was not stable");
            }
        }
    }
    require(log.lines().size() == (std::size_t)options.size, "Log capacity");
    require(tree.nodes()[0].children.size() == (std::size_t)options.size, "Tree size");
    require(table.rows().size() == (std::size_t)options.size, "Table size");
    require(text.text() == wide + "\n" + initial, "Text undo did not restore document");
    require(events > 0 && signals > 0 && activations > 0, "Workloads did not run");
    app.step(Clock::now() + 10s);
    require(timers >= (std::size_t)options.widgets, "Tracked timers did not fire");
    require(animation_frames > 0, "Animations did not advance");
    if (options.check)
        std::cout << "Stress Lab: nine workloads, Unicode, capacity/undo, posted "
                     "events, timers, stable frames and five resize sizes passed.\n";
    return 0;
}
catch (std::exception const& error) {
    std::cerr << "stress: " << error.what() << '\n';
    return 1;
}
