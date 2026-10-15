#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <ox/ox.hpp>
#include <sstream>
using namespace ox;
using namespace std::chrono_literals;

namespace {
// Borrowed composition keeps the example's named controls easy to connect.
class View : public Widget {
   public:
    Widget& child;
    bool fill_available;
    explicit View(Widget& w, bool fill = false) : child{w}, fill_available{fill} {}
    auto get_children() -> zzz::Generator<Widget&> override { co_yield child; }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield child; }
    auto measure(Area a, esc::WidthPolicy p = {}) const -> Area override
    { return fill_available ? Area{} : child.measure(a, p); }

   protected:
    void on_arrange(Rect) override { arrange(child, {{}, size()}); }
};
// Bound long forms and keep actions directly below their fields.
class Compact : public View {
   public:
    int maximum_width;
    Compact(Widget& w, int width) : View{w}, maximum_width{width} {}
    auto measure(Area a, esc::WidthPolicy p = {}) const -> Area override
    {
        a.width = std::min(a.width, maximum_width);
        return child.measure(a, p);
    }

   protected:
    void on_arrange(Rect) override
    {
        auto width = std::min(size().width, maximum_width);
        auto natural = child.measure({width, size().height}, glyph_width_policy());
        arrange(child, {{}, {width, std::min(size().height, natural.height)}});
    }
};
auto section(Widget& content, std::string label)
{
    return Padding{View{content}, std::array{0, 1, 0, 1}} |
           Border{.label = {.text = label.empty() ? "" : " " + label + " ",
                            .align = Align::Start}};
}
// The same controls reflow into one column on a small terminal.
class Responsive : public Widget {
   public:
    Widget& first;
    Widget& second;
    int breakpoint = 88;
    explicit Responsive(Widget& a, Widget& b) : first{a}, second{b} {}
    auto get_children() -> zzz::Generator<Widget&> override
    {
        co_yield first;
        co_yield second;
    }
    auto get_children() const -> zzz::Generator<Widget const&> override
    {
        co_yield first;
        co_yield second;
    }
    auto measure(Area a, esc::WidthPolicy p = {}) const -> Area override
    {
        validate(a);
        auto one = first.measure(a, p), two = second.measure(a, p);
        return a.width >= breakpoint ? Area{a.width, std::max(one.height, two.height)}
                                     : Area{a.width, one.height + two.height + 2};
    }

