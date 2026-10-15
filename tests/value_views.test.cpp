#include <cmath>
#include <fstream>
#include <memory>
#include <ox/ox.hpp>
#include <thread>
#include <zzz/test.hpp>
using namespace ox;
TEST(value_range_validation_and_extreme_fractions)
{
    ASSERT_THROWS(validate(ValueRange{10, 0, 1}), std::invalid_argument);
    ASSERT_THROWS(validate(ValueRange{0, 10, 0}), std::invalid_argument);
    ASSERT_THROWS(clamp_value(INFINITY, {}), std::invalid_argument);
    auto m = std::numeric_limits<double>::max();
    ASSERT(value_fraction(0, {-m, m, 1}) == .5);
    ASSERT(value_fraction(1, {1, 1, 1}) == 1);
    ASSERT(clamp_value(-1, {0, 10, 1}) == 0);
}
TEST(slider_keyboard_pointer_capture_and_disabled_state)
{
    auto slider = Slider{{.range = {0, 10, 2}, .value = 4}};
    auto app = Application{slider};
    app.dispatch(esc::Resize{{11, 1}});
    int changes = 0;
    slider.on_change.connect([&](double) { ++changes; });
    app.set_focus(slider);
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    ASSERT(slider.value() == 6);
    app.dispatch(MousePress{.at = {0, 0}, .button = MouseButton::Left});
    ASSERT(slider.value() == 0 && slider.is_pointer_captured());
    app.dispatch(MouseMove{.at = {50, 0}, .held_button = {}});
    ASSERT(slider.value() == 10);
    app.dispatch(MouseRelease{.at = {50, 0}, .button = MouseButton::Left});
    ASSERT(!slider.is_pointer_captured());
    slider.enabled = false;
    app.step();
    ASSERT(!slider.handle(KeyEvent{.key = KeyCode::Home}).handled);
    ASSERT(slider.value() == 10 && changes == 3);
    auto vertical = Slider{{.range = {0, 10, 1}, .vertical = true}};
    arrange(vertical, {{}, {1, 11}});
    vertical.handle(MousePress{.at = {0, 0}, .button = MouseButton::Left});
    ASSERT(vertical.value() == 10);
}
TEST(numeric_edit_staging_validation_step_and_revert)
{
    auto number = NumericEdit{{.range = {-5, 5, .5}, .value = 1}};
    auto app = Application{number};
    app.dispatch(esc::Resize{{12, 1}});
    app.set_focus(number.editor);
    int changes = 0, invalid = 0;
    number.on_change.connect([&](double) { ++changes; });
    number.on_invalid.connect([&](auto const&) { ++invalid; });
    number.editor.set_text("-");
    ASSERT(number.value() == 1);
    ASSERT(!number.commit());
    ASSERT(invalid == 1 && number.editor.text() == "-");
    number.editor.set_text("2.5");
    app.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(number.value() == 2.5 && changes == 1);
    app.dispatch(KeyEvent{.key = KeyCode::ArrowUp});
    ASSERT(number.value() == 3 && changes == 2);
    number.editor.set_text("bad");
    app.dispatch(KeyEvent{.key = KeyCode::Escape});
    ASSERT(number.editor.text() == "3");
    number.set_range({-1, 1, .1});
    ASSERT(number.value() == 1 && changes == 3);
    number.editor.set_text("nan");
    ASSERT(!number.commit());
    number.editor.set_text("6");
    ASSERT(!number.commit());
}
TEST(progress_and_spinner_paint_and_lifecycle)
{
    auto progress =
        ProgressBar{{.range = {0, 10, 1}, .value = 5, .show_percentage = false}};
    auto frame = Frame{{10, 1}};
    progress.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"█"));
    ASSERT((frame[Point{9, 0}].text == U"░"));
    auto spinner = Spinner{{.frames = {"a", "b"}, .running = false}};
    ASSERT(!spinner.tick().repaint);
    spinner.set_running(true);
    ASSERT(spinner.tick().repaint);
    auto one = Frame{{1, 1}};
    spinner.paint(Canvas{one});
    ASSERT((one[Point{0, 0}].text == U"b"));
    auto moved = std::move(spinner);
    moved.set_running(false);
    ASSERT(!moved.is_running());
    ASSERT(!moved.tick().repaint);
    auto toggle = Switch{};
    int calls = 0;
    toggle.on_change.connect([&](bool) { ++calls; });
    toggle.handle(KeyEvent{.key = U' '});
    ASSERT(toggle.checked() && calls == 1);
    toggle.set_checked(true);
    ASSERT(calls == 1);
}
TEST(value_notifications_may_destroy_or_move_widgets)
{
    auto number = std::make_unique<NumericEdit>();
    number->editor.on_change.connect([&](auto const&) { number.reset(); });
    number->set_value(4);
    ASSERT(!number);
    auto slider = std::make_unique<Slider>();
    slider->on_change.connect([&](double) { slider.reset(); });
    slider->set_value(3);
    ASSERT(!slider);
    auto progress = std::make_unique<ProgressBar>();
    progress->on_change.connect([&](double) { progress.reset(); });
    progress->set_value(3);
    ASSERT(!progress);
}
TEST(tree_expansion_navigation_selection_and_disabled_descendants)
{
    auto tree = TreeView{{.nodes = {{"root", true, false, {{"one"}, {"two"}}},
                                    {"disabled", false, true, {{"child"}}}}}};
    auto app = Application{tree};
    app.dispatch(esc::Resize{{20, 5}});
    for (auto& list : tree.get_children()) {
        app.set_focus(list);
        break;
    }
    tree.select({0});
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    ASSERT(tree.nodes()[0].expanded);
    ASSERT(tree.list().items().size() == 5);
    app.dispatch(KeyEvent{.key = KeyCode::ArrowRight});
    ASSERT((tree.current_path() == std::optional{std::vector<std::size_t>{0, 0}}));
    app.dispatch(KeyEvent{.key = KeyCode::ArrowLeft});
    ASSERT(tree.current_path() == std::optional{std::vector<std::size_t>{0}});
    tree.select({0, 1});
    tree.set_expanded({0}, false);
    ASSERT(tree.current_path() == std::optional{std::vector<std::size_t>{0}});
    ASSERT(!tree.select({1, 0}));
    ASSERT(tree.select({0, 1}));
    ASSERT(tree.nodes()[0].expanded);
    ASSERT_THROWS(tree.select({9}), std::out_of_range);
}
TEST(tree_multiple_selection_survives_unrelated_expansion)
{
    auto tree = TreeView{
        {.nodes = {{"one", true, false, {{"child"}}}, {"two"}}, .multiple = true}};
    tree.select({0});
    tree.select({1}, SelectionAction::Toggle);
    ASSERT(tree.selected_paths().size() == 2);
    tree.set_expanded({0}, true);
    ASSERT((tree.selected_paths() == std::vector<std::vector<std::size_t>>{{0}, {1}}));
    auto moved = std::move(tree);
    int calls = 0;
    moved.on_expand.connect([&](auto const&, bool) { ++calls; });
    moved.set_expanded({0}, false);
    ASSERT(calls == 1);
}
TEST(log_bounded_records_readonly_and_following)
{
    auto log = LogView{{.max_lines = 3, .editor = {.wrap = TextWrap::None}}};
    arrange(log, {{}, {12, 2}});
    log.append("one\ntwo");
    log.append("three\nfour\n");
    ASSERT(log.lines().size() == 3);
    ASSERT(log.editor.text() == "two\nthree\nfour\n");
    ASSERT(log.editor.read_only);
    ASSERT(!log.editor.editor().can_undo());
    log.set_follow_tail(false);
    log.editor.set_scroll_offset({0, 0});
    log.append("five");
    ASSERT(log.editor.scroll_offset().y == 0 && !log.follows_tail());
    log.set_follow_tail(true);
    ASSERT(log.editor.scroll_offset().y > 0);
    log.set_max_lines(1);
    ASSERT(log.editor.text() == "five\n");
    log.clear();
    ASSERT(log.lines().empty() && log.editor.text().empty());
    ASSERT_THROWS(log.set_max_lines(0), std::invalid_argument);
    ASSERT_THROWS(log.append("bad\x1b"), std::invalid_argument);
    ASSERT(log.lines().empty());
}
TEST(log_user_scrolling_pauses_follow_and_preserves_selection)
{
    auto log = LogView{{.max_lines = 10, .editor = {.wrap = TextWrap::None}}};
    arrange(log, {{}, {10, 2}});
    log.append("one\ntwo\nthree\nfour");
    log.editor.set_scroll_offset({0, 0});
    ASSERT(!log.follows_tail());
    log.editor.set_caret(0);
    log.editor.set_caret(3, true);
    log.append("five");
    ASSERT(
        (log.editor.editor().selection() == std::pair<std::size_t, std::size_t>{0, 3}));
    ASSERT(log.editor.scroll_offset().y == 0);
}
TEST(split_layout_drag_resize_and_minimum_constraints)
{
    auto split = SplitPane{Label{"one"}, Label{"two"}, .25};
    auto app = Application{split};
    app.dispatch(esc::Resize{{21, 5}});
    ASSERT(split.first.size().width == 5 && split.second.size().width == 15);
    app.dispatch(MousePress{.at = {5, 2}, .button = MouseButton::Left});
    ASSERT(split.is_pointer_captured());
    app.dispatch(MouseMove{.at = {10, 2}, .held_button = {}});
    ASSERT(split.ratio() == .5);
    app.dispatch(MouseRelease{.at = {10, 2}, .button = MouseButton::Left});
    ASSERT(!split.is_pointer_captured());
    app.dispatch(esc::Resize{{41, 5}});
    ASSERT(split.first.size().width == 20);
    split.first_minimum = 5;
    split.second_minimum = 5;
    split.set_ratio(0);
    ASSERT(split.first.size().width == 5);
    app.dispatch(esc::Resize{{5, 1}});
    ASSERT(split.first.size().width == 2 && split.second.size().width == 2);
    split.vertical = true;
    app.dispatch(esc::Resize{{10, 9}});
    ASSERT(split.first.size().height == 4 && split.second.size().height == 4);
    ASSERT_THROWS(split.set_ratio(NAN), std::invalid_argument);
}
TEST(form_validation_errors_submission_and_layout)
{
    auto form = Form{std::tuple{LineEdit{}, NumericEdit{}},
                     std::vector<FormField>{
                         {"Name", "",
                          [](Widget const& w) -> std::optional<std::string> {
                              return static_cast<LineEdit const&>(w).text().empty()
                                         ? std::optional<std::string>{"Required"}
                                         : std::nullopt;
                          }},
                         {"Amount", "Number"}}};
    arrange(form, {{}, {30, 8}});
    int submitted = 0, invalid = 0;
    form.on_submit.connect([&] { ++submitted; });
    form.on_invalid.connect([&](auto const&) { ++invalid; });
    ASSERT(!form.submit());
    ASSERT(form.errors()[0] == "Required" && invalid == 1);
    std::get<0>(form.children).set_text("Ada");
    ASSERT(form.submit());
    ASSERT(!form.errors()[0] && submitted == 1);
    ASSERT(std::get<0>(form.children).at().x >= 7);
    auto frame = Frame{{30, 8}};
    form.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"N"));
    ASSERT_THROWS(form.set_fields({{"only one"}}), std::invalid_argument);
    ASSERT(form.fields().size() == 2);
}
TEST(command_palette_filter_navigation_disabled_and_action_snapshot)
{
    int invoked = 0;
    auto palette = CommandPalette{
        {command({"Open File", "file", true,
                  [&] {
                      ++invoked;
                      return EventResponse{};
                  }}),
         command({"Close File", "", true,
                  [&] {
                      invoked += 2;
                      return EventResponse{};
                  }}),
         command({"Disabled", "", false, [] { return EventResponse{}; }})}};
    auto app = Application{palette};
    app.dispatch(esc::Resize{{30, 8}});
    app.set_focus(palette.query);
    palette.query.set_text("clf");
    ASSERT(palette.matches().size() == 1 && palette.matches()[0] == 1);
    app.dispatch(KeyEvent{.key = KeyCode::Enter});
    ASSERT(invoked == 2);
    palette.query.set_text("");
    app.dispatch(KeyEvent{.key = KeyCode::ArrowDown});
    ASSERT(palette.choices().current_row() == 1);
    ASSERT(!palette.execute(2));
    ASSERT(command_score("界", "a界") == 1);
    ASSERT(!command_score("z", "abc"));
    palette.on_execute.connect([&](auto) { palette.set_commands({}); });
    palette.execute(0);
    ASSERT(invoked == 3 && palette.commands().empty());
}
namespace {
auto settle(FilePicker& picker) -> FilePickerState
{
    auto lifetime = track(picker);
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds{5};
    while (auto* self = lifetime.try_get()) {
        if (!self->is_loading()) { return self->state(); }
        if (std::chrono::steady_clock::now() > deadline) {
            throw std::runtime_error{"FilePicker timeout"};
        }
        self->poll();
        std::this_thread::sleep_for(std::chrono::milliseconds{1});
    }
    return FilePickerState::Cancelled;
}
struct TempFiles {
    std::filesystem::path path =
        std::filesystem::temp_directory_path() /
        ("termox-picker-" +
         std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()));
    TempFiles()
    {
        std::filesystem::create_directories(path / "sub");
        path = std::filesystem::canonical(path);
        std::ofstream{path / "a.txt"} << "a";
        std::ofstream{path / "b.cpp"} << "b";
        std::ofstream{path / ".hidden"} << "h";
    }
    ~TempFiles()
    {
        std::error_code ec;
        std::filesystem::remove_all(path, ec);
    }
};
}  // namespace
TEST(file_picker_browsing_filter_open_save_and_directory_modes)
{
    auto files = TempFiles{};
    auto picker = FilePicker{{.directory = files.path, .filter = [](auto const& e) {
                                  return e.path().extension() == ".txt";
                              }}};
    ASSERT(settle(picker) == FilePickerState::Ready);
    ASSERT(picker.paths().size() == 2 && picker.paths()[0].filename() == "sub");
    auto chosen = std::filesystem::path{};
    picker.on_choose.connect([&](auto const& p) { chosen = p; });
    picker.filename.set_text("a.txt");
    ASSERT(picker.accept());
    ASSERT(settle(picker) == FilePickerState::Ready);
    ASSERT(chosen == files.path / "a.txt");
    picker.filename.set_text("b.cpp");
    ASSERT(picker.accept());
    ASSERT(settle(picker) == FilePickerState::Error);
    ASSERT(picker.set_directory("sub"));
    ASSERT(settle(picker) == FilePickerState::Ready);
    ASSERT(picker.paths().empty());
    ASSERT(picker.set_directory("missing"));
    ASSERT(settle(picker) == FilePickerState::Error);
    ASSERT(picker.directory() == files.path / "sub");
    picker.mode = FilePickerMode::SaveFile;
    picker.filename.set_text("new.txt");
    ASSERT(picker.accept());
    ASSERT(settle(picker) == FilePickerState::Ready);
    ASSERT(chosen == files.path / "sub/new.txt");
    ASSERT(!std::filesystem::exists(chosen));
    picker.mode = FilePickerMode::Directory;
    ASSERT(picker.accept());
    ASSERT(settle(picker) == FilePickerState::Ready);
    ASSERT(chosen == files.path / "sub");
    picker.show_hidden = true;
    picker.filter = {};
    picker.mode = FilePickerMode::OpenFile;
    picker.set_directory(files.path);
    ASSERT(settle(picker) == FilePickerState::Ready);
    ASSERT(picker.paths().size() == 4);
}
TEST(composite_callbacks_may_destroy_the_owner)
{
    auto log = std::make_unique<LogView>();
    log->editor.on_change.connect([&](auto const&) { log.reset(); });
    log->append("line");
    ASSERT(!log);
    auto tree = std::make_unique<TreeView>(
        TreeView::Options{.nodes = {{"root", true, false, {{"child"}}}}});
    tree->on_expand.connect([&](auto const&, bool) { tree.reset(); });
    tree->set_expanded({0}, true);
    ASSERT(!tree);
    auto palette = std::make_unique<CommandPalette>(
        std::vector<std::shared_ptr<Command>>{command({"Command"})});
    palette->on_execute.connect([&](auto) { palette.reset(); });
    palette->execute(0);
    ASSERT(!palette);
    auto files = TempFiles{};
    auto picker = std::make_unique<FilePicker>(FilePicker::Options{
        .directory = files.path, .mode = FilePickerMode::Directory});
    ASSERT(settle(*picker) == FilePickerState::Ready);
    picker->on_choose.connect([&](auto const&) { picker.reset(); });
    picker->accept();
    settle(*picker);
    ASSERT(!picker);
}