   protected:
    void on_arrange(Rect) override
    {
        auto width = size().width, height = size().height;
        if (width >= breakpoint) {
            auto left = std::max(0, (width - 4) / 2);
            arrange(first, {{}, {left, height}});
            arrange(second, {{std::min(width, left + 4), 0},
                             {std::max(0, width - left - 4), height}});
        }
        else {
            auto preferred = first.measure(size(), glyph_width_policy()).height;
            auto top = std::min(height, preferred);
            arrange(first, {{}, {width, top}});
            auto y = std::min(height, top + 2);
            arrange(second, {{0, y}, {width, std::max(0, height - y)}});
        }
    }
};
class PaletteView : public Widget {
   public:
    PaletteView() : Widget{FocusPolicy::None, SizePolicy::flex(), SizePolicy::fixed(3)}
    {}
    auto measure(Area a, esc::WidthPolicy = {}) const -> Area override
    {
        validate(a);
        return {30, 3};
    }
    auto paint(Canvas c) -> Cursor override
    {
        fill(c, appearance({}));
        auto const& p = theme().palette;
        std::array<Color, 5> colors{p.foreground, p.muted, p.accent, p.secondary,
                                    p.positive};
        std::array<std::string_view, 5> names{"text", "muted", "accent", "secondary",
                                              "positive"};
        for (int i = 0; i < 5; ++i) {
            auto x = i * c.size().width / 5;
            auto end = (i + 1) * c.size().width / 5;
            auto area = c.subcanvas(
                {x, 0}, {std::max(0, end - x - 1), std::min(1, c.size().height)});
            auto s = appearance({});
            s.foreground = colors[(std::size_t)i];
            fill(area, (theme().ascii ? U'#' : U'▄') | s);
            if (c.size().height > 1)
                put_label(c.subcanvas({x, 1}, {std::max(0, end - x - 1), 1}),
                          names[(std::size_t)i], Align::Start,
                          appearance({}, Surface::Muted));
        }
        return {};
    }
};
class SignalView : public Widget {
   public:
    Animation phase{*this};
    SignalView() : Widget{FocusPolicy::None, SizePolicy::flex(), SizePolicy::fixed(4)}
    {}
    auto measure(Area a, esc::WidthPolicy = {}) const -> Area override
    {
        validate(a);
        return {32, 4};
    }
    auto paint(Canvas c) -> Cursor override
    {
        fill(c, appearance({}));
        auto glyphs = std::u32string_view{U"▁▂▃▄▅▆▇█"};
        for (int x = 0; x < c.size().width; ++x) {
            auto v = (std::sin(x * .22 + phase.value()) +
                      std::sin(x * .09 - phase.value()) + 2) /
                     4;
            auto level = (int)std::lround(v * 7);
            auto glyph =
                theme().ascii ? (v > .6 ? U'#' : U'.') : glyphs[(std::size_t)level];
            auto s = appearance({}, x % 7 == 0 ? Surface::Accent : Surface::Positive);
            c.write({x, std::min(1, std::max(0, c.size().height - 1))},
                    std::u32string_view{&glyph, 1}, s);
        }
        return {};
    }
};
auto title(std::string text) -> Label
{
    return Label{{.text = std::move(text),
                  .align = Align::Start,
                  .style = {.traits = Trait::Bold},
                  .role = Surface::Accent}};
}
auto caption(std::string text) -> Label
{
    return Label{
        {.text = std::move(text), .align = Align::Start, .role = Surface::Muted}};
}
auto panel_height(Widget& w, int rows) { w.height_policy = SizePolicy::fixed(rows); }
auto data_rows() -> std::vector<std::vector<std::string>>
{
    return {{"Button", "Control", "Ready"},
            {"CheckBox", "Control", "Ready"},
            {"RadioGroup", "Selection", "Ready"},
            {"Slider", "Value", "Ready"},
            {"NumericEdit", "Value", "Typed"},
            {"ListView", "Selection", "Multiple"},
            {"DataTable", "Data", "Sortable"},
            {"TreeView", "Data", "Expandable"},
            {"TextBox", "Text", "Unicode"},
            {"ScrollView", "Container", "Clipped"},
            {"Tabs", "Container", "Animated"},
            {"Dialog", "Overlay", "Modal"},
            {"CommandPalette", "Overlay", "Searchable"},
            {"PixelGrid", "Graphics", "Half-block"},
            {"KeyHelp", "Navigation", "Contextual"},
            {"Timer", "Core", "Scheduled"}};
}
void dump(Frame const& frame, bool json)
{
    if (json)
        std::cout << "{\"width\":" << frame.size().width
                  << ",\"height\":" << frame.size().height << ",\"rows\":[";
    for (int y = 0; y < frame.size().height; ++y) {
        if (json) {
            if (y) std::cout << ',';
            std::cout << '[';
        }
        for (int x = 0; x < frame.size().width; ++x) {
            auto const& c = frame[Point{x, y}];
            if (json) {
                if (x) std::cout << ',';
                auto color = [](Color const& color) -> std::string {
                    if (auto* rgb = std::get_if<RGB>(&color))
                        return std::to_string(rgb->red) + "," +
                               std::to_string(rgb->green) + "," +
                               std::to_string(rgb->blue);
                    return "";
                };
                std::cout << "{\"text\":" << std::quoted(esc::utf32_to_utf8(c.text))
                          << ",\"columns\":" << c.columns
                          << ",\"fg\":" << std::quoted(color(c.style.foreground))
                          << ",\"bg\":" << std::quoted(color(c.style.background))
                          << '}';
            }
            else if (c.columns)
                std::cout << esc::utf32_to_utf8(c.text);
        }
        if (json)
            std::cout << ']';
        else
            std::cout << '\n';
    }
    if (json) std::cout << "]}\n";
}
}  // namespace

int main(int argc, char** argv)
{
    bool check = false, snapshot = false, json = false;
    int page = 0, width = 120, height = 38, theme_index = 0;
    for (int i = 1; i < argc; ++i) {
        auto arg = std::string_view{argv[i]};
        if (arg == "--check")
            check = true;
        else if (arg == "--dump")
            snapshot = true;
        else if (arg == "--json") {
            snapshot = true;
            json = true;
        }
        else if (arg == "--page" && i + 1 < argc)
            page = std::stoi(argv[++i]);
        else if (arg == "--width" && i + 1 < argc)
            width = std::stoi(argv[++i]);
        else if (arg == "--height" && i + 1 < argc)
            height = std::stoi(argv[++i]);
        else if (arg == "--light")
            theme_index = 1;
        else if (arg == "--terminal")
            theme_index = 2;
        else {
            std::cerr << "Usage: showcase [--check|--dump|--json] [--page 0..5] "
                         "[--width N] [--height N] [--light|--terminal]\n";
            return 2;
        }
    }
    if (page < 0 || page > 5 || width < 0 || height < 0) return 2;

    auto brand = title("TermOx");
    auto theme_choice = Select{{.items = {{"Dark"}, {"Light"}, {"Terminal"}},
                                .selected = (std::size_t)theme_index}};
    theme_choice.width_policy = SizePolicy::fixed(16);
    auto motion = CheckBox{{.checked = true, .label = {.text = "Motion"}}};
    motion.width_policy = SizePolicy::fixed(13);
    auto ascii = CheckBox{{.checked = theme_index == 2, .label = {.text = "ASCII"}}};
    ascii.width_policy = SizePolicy::fixed(12);
    auto toolbar = Row{std::tie(brand, theme_choice, motion, ascii)};
    panel_height(toolbar, 1);
    auto intro = caption("Widget examples");
    auto status = caption("Ready");
    auto log = LogView{{.max_lines = 200}};
    panel_height(log, 8);

    // Overview: shared motion, live semantic values, composition, and palette roles.
    auto summary =
        TextBox{{.text = "Run the animation, adjust the value, or open a dialog. "
                         "Use the toolbar to change the theme and motion settings.",
                 .read_only = true}};
    summary.focus_policy = FocusPolicy::None;
    panel_height(summary, 3);
    auto run = Button{
        {.label = {.text = "Run sequence"}, .height_policy = SizePolicy::fixed(3)}};
    auto open = Button{
        {.label = {.text = "Open dialog"}, .height_policy = SizePolicy::fixed(3)}};
    auto actions = Row{std::tie(run, open)};
    actions.gap = 2;
    panel_height(actions, 3);
    auto meter = ProgressBar{{.value = 35, .show_percentage = false}};
    auto percent = IntegerLabel{
        {.value = 35, .align = Align::End, .role = Surface::Accent, .suffix = "%"}};
    percent.width_policy = SizePolicy::fixed(5);
    auto meter_row = Row{std::tie(meter, percent)};
    meter_row.gap = 2;
    panel_height(meter_row, 1);
    auto slider = Slider{{.range = {0, 100, 5}, .value = 35}};
    auto number = NumericEdit{
        {.range = {0, 100, .5}, .value = 35, .format = "{:.1f}", .suffix = " %"}};
    number.width_policy = SizePolicy::fixed(12);
    auto tuning = Row{std::tie(slider, number)};
    tuning.gap = 2;
    panel_height(tuning, 1);
    auto spinner = Spinner{{.running = false}};
    auto activity = caption("Idle until activated");
    auto activity_row = Row{std::tie(spinner, activity)};
    activity_row.gap = 2;
    panel_height(activity_row, 1);
    auto left = Column{std::tie(summary, actions, meter_row, tuning, activity_row)};
    left.gap = 1;
    auto palette_heading = title("Theme colors");
    auto palette = PaletteView{};
    auto signal_heading = title("Animation");
    auto signal = SignalView{};
    auto signal_note =
        TextBox{{.text = "Run sequence animates the graph and progress bar. "
                         "Disable Motion to update them without transitions.",
                 .read_only = true}};
    signal_note.focus_policy = FocusPolicy::None;
    panel_height(signal_note, 3);
    auto right =
        Column{std::tie(palette_heading, palette, signal_heading, signal, signal_note)};
    right.gap = 1;
    auto progress_section = section(left, "Progress and values");
    auto theme_section = section(right, "Theme and animation");
    auto overview_content = Responsive{progress_section, theme_section};
    auto overview = ScrollView{View{overview_content}};

    // Typed form, selection controls and validation.
    auto name = LineEdit{{.text = "Ada", .placeholder = "Display name"}};
    auto voices = IntegerEdit{{.range = {1, 128, 1}, .value = 12, .suffix = " voices"}};
    auto gain = NumericEdit{{.range = {0, 1, .05},
                             .value = .35,
                             .format = "{:.2f}",
                             .suffix = " gain",
                             .commit_on_focus_loss = true}};
    auto quality =
        Select{{.items = {{"Draft"}, {"Balanced"}, {"Detailed"}}, .selected = 1}};
    auto tags = ComboBox{{.items = {{"Animation"}, {"Color"}, {"Layout"}, {"Unicode"}},
                          .editor = {.placeholder = "Choose or type"}}};
    auto notify = CheckBox{
        {.checked = true, .label = {.text = "Notifications", .align = Align::Start}}};
    auto toggle = Switch{
        {.checked = true, .label = {.text = "Live preview", .align = Align::Start}}};
    auto mode = RadioGroup{
        {.items = {"Interactive", "Recorded", "Offline"}, .selected_index = 0}};
    panel_height(mode, 3);
    name.width_policy = SizePolicy::max(40);
    voices.width_policy = SizePolicy::fixed(22);
    gain.width_policy = SizePolicy::fixed(22);
    quality.width_policy = SizePolicy::fixed(26);
    tags.width_policy = SizePolicy::max(36);
    auto form = Form{std::tie(name, voices, gain, quality, tags, notify, toggle, mode),
                     std::vector<FormField>{
                         {"Name", "Required",
                          [](Widget const& w) -> std::optional<std::string> {
                              return static_cast<LineEdit const&>(w).text().empty()
                                         ? std::optional<std::string>{"Enter a name"}
                                         : std::nullopt;
                          }},
                         {"Voices", "Whole numbers, 1–128"},
                         {"Gain", "Decimals, 0–1"},
                         {"Quality"},
                         {"Tag"},
                         {"Alerts"},
                         {"Preview"},
                         {"Mode"}}};
    auto submit = Button{{.label = {.text = "Apply settings"}}};
    auto reset = Button{{.label = {.text = "Reset"}}};
    submit.width_policy = SizePolicy::fixed(18);
    reset.width_policy = SizePolicy::fixed(14);
    auto form_actions = Row{std::tie(submit, reset)};
    form_actions.width_policy = SizePolicy::max(34);
    form_actions.gap = 2;
    panel_height(form_actions, 1);
    auto controls_note =
        caption("Enter commits numbers. Ctrl+Enter validates the form.");
    auto controls_content = Column{std::tie(controls_note, form, form_actions)};
    controls_content.gap = 1;
    auto controls_section = section(controls_content, "Form controls");
    auto controls = ScrollView{Compact{controls_section, 78}};

    // Searchable rows, sorting, two-axis scrollbars, tree navigation, and a live log.
    auto search = LineEdit{{.placeholder = "Filter widgets…"}};
    auto items = ListView{{.multiple = true}};
    panel_height(items, 9);
    auto list_bar = ScrollBar{};
    auto list_body = Row{std::tie(items, list_bar)};
    panel_height(list_body, 9);
    auto list_note = caption("Shift: range · Ctrl: toggle");
    auto list_title = title("Collection");
    auto list_panel = Column{std::tie(list_title, search, list_body, list_note)};
    list_panel.gap = 1;
    auto table = DataTable{{.columns = {{"Widget", SizePolicy::min(17)},
                                        {"Family", SizePolicy::min(12)},
                                        {"State", SizePolicy::min(13)}},
                            .rows = data_rows(),
                            .column_lines = false}};
    panel_height(table, 10);
    auto table_bar = ScrollBar{};
    auto table_horizontal = ScrollBar{{.vertical = false}};
    auto table_body = Row{std::tie(table, table_bar)};
    panel_height(table_body, 10);
    auto table_note = caption("Click a heading to sort");
    auto table_title = title("Sample data");
    auto table_panel =
        Column{std::tie(table_title, table_body, table_horizontal, table_note)};
    table_panel.gap = 1;
    auto list_section = Compact{list_panel, std::numeric_limits<int>::max()};
    auto table_section = Compact{table_panel, std::numeric_limits<int>::max()};
    auto tables = Responsive{list_section, table_section};
    tables.height_policy.flexibility = 0;
    auto tree = TreeView{
        {.nodes = {
             {"Widgets",
              true,
              true,
              {{"Label"}, {"Button"}, {"NumericEdit"}, {"Select"}}},
             {"Core", true, true, {{"Frame"}, {"Canvas"}, {"Tween"}, {"Timer"}}}}}};
    panel_height(tree, 8);
    auto tree_title = title("Structure");
    auto tree_panel = Column{std::tie(tree_title, tree)};
    tree_panel.gap = 1;
    auto log_title = title("Activity");
    auto clear = Button{{.label = {.text = "Clear"}}};
    clear.width_policy = SizePolicy::fixed(11);
    auto log_head = Row{std::tie(log_title, clear)};
    panel_height(log_head, 1);
    auto log_panel = Column{std::tie(log_head, log)};
    log_panel.gap = 1;
    auto lower = SplitPane{View{tree_panel}, View{log_panel}, .42};
    auto lower_section = section(lower, "");
    panel_height(lower_section, 12);
    auto data_content = Column{std::tie(tables, lower_section)};
    data_content.gap = 1;
    auto data = ScrollView{View{data_content}};
    link(items, list_bar);
    link(table, table_bar);
    link(table, table_horizontal);

    // Grapheme-safe editing, selection, clipboard, wrapping, and vertical labels.
    auto text_note = caption("Selection, clipboard, undo, and Unicode text");
    auto editor = TextBox{
        {.text =
             "Editable sample text.\n\nTry wide text: 界 "
             "日本語\nCombining text: café\nEmoji clusters: 👩‍💻\n\nSelect "
             "with "
             "Shift+arrows.\nCtrl+A selects all. Ctrl+Z undoes.\nLong lines wrap while "
             "their graphemes stay intact.\n\nResize the terminal and keep editing."}};
    panel_height(editor, 14);
    auto text_bar = ScrollBar{};
    auto text_horizontal = ScrollBar{{.vertical = false}};
    auto text_body = Row{std::tie(editor, text_bar)};
    panel_height(text_body, 14);
    link(editor, text_bar);
    link(editor, text_horizontal);
    auto vertical = Label{{.text = "TERMOX",
                           .vertical = true,
                           .align = Align::Center,
                           .role = Surface::Accent}};
    vertical.width_policy = SizePolicy::fixed(3);
    auto text_with_label = Row{std::tie(vertical, text_body)};
    text_with_label.gap = 2;
    panel_height(text_with_label, 14);
    auto echo =
        LineEdit{{.placeholder = "A single line, with an independent viewport"}};
    auto number_label = FloatLabel{{.value = 12345.678f,
                                    .align = Align::Start,
                                    .role = Surface::Accent,
                                    .format = "{:.2f}",
                                    .prefix = "Total ",
                                    .suffix = " units"}};
    auto text_content = Column{
        std::tie(text_note, text_with_label, text_horizontal, echo, number_label)};
    text_content.gap = 1;
    auto text_section = section(text_content, "Text editing");
    auto text = ScrollView{View{text_section}};

    // Native half-block image rendering inside a clipped two-axis viewport.
    auto image = PixelGrid{{100, 36}};
    auto graphics_scroll = ScrollView{View{image}};
    graphics_scroll.requested_size = Area{100, 18};
    auto graphics_bar = ScrollBar{};
    auto graphics_horizontal = ScrollBar{{.vertical = false}};
    link(graphics_scroll, graphics_bar);
    link(graphics_scroll, graphics_horizontal);
    auto image_body = Row{std::tie(graphics_scroll, graphics_bar)};
    panel_height(image_body, 16);
    auto graphics_note =
        caption("Half-block pixels · clipped canvas · two-axis scrolling");
    auto graphics_content =
        Column{std::tie(graphics_note, image_body, graphics_horizontal)};
    graphics_content.gap = 1;
    auto graphics_section = section(graphics_content, "Character graphics");
    auto graphics = ScrollView{View{graphics_section}};
    for (int y = 0; y < 36; ++y)
        for (int x = 0; x < 100; ++x) {
            auto ripple = (std::sin(x * .13) + std::cos(y * .24) + 2) / 4;
            image.at({x, y}) = RGB{(std::uint8_t)(25 + 190 * ripple),
                                   (std::uint8_t)(70 + 95 * (1 - ripple)),
                                   (std::uint8_t)(60 + 85 * (1 - ripple))};
        }

    auto picker = FilePicker{};
    auto picked = caption("Choose a file to display its path.");
    auto files_content = Column{std::tie(picker, picked)};
    files_content.gap = 1;
    auto files_section = section(files_content, "File picker");
    auto files = ScrollView{View{files_section}};

    auto tabs = Tabs{std::tie(overview, controls, data, text, graphics, files),
                     std::vector<std::string>{"Overview", "Controls", "Data", "Text",
                                              "Graphics", "Files"}};

    // Overlays remain ordinary widgets with modal focus and keyboard routing.
    auto dialog_text =
        TextBox{{.text = "Enter a note, then accept or cancel.", .read_only = true}};
    dialog_text.focus_policy = FocusPolicy::None;
    panel_height(dialog_text, 3);
    auto dialog_input = LineEdit{{.placeholder = "A short note"}};
    auto accept = Button{{.label = {.text = "Accept"}}};
    auto cancel = Button{{.label = {.text = "Cancel"}}};
    auto dialog_actions = Row{std::tie(accept, cancel)};
    dialog_actions.gap = 2;
    panel_height(dialog_actions, 1);
    auto dialog_body = Column{std::tie(dialog_text, dialog_input, dialog_actions)};
    dialog_body.gap = 1;
    auto dialog = Dialog{View{dialog_body}, "Dialog"};
    dialog.requested_size = Area{54, 10};
    auto commands = CommandPalette{};
    auto command_dialog = Dialog{View{commands}, "Commands"};
    command_dialog.requested_size = Area{58, 14};
    auto menu = MenuBar{};
    panel_height(menu, 1);
    auto help = KeyHelp{};
    // The page takes the remaining height; its natural content scrolls inside it.
    auto page_area = View{tabs, true};
    auto root_content = Column{std::tie(toolbar, intro, menu, page_area, status, help,
                                        dialog, command_dialog)};
    auto root = Padding{std::move(root_content), std::array{1, 2, 1, 2}};
    auto app = Application{root};

    auto update_theme = [&] {
        auto index = theme_choice.selected_index().value_or(0);
        auto visual = index == 0 ? Theme::dark()
                                 : (index == 1 ? Theme::light() : Theme::terminal());
        visual.motion.enabled = motion.checked();
        visual.ascii = ascii.checked();
        app.set_theme(visual);
    };
    auto report = [&](std::string message) {
        status.set_text(message);
        log.append(message);
    };
    auto go = [&](std::size_t i) {
        tabs.select(i);
        report("Viewing " + std::string{tabs.labels()[i]});
    };
    theme_choice.on_change.connect([&](auto index) {
        if (index == 2) ascii.set_checked(true);
        update_theme();
    });
    motion.on_change.connect([&](bool) { update_theme(); });
    ascii.on_change.connect([&](bool) { update_theme(); });
    update_theme();
    auto run_sequence = [&] {
        spinner.set_running(true);
        activity.set_text("Working");
        meter.set_value(0);
        signal.phase.reset(0);
        signal.phase.to(6.283185, {1200ms, Ease::InOutCubic});
        report("Sequence started");
    };
    int step = 0;
    Timer* work_clock = nullptr;
    auto timer = Timer{root, 70ms, [&](auto&) -> EventResponse {
                           if (!spinner.is_running()) return {};
                           step = std::min(100, step + 5);
                           meter.set_value(step);
                           percent.set_value(step);
                           if (step == 100) {
                               spinner.set_running(false);
                               activity.set_text("Complete");
                               step = 0;
                               work_clock->stop();
                               report("Sequence complete");
                           }
                           return {.repaint = true};
                       }};
    work_clock = &timer;
    run.on_activate.connect([&] {
        step = 0;
        timer.start();
        run_sequence();
    });
    open.on_activate.connect([&] { dialog.show(); });
    slider.on_change.connect([&](double v) {
        number.set_value(v);
        meter.set_value(v);
        percent.set_value((int)std::lround(v));
    });
    number.on_change.connect([&](double v) {
        slider.set_value(v);
        meter.set_value(v);
        percent.set_value((int)std::lround(v));
    });
    submit.on_activate.connect([&] { form.submit(); });
    reset.on_activate.connect([&] {
        name.set_text("Ada");
        voices.set_value(12);
        gain.set_value(.35);
        quality.select(1);
        tags.set_text("");
        notify.set_checked(true);
        toggle.set_checked(true);
        mode.select(0);
        report("Settings reset");
    });
    form.on_submit.connect([&] {
        report("Settings applied: " + name.text() + ", " +
               std::to_string(voices.value()) + " voices");
    });
    form.on_invalid.connect(
        [&](auto const&) { report("Review the highlighted field"); });
    std::vector<ListItem> all_items;
    for (auto const& row : data_rows())
        all_items.push_back({row[0]});
    items.set_items(all_items);
    search.on_change.connect([&](std::string const& query) {
        auto list = std::vector<ListItem>{};
        auto rows = std::vector<std::vector<std::string>>{};
        for (auto const& row : data_rows())
            if (command_score(query, row[0])) {
                list.push_back({row[0]});
                rows.push_back(row);
            }
        items.set_items(std::move(list));
        table.set_rows(std::move(rows));
    });
    items.on_activate.connect(
        [&](std::size_t i) { report("Activated " + items.items()[i].text); });
    bool ascending = true;
    table.on_header.connect([&](std::size_t column) {
        table.sort_rows(column, ascending);
        ascending = !ascending;
    });
    tree.on_activate.connect([&](auto const& path) {
        report("Tree path depth " + std::to_string(path.size()));
    });
    clear.on_activate.connect([&] {
        log.clear();
        status.set_text("Activity cleared");
    });
    picker.on_choose.connect([&](auto const&) {
        auto safe = picker.location.text() + "/" + picker.filename.text();
        picked.set_text(safe);
        report("Selected " + safe);
    });
    picker.on_error.connect(
        [&](std::string const&) { report("Could not read that directory"); });
    accept.on_activate.connect([&] { dialog.accept(); });
    cancel.on_activate.connect([&] { dialog.reject(); });
    dialog.on_result.connect([&](auto result) {
        report(result == DialogResult::Accepted ? "Dialog accepted"
                                                : "Dialog cancelled");
    });
    auto close_commands = [&] { command_dialog.hide(); };
    commands.set_commands({{"Run animation sequence", "motion progress", true,
                            [&] {
                                close_commands();
                                go(0);
                                step = 0;
                                timer.start();
                                run_sequence();
                            }},
                           {"Open dialog", "modal popup", true,
                            [&] {
                                close_commands();
                                dialog.show();
                            }},
                           {"Dark theme", "appearance", true,
                            [&] {
                                close_commands();
                                theme_choice.select(0);
                            }},
                           {"Light theme", "appearance", true,
                            [&] {
                                close_commands();
                                theme_choice.select(1);
                            }},
                           {"Terminal palette", "appearance ascii", true,
                            [&] {
                                close_commands();
                                theme_choice.select(2);
                            }},
                           {"Toggle motion", "accessibility", true,
                            [&] {
                                close_commands();
                                motion.toggle();
                            }},
                           {"Browse files", "filesystem", true,
                            [&] {
                                close_commands();
                                go(5);
                            }},
                           {"Quit", "exit", true, [&] { app.quit(); }}});
    // MenuBar is reassigned before running; tracked connections follow the new object.
    menu = MenuBar{
        {{"Explore",
          {{.text = "Overview", .action = [&] { go(0); }},
           {.text = "Controls", .action = [&] { go(1); }},
           {.text = "Data", .action = [&] { go(2); }},
           {.text = "Files", .action = [&] { go(5); }}}},
         {"Actions",
          {{.text = "Commands",
            .shortcut = "Ctrl+P",
            .action = [&] { command_dialog.show(); }},
           {.text = "Open dialog", .action = [&] { dialog.show(); }},
           {.text = "Theme",
            .children = {
                {.text = "Dark", .action = [&] { theme_choice.select(0); }},
                {.text = "Light", .action = [&] { theme_choice.select(1); }},
                {.text = "Terminal", .action = [&] { theme_choice.select(2); }}}}}}}};
    panel_height(menu, 1);
    help.bindings = {{.keys = {{KeyCode::Tab, {}}}, .description = "focus"},
                     {.keys = {{KeyCode::Tab, {.ctrl = true}}}, .description = "page"},
                     {.keys = {{U'p', {.ctrl = true}}},
                      .description = "commands",
                      .action =
                          [&] {
                              command_dialog.show();
                              return EventResponse{.repaint = true};
                          }},
                     {.keys = {{KeyCode::F1, {}}},
                      .description = "help",
                      .action =
                          [&] {
                              help.expanded = !help.expanded;
                              help.refresh();
                              return EventResponse{.repaint = true};
                          }},
                     {.keys = {{KeyCode::F2, {}}},
                      .description = "theme",
                      .action =
                          [&] {
                              theme_choice.select(
                                  (theme_choice.selected_index().value_or(0) + 1) % 3);
                              return EventResponse{.repaint = true};
                          }},
                     {.keys = {{KeyCode::F3, {}}},
                      .description = "motion",
                      .action =
                          [&] {
                              motion.toggle();
                              return EventResponse{.repaint = true};
                          }},
                     {.keys = {{U'c', {.ctrl = true}}}, .description = "quit"}};
    help.refresh();
    log.append("Welcome to TermOx.");
    tabs.select((std::size_t)page);
    if (check || snapshot) {
        motion.set_checked(false);
        spinner.set_running(false);
        timer.stop();
        update_theme();
        auto render = [&](Area size) {
            app.dispatch(esc::Resize{size});
            app.step(Tween::Clock::now() + 2s);
            app.paint();
        };
        if (check) {
            for (auto size :
                 {Area{120, 38}, Area{80, 24}, Area{40, 16}, Area{1, 1}, Area{0, 0}})
                for (std::size_t i = 0; i < 6; ++i) {
                    tabs.select(i);
                    render(size);
                }
            render({80, 24});
            dialog.show();
            app.step();
            app.paint();
            dialog.reject();
            command_dialog.show();
            app.step();
            app.paint();
            command_dialog.hide();
            voices.editor.set_text("24");
            if (!form.submit() || voices.value() != 24) return 3;
            voices.editor.set_text("2.5");
            if (form.submit()) return 4;
            voices.revert();
            search.set_text("Text");
            if (table.rows().size() != 1) return 5;
            search.set_text("");
            for (std::size_t i = 0; i < 3; ++i) {
                theme_choice.select(i);
                render({80, 24});
            }
            tabs.select(0);
            render({80, 24});
            run.activate();
            auto now = Tween::Clock::now();
            for (int i = 1; i <= 21; ++i) {
                app.step(now + i * 70ms);
                app.paint();
            }
            if (timer.is_running() || spinner.is_running() || meter.value() != 100)
                return 6;
            std::cout << "Showcase: all six pages, five sizes, themes, overlays, "
                         "search and typed forms passed.\n";
            return 0;
        }
        render({width, height});
        dump(app.loop().screen, json);
        return 0;
    }
    return app.run({.mouse_mode = MouseMode::Move});
}