TEST(form_ctrl_enter_submits_with_multiline_field_and_focuses_nested_input)
{
    auto form = Form{
        std::tuple{NumericEdit{}, TextBox{}},
        std::vector<FormField>{
            {"Number", "",
             [](Widget const&) -> std::optional<std::string> { return "Required"; }},
            {"Notes"}}};
    auto app = Application{form};
    app.dispatch(esc::Resize{{30, 10}});
    app.set_focus(std::get<1>(form.children));
    int invalid = 0;
    form.on_invalid.connect([&](auto const&) { ++invalid; });
    app.dispatch(KeyEvent{.key = KeyCode::Enter, .modifiers = {.ctrl = true}});
    ASSERT(invalid == 1);
    ASSERT(std::get<0>(form.children).editor.is_focused());
}
TEST(tree_replacement_clears_selection_and_log_trimming_retains_offsets)
{
    auto tree = TreeView{{.nodes = {{"one"}, {"two"}}}};
    tree.select({1});
    tree.set_nodes({{"other"}, {"new"}});
    ASSERT(tree.selected_paths().empty());
    ASSERT(!tree.current_path());
    auto log = LogView{
        {.max_lines = 3, .follow_tail = false, .editor = {.wrap = TextWrap::None}}};
    arrange(log, {{}, {10, 1}});
    log.append("one\ntwo\nthree");
    log.editor.set_caret(4);
    log.editor.set_caret(7, true);
    log.editor.set_scroll_offset({0, 1});
    log.append("four");
    ASSERT(log.editor.text() == "two\nthree\nfour\n");
    ASSERT(
        (log.editor.editor().selection() == std::pair<std::size_t, std::size_t>{0, 3}));
    ASSERT(log.editor.scroll_offset().y == 0);
}
TEST(file_picker_unusual_names_and_filter_lifetime)
{
    auto files = TempFiles{};
    std::ofstream{files.path / "line\nbreak.txt"} << "data";
    auto picker = FilePicker{{.directory = files.path}};
    ASSERT(settle(picker) == FilePickerState::Ready);
    auto chosen = std::filesystem::path{};
    picker.on_choose.connect([&](auto const& p) { chosen = p; });
    std::size_t row = 0;
    for (; row < picker.paths().size(); ++row)
        if (picker.paths()[row].filename() == "line\nbreak.txt") break;
    for (auto& child : picker.get_children())
        if (auto* list = dynamic_cast<ListView*>(&child)) list->select(row);
    ASSERT(picker.accept());
    ASSERT(settle(picker) == FilePickerState::Ready);
    ASSERT(chosen.filename() == "line\nbreak.txt");
    auto owned =
        std::make_unique<FilePicker>(FilePicker::Options{.directory = files.path});
    ASSERT(settle(*owned) == FilePickerState::Ready);
    owned->filter = [](auto const&) -> bool {
        throw std::runtime_error{"Filter failed"};
    };
    owned->on_error.connect([&](auto const&) { owned.reset(); });
    owned->reload();
    settle(*owned);
    ASSERT(!owned);
}
TEST(spinner_timer_follows_moves_and_respects_disabled_widgets)
{
    auto scene =
        Row{Spinner{{.frames = {"a", "b"}, .interval = std::chrono::milliseconds{10}}},
            Label{"Status"}};
    auto app = Application{scene};
    app.dispatch(esc::Resize{{10, 1}});
    auto& spinner = std::get<0>(scene.children);
    auto before = Frame{{1, 1}};
    spinner.paint(Canvas{before});
    auto time = EventLoop::Clock::now();
    app.step(time + std::chrono::milliseconds{20});
    auto after = Frame{{1, 1}};
    spinner.paint(Canvas{after});
    ASSERT((before[Point{0, 0}].text != after[Point{0, 0}].text));
    spinner.enabled = false;
    app.step(time + std::chrono::milliseconds{50});
    auto disabled = Frame{{1, 1}};
    spinner.paint(Canvas{disabled});
    ASSERT((after[Point{0, 0}].text == disabled[Point{0, 0}].text));
}
TEST(paint_percentage_preserves_progress_track_and_tiny_geometry)
{
    auto bar = ProgressBar{{.value = 50}};
    auto frame = Frame{{20, 1}};
    bar.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"█"));
    ASSERT((frame[Point{19, 0}].text == U"░"));
    for (auto size : {Area{0, 0}, Area{1, 1}, Area{2, 1}}) {
        auto f = Frame{size};
        auto split = SplitPane{Label{"a"}, Label{"b"}};
        arrange(split, {{}, size});
        split.paint(Canvas{f});
        auto picker = FilePicker{};
        arrange(picker, {{}, size});
        picker.paint(Canvas{f});
        bar.paint(Canvas{f});
    }
}

TEST(file_picker_invalid_native_encoding_is_displayable)
{
    auto files = TempFiles{};
    auto name = std::string{"bad-"} + char(0xff);
    std::ofstream{files.path / name} << "data";
    auto picker = FilePicker{{.directory = files.path}};
    ASSERT(settle(picker) == FilePickerState::Ready);
    std::error_code ec;
    auto created = std::filesystem::is_regular_file(files.path / name, ec);
    ASSERT(picker.paths().size() == (created ? 4 : 3));
    for (auto const& item : picker.entries().items())
        validate_row_text(item.text);
}
TEST(numeric_readonly_input_and_disabled_command_replacement)
{
    auto number = NumericEdit{{.editor = {.read_only = true}}};
    ASSERT(!number.handle(KeyEvent{.key = KeyCode::ArrowUp}).handled);
    ASSERT(number.value() == 0);
    auto palette = CommandPalette{{command({"one"}), command({"two"})}};
    palette.set_commands({command({"off", "", false}), command({"on"})});
    ASSERT(palette.choices().current_row() == 1);
}

TEST(typed_numeric_edit_preserves_integer_precision_and_rejects_fraction_overflow)
{
    using I = std::int64_t;
    auto number = BasicNumericEdit<I>{
        {.range = {std::numeric_limits<I>::lowest(), std::numeric_limits<I>::max(), 1},
         .value = 9007199254740993LL}};
    static_assert(std::same_as<decltype(number.value()), I>);
    I notified = 0;
    number.on_change.connect([&](I value) { notified = value; });
    number.editor.set_text("9007199254740995");
    ASSERT(number.commit() && notified == 9007199254740995LL);
    for (auto text :
         {"1.5", "9223372036854775808", "-9223372036854775809", "1e2", ""}) {
        number.editor.set_text(text);
        ASSERT(!number.commit() && number.value() == 9007199254740995LL);
    }
    number.editor.set_text("9223372036854775807");
    ASSERT(number.commit());
    number.handle(KeyEvent{.key = KeyCode::ArrowUp});
    ASSERT(number.value() == std::numeric_limits<I>::max());
    number.editor.set_text("-9223372036854775808");
    ASSERT(number.commit());
    number.handle(KeyEvent{.key = KeyCode::ArrowDown});
    ASSERT(number.value() == std::numeric_limits<I>::lowest());
    auto positive = BasicNumericEdit<std::uint64_t>{
        {.range = {0, std::numeric_limits<std::uint64_t>::max(), 1}}};
    positive.editor.set_text("-1");
    ASSERT(!positive.commit());
    positive.editor.set_text("18446744073709551615");
    ASSERT(positive.commit() &&
           positive.value() == std::numeric_limits<std::uint64_t>::max());
}

TEST(numeric_format_focus_units_and_drafts_preserve_the_committed_value)
{
    auto number = NumericEdit{
        {.value = 12.3456789, .format = "{:.2f}", .prefix = "$", .suffix = " kg"}};
    ASSERT(number.editor.text() == "12.35");
    ASSERT(number.commit() && number.value() == 12.3456789);
    auto app = Application{number};
    app.dispatch(esc::Resize{{16, 1}});
    app.set_focus(number.editor);
    ASSERT(number.editor.text() != "12.35");
    ASSERT(number.commit() && number.value() == 12.3456789);
    ASSERT(number.editor.bounds().at.x == 1 && number.editor.size().width == 12);
    number.editor.set_text("13.125");
    app.clear_focus();
    ASSERT(number.has_draft() && number.editor.text() == "13.125");
    ASSERT(number.value() == 12.3456789);
    number.revert();
    ASSERT(number.editor.text() == "12.35");
    number.commit_on_focus_loss = true;
    app.set_focus(number.editor);
    number.editor.set_text("14.125");
    app.clear_focus();
    ASSERT(number.value() == 14.125 && number.editor.text() == "14.12");
    app.set_focus(number.editor);
    number.editor.set_text("bad");
    app.clear_focus();
    ASSERT(number.value() == 14.125 && number.editor.text() == "bad");
    auto frame = Frame{{16, 1}};
    number.paint(Canvas{frame});
    ASSERT((frame[Point{0, 0}].text == U"$"));
    ASSERT((frame[Point{14, 0}].text == U"k"));
    arrange(number, {{}, {1, 1}});
    ASSERT(number.editor.size().width == 0);
}

TEST(numeric_edit_locale_and_integer_alias)
{
    struct Comma : std::numpunct<char> {
        auto do_decimal_point() const -> char override { return ','; }
    };
    auto number =
        NumericEdit{{.format = "{:.2Lf}",
                     .locale = std::locale{std::locale::classic(), new Comma}}};
    number.editor.set_text("1,25");
    ASSERT(number.commit() && number.value() == 1.25);
    ASSERT(number.editor.text() == "1,25");
    auto count = IntegerEdit{{.value = 3, .suffix = " items"}};
    count.editor.set_text("3.5");
    ASSERT(!count.commit() && count.value() == 3);
}

TEST(form_commits_numeric_drafts_before_validation_and_rejects_invalid_drafts)
{
    auto form = Form{std::tuple{IntegerEdit{{.range = {0, 120, 1}, .value = 18}}},
                     std::vector<FormField>{
                         {"Age", "", [](Widget const& w) -> std::optional<std::string> {
                              return static_cast<IntegerEdit const&>(w).value() >= 21
                                         ? std::nullopt
                                         : std::optional<std::string>{"Must be 21"};
                          }}}};
    auto& age = std::get<0>(form.children);
    age.editor.set_text("25");
    ASSERT(form.submit() && age.value() == 25);
    age.editor.set_text("25.5");
    ASSERT(!form.submit() && form.errors()[0] && age.value() == 25);
    age.editor.set_text("19");
    ASSERT(!form.submit() && form.errors()[0] == "Must be 21" && age.value() == 19);
}

TEST(numeric_commit_respects_validator_and_form_supports_explicit_commit_overrides)
{
    auto count = IntegerEdit{
        {.editor = {.validator = [](std::string_view text) { return text != "13"; }}}};
    count.editor.set_text("13");
    ASSERT(!count.commit() && count.value() == 0);
    count.editor.set_text("14");
    ASSERT(count.commit() && count.value() == 14);
    auto form = Form{std::tuple{IntegerEdit{}},
                     std::vector<FormField>{
                         {.label = "Draft", .commit = [](Widget&) { return true; }}}};
    std::get<0>(form.children).editor.set_text("unfinished");
    ASSERT(form.submit() && std::get<0>(form.children).has_draft());
    auto doomed = std::make_unique<IntegerEdit>();
    doomed->editor.validator = [&](std::string_view) {
        doomed.reset();
        return true;
    };
    doomed->editor.set_text("1");
    ASSERT(!doomed->commit());
}

TEST(numeric_edit_and_scroll_connections_follow_moves)
{
    auto input = IntegerEdit{{.value = 2}};
    auto moved = std::move(input);
    moved.editor.set_text("4");
    ASSERT(moved.commit() && moved.value() == 4);
    auto app = Application{moved};
    app.dispatch(esc::Resize{{10, 1}});
    app.set_focus(moved.editor);
    app.dispatch(KeyEvent{.key = KeyCode::ArrowUp});
    ASSERT(moved.value() == 5);
    auto log = LogView{};
    arrange(log, {{}, {8, 1}});
    auto bar = ScrollBar{};
    link(log, bar);
    auto moved_log = std::move(log);
    moved_log.append("a\nb\nc");
    ASSERT(bar.content_length() == moved_log.content_size().height);
    bar.set_position(0);
    ASSERT(moved_log.scroll_offset().y == 0);
}

TEST(tree_selection_cancels_when_expansion_replaces_the_model)
{
    for (bool empty : {false, true}) {
        auto tree = TreeView{
            {.nodes = {{"root", true, false, {{"branch", true, false, {{"leaf"}}}}}}}};
        int expansions = 0;
        tree.on_expand.connect([&](auto const&, bool) {
            ++expansions;
            tree.set_nodes(
                empty ? std::vector<TreeNode>{}
                      : std::vector<TreeNode>{{"replacement",
                                               true,
                                               false,
                                               {{"branch", true, false, {{"leaf"}}}}}});
        });
        ASSERT(!tree.select({0, 0, 0}));
        ASSERT(expansions == 1);
        ASSERT(tree.selected_paths().empty());
        ASSERT(!tree.current_path());
        if (!empty) {
            ASSERT(tree.nodes()[0].text == "replacement");
            ASSERT(!tree.nodes()[0].expanded);
        }
    }
}

TEST(tree_model_replacement_cancels_stale_activation)
{
    auto tree = TreeView{{.nodes = {{"root", true, false, {{"leaf"}}}}}};
    int activations = 0;
    tree.on_activate.connect([&](auto const&) { ++activations; });
    tree.on_expand.connect([&](auto const&, bool) {
        tree.set_nodes({{"replacement"}});
        tree.select({0});
    });
    for (auto& child : tree.get_children()) {
        static_cast<ListView&>(child).activate(0);
    }
    ASSERT(activations == 0);
    ASSERT((tree.selected_paths() == std::vector<std::vector<std::size_t>>{{0}}));
    ASSERT(tree.nodes()[0].text == "replacement");
}

TEST(tree_nested_scroll_rebuild_preserves_the_replacement_selection)
{
    auto tree = TreeView{
        {.nodes = {{"root", true, false, {{"leaf"}}}, {"other"}}, .multiple = true}};
    arrange(tree, {{}, {20, 1}});
    tree.select({1});
    bool armed = true;
    int expansions = 0;
    tree.on_expand.connect([&](auto const&, bool) { ++expansions; });
    tree.on_scroll.connect([&](auto, auto, auto) {
        if (!std::exchange(armed, false)) { return; }
        tree.set_nodes({{"replacement"}, {"new selection"}});
        tree.select({1});
    });
    tree.set_expanded({0}, true);
    ASSERT(!armed);
    ASSERT(expansions == 0);
    ASSERT(tree.nodes()[0].text == "replacement");
    ASSERT((tree.selected_paths() == std::vector<std::vector<std::size_t>>{{1}}));
    ASSERT((tree.current_path() == std::optional{std::vector<std::size_t>{1}}));
}

TEST(tree_selection_follows_a_move_during_expansion)
{
    auto source = std::make_unique<TreeView>(TreeView::Options{
        .nodes = {{"root", true, false, {{"branch", true, false, {{"leaf"}}}}}}});
    auto moved = std::unique_ptr<TreeView>{};
    source->on_expand.connect([&](auto const&, bool) {
        if (source) {
            moved = std::make_unique<TreeView>(std::move(*source));
            source.reset();
        }
    });
    ASSERT(source->select({0, 0, 0}));
    ASSERT(!source && moved);
    ASSERT(
        (moved->selected_paths() == std::vector<std::vector<std::size_t>>{{0, 0, 0}}));
}

TEST(log_capacity_trimming_preserves_paused_viewport_and_selection)
{
    auto log = LogView{
        {.max_lines = 10, .follow_tail = false, .editor = {.wrap = TextWrap::None}}};
    arrange(log, {{}, {4, 1}});
    log.append("zero-long\none-long\ntwo-long\nthree-long\nfour-long");
    log.editor.set_caret(19);
    log.editor.set_caret(22, true);
    log.set_scroll_offset({2, 3});
    log.set_max_lines(3);
    ASSERT(log.editor.text() == "two-long\nthree-long\nfour-long\n");
    ASSERT((log.scroll_offset() == Point{2, 1}));
    ASSERT(
        (log.editor.editor().selection() == std::pair<std::size_t, std::size_t>{0, 3}));
    ASSERT(!log.follows_tail());
    log.set_max_lines(1);
    ASSERT((log.scroll_offset() == Point{2, 0}));
}

TEST(log_capacity_trimming_counts_wrapped_unicode_rows_after_resize)
{
    for (auto wrap : {TextWrap::Word, TextWrap::Any}) {
        auto log =
            LogView{{.max_lines = 10, .follow_tail = false, .editor = {.wrap = wrap}}};
        arrange(log, {{}, {8, 1}});
        log.append("界界界界\nsecond\nthird\nfourth");
        arrange(log, {{}, {4, 1}});
        auto removed = TextLayout{};
        removed.update("界界界界\n", 4, wrap);
        auto rows = (int)removed.lines().size() - 1;
        log.set_scroll_offset({0, rows + 1});
        log.set_max_lines(3);
        ASSERT(log.scroll_offset().y == 1);
        ASSERT(log.editor.text() == "second\nthird\nfourth\n");
        ASSERT(!log.follows_tail());
    }
}

TEST(log_capacity_trimming_keeps_tail_following_and_allows_owner_destruction)
{
    auto log = LogView{{.max_lines = 10, .editor = {.wrap = TextWrap::None}}};
    arrange(log, {{}, {10, 1}});
    log.append("zero\none\ntwo\nthree\nfour");
    log.set_max_lines(2);
    ASSERT(log.follows_tail());
    ASSERT(log.scroll_offset().y ==
           log.content_size().height - log.viewport_size().height);
    auto owned = std::make_unique<LogView>(LogView::Options{.follow_tail = false});
    owned->append("zero\none\ntwo");
    owned->editor.on_change.connect([&](auto const&) { owned.reset(); });
    owned->set_max_lines(1);
    ASSERT(!owned);
}

TEST(tree_selection_callback_replacement_cancels_stale_expansion_notice)
{
    auto tree = TreeView{{.nodes = {{"root", true, true, {{"leaf"}}}}}};
    tree.select({0, 0});
    int expansions = 0;
    tree.on_expand.connect([&](auto const&, bool) { ++expansions; });
    tree.on_select.connect([&](auto const& selected) {
        if (!selected.empty()) { return; }
        tree.set_nodes({{"replacement"}});
        tree.select({0});
    });
    tree.set_expanded({0}, false);
    ASSERT(expansions == 0);
    ASSERT(tree.nodes()[0].text == "replacement");
    ASSERT((tree.selected_paths() == std::vector<std::vector<std::size_t>>{{0}}));
}

TEST(tree_rebuild_callback_exception_restores_selection_notifications)
{
    auto tree = TreeView{{.nodes = {{"root", true, false, {{"leaf"}}}, {"other"}}}};
    arrange(tree, {{}, {20, 1}});
    tree.select({0});
    bool armed = true;
    tree.on_scroll.connect([&](auto, auto, auto) {
        if (std::exchange(armed, false)) { throw std::runtime_error{"callback"}; }
    });
    ASSERT_THROWS(tree.set_expanded({0}, true), std::runtime_error);
    int selections = 0;
    tree.on_select.connect([&](auto const&) { ++selections; });
    ASSERT(tree.select({1}));
    ASSERT(selections == 1);
}
