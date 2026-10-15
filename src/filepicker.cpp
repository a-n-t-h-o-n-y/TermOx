#include <mutex>
#include <ox/filepicker.hpp>
#include <ox/put.hpp>
#include <thread>
namespace ox {
namespace fs = std::filesystem;
namespace {
auto display_path(fs::path const& p) -> std::string
{
    auto bytes = p.string();
    auto escape = [&](bool non_ascii) {
        auto result = std::string{};
        char const hex[] = "0123456789abcdef";
        for (char byte : bytes) {
            auto c = (unsigned char)byte;
            if (c == '\\') { result += "\\\\"; }
            else if (c < 32 || c == 127 || (non_ascii && c >= 128)) {
                result += "\\x";
                result += hex[c >> 4];
                result += hex[c & 15];
            }
            else {
                result += (char)c;
            }
        }
        return result;
    };
    auto result = escape(false);
    try {
        validate_row_text(result);
    }
    catch (esc::UnicodeError const&) {
        return escape(true);
    }
    catch (std::invalid_argument const&) {
        return escape(true);
    }
    return result;
}
}  // namespace
auto read_directory(fs::path const& requested, std::stop_token stop) -> DirectoryListing
{
    auto result = DirectoryListing{};
    std::error_code ec;
    result.directory = fs::canonical(requested, ec);
    if (ec) {
        result.error = ec.message();
        return result;
    }
    if (stop.stop_requested()) { return result; }
    if (!fs::is_directory(result.directory, ec) || ec) {
        result.error = "Not a directory";
        return result;
    }
    auto it = fs::directory_iterator(result.directory, ec);
    while (!ec && it != fs::directory_iterator{} && !stop.stop_requested()) {
        result.entries.push_back(*it);
        it.increment(ec);
    }
    if (ec) { result.error = ec.message(); }
    return result;
}
struct FilePicker::Result {
    fs::path directory;
    std::vector<fs::path> paths;
    std::vector<bool> directories;
    std::vector<ListItem> items;
    std::optional<fs::path> chosen;
    std::string error;
};
struct FilePicker::Load {
    struct State {
        std::mutex mutex;
        std::optional<Result> result;
    };
    std::shared_ptr<State> state = std::make_shared<State>();
    std::stop_source stop;
    ~Load() { stop.request_stop(); }
};
FilePicker::~FilePicker() = default;
FilePicker::FilePicker(FilePicker&&) = default;
auto FilePicker::operator=(FilePicker&&) -> FilePicker& = default;
FilePicker::FilePicker() : FilePicker{Options{}} {}
FilePicker::FilePicker(Options x)
    : Widget{},
      mode{x.mode},
      show_hidden{x.show_hidden},
      filter{std::move(x.filter)},
      loader{std::move(x.loader)},
      poll_timer_{*this, std::chrono::milliseconds{16}, [](FilePicker& self) {
                      return EventResponse{.repaint = self.poll()};
                  }}
{
    if (!loader) {
        throw std::invalid_argument{"FilePicker requires a directory loader"};
    }
    Connection{.signal = location.on_submit,
               .slot = [](std::string const& s, FilePicker& self) {
                   if (s == display_path(self.directory_)) { self.reload(); }
                   else {
                       self.set_directory(s);
                   }
               }}(*this);
    Connection{.signal = filename.on_submit,
               .slot = [](auto const&, FilePicker& self) { self.accept(); }}(*this);
    Connection{
        .signal = entries_.on_current, .slot = [](auto row, FilePicker& self) {
            if (row && *row < self.paths_.size()) {
                if (!self.directories_[*row])
                    self.filename.set_text(display_path(self.paths_[*row].filename()));
            }
        }}(*this);
    Connection{
        .signal = entries_.on_activate,
        .slot = [](std::size_t row, FilePicker& self) { self.activate(row); }}(*this);
    Connection{.signal = up.on_activate, .slot = [](FilePicker& self) {
                   self.set_directory(
                       (self.is_loading() ? self.pending_directory_ : self.directory_)
                           .parent_path());
               }}(*this);
    Connection{.signal = choose.on_activate,
               .slot = [](FilePicker& self) { self.accept(); }}(*this);
    Connection{.signal = cancel.on_activate, .slot = [](FilePicker& self) {
                   auto lifetime = track(self);
                   self.cancel_loading();
                   if (auto* current = lifetime.try_get()) { current->on_cancel(); }
               }}(*this);
    set_directory(std::move(x.directory));
}
auto FilePicker::fail(std::string error) -> bool
{
    load_.reset();
    poll_timer_.stop();
    auto revision = ++revision_;
    state_ = FilePickerState::Error;
    error_ = std::move(error);
    entries_.enabled = true;
    choose.enabled = true;
    refresh();
    auto copy = error_;
    auto lifetime = track(*this);
    on_state(FilePickerState::Error);
    if (auto* self = lifetime.try_get(); self && self->revision_ == revision) {
        self->on_error(copy);
    }
    return false;
}
auto FilePicker::start(std::function<Result(std::stop_token)> job,
                       fs::path path,
                       FilePickerState phase) -> bool
{
    auto next = std::make_unique<Load>();
    auto state = next->state;
    auto stop = next->stop.get_token();
    try {
        auto worker = std::thread{[state, stop, job = std::move(job)] {
            auto result = Result{};
            try {
                result = job(stop);
            }
            catch (std::exception const& e) {
                result.error = e.what();
            }
            catch (...) {
                result.error = "Filesystem operation failed";
            }
            if (stop.stop_requested()) { return; }
            auto lock = std::scoped_lock{state->mutex};
            state->result = std::move(result);
        }};
        worker.detach();
    }
    catch (std::exception const& e) {
        return fail(e.what());
    }
    load_ = std::move(next);
    ++revision_;
    pending_directory_ = std::move(path);
    state_ = phase;
    error_.clear();
    entries_.enabled = false;
    choose.enabled = false;
    poll_timer_.start();
    refresh();
    on_state(phase);
    return true;
}
auto FilePicker::cancel_loading() -> bool
{
    if (!load_) { return false; }
    load_.reset();
    poll_timer_.stop();
    ++revision_;
    state_ = FilePickerState::Cancelled;
    entries_.enabled = true;
    choose.enabled = true;
    refresh();
    on_state(state_);
    return true;
}
auto FilePicker::set_directory(fs::path path) -> bool
{
    if (path.is_relative() && !directory_.empty()) { path = directory_ / path; }
    // Snapshot cwd before dispatch; filesystem canonicalization runs on the worker.
    std::error_code ec;
    path = fs::absolute(path, ec);
    if (ec) { return fail(ec.message()); }
    auto read = loader;
    auto accepts = filter;
    auto hidden = show_hidden;
    auto directories_only = mode == FilePickerMode::Directory;
    return start(
        [path, read = std::move(read), accepts = std::move(accepts), hidden,
         directories_only](std::stop_token stop) {
            auto listing = read(path, stop);
            auto result = Result{};
            result.directory = std::move(listing.directory);
            result.error = std::move(listing.error);
            if (!result.error.empty() || stop.stop_requested()) { return result; }
            auto rows = std::vector<std::pair<bool, fs::path>>{};
            for (auto const& entry : listing.entries) {
                if (stop.stop_requested()) { return result; }
                auto name = entry.path().filename().string();
                if (!hidden && !name.empty() && name[0] == '.') { continue; }
                std::error_code error;
                auto dir = entry.is_directory(error);
                if (error) { continue; }
                if (!dir && (directories_only || (accepts && !accepts(entry)))) {
                    continue;
                }
                rows.push_back({dir, entry.path()});
            }
            std::sort(rows.begin(), rows.end(), [](auto const& a, auto const& b) {
                if (a.first != b.first) { return a.first > b.first; }
                return a.second.filename() < b.second.filename();
            });
            for (auto const& [dir, p] : rows) {
                if (stop.stop_requested()) { return result; }
                result.paths.push_back(p);
                result.directories.push_back(dir);
                result.items.push_back({display_path(p.filename()) + (dir ? "/" : "")});
            }
            return result;
        },
        path, FilePickerState::Loading);
}
auto FilePicker::poll() -> bool
{
    if (!load_) { return false; }
    auto result = std::optional<Result>{};
    {
        auto lock = std::scoped_lock{load_->state->mutex};
        result = std::move(load_->state->result);
    }
    if (!result) { return false; }
    load_.reset();
    poll_timer_.stop();
    auto revision = revision_;
    if (!result->error.empty()) {
        fail(std::move(result->error));
        return true;
    }
    entries_.enabled = true;
    choose.enabled = true;
    state_ = FilePickerState::Ready;
    error_.clear();
    auto lifetime = track(*this);
    auto current = [lifetime, revision]() -> FilePicker* {
        auto* self = lifetime.try_get();
        return self && self->revision_ == revision ? self : nullptr;
    };
    if (result->chosen) {
        on_state(state_);
        if (auto* self = current()) { self->on_choose(*result->chosen); }
        return true;
    }
    directory_ = std::move(result->directory);
    paths_ = std::move(result->paths);
    directories_ = std::move(result->directories);
    entries_.set_items(std::move(result->items));
    auto* self = current();
    if (!self) { return true; }
    self->location.set_text(display_path(self->directory_));
    self = current();
    if (!self) { return true; }
    self->filename.set_text("");
    self = current();
    if (!self) { return true; }
    self->refresh();
    self->on_state(FilePickerState::Ready);
    return true;
}
auto FilePicker::reload() -> bool
{ return set_directory(is_loading() ? pending_directory_ : directory_); }
void FilePicker::activate(std::size_t row)
{
    if (is_loading() || row >= paths_.size()) { return; }
    auto path = paths_[row];
    if (directories_[row]) {
        set_directory(path);
        return;
    }
    auto lifetime = track(*this);
    filename.set_text(display_path(path.filename()));
    if (auto* self = lifetime.try_get()) { self->accept(); }
}
auto FilePicker::accept() -> bool
{
    if (!is_enabled() || is_loading() || directory_.empty()) { return false; }
    auto accepts = filter;
    auto requested_mode = mode;
    auto path = directory_;
    auto row = entries_.current_row();
    if (mode == FilePickerMode::Directory) {
        if (row && *row < paths_.size()) { path = paths_[*row]; }
    }
    else {
        if (filename.text().empty()) { return fail("Enter a file name"); }
        if (row && *row < paths_.size() &&
            filename.text() == display_path(paths_[*row].filename())) {
            path = paths_[*row];
        }
        else {
            path = fs::path{filename.text()};
            if (path.is_relative()) { path = directory_ / path; }
        }
    }
    return start(
        [path, requested_mode, accepts = std::move(accepts)](std::stop_token stop) {
            auto result = Result{};
            std::error_code ec;
            auto resolved = fs::weakly_canonical(path, ec);
            if (ec) {
                result.error = ec.message();
                return result;
            }
            if (stop.stop_requested()) { return result; }
            auto status = fs::status(resolved, ec);
            if (requested_mode == FilePickerMode::SaveFile) {
                if (ec && ec != std::errc::no_such_file_or_directory) {
                    result.error = ec.message();
                    return result;
                }
                ec.clear();
                if (fs::exists(status) && !fs::is_regular_file(status)) {
                    result.error = "Not a regular file";
                    return result;
                }
                if (!fs::is_directory(resolved.parent_path(), ec) || ec) {
                    result.error = "Parent directory does not exist";
                    return result;
                }
            }
            else if (ec || (requested_mode == FilePickerMode::Directory
                                ? !fs::is_directory(status)
                                : !fs::is_regular_file(status))) {
                result.error = requested_mode == FilePickerMode::Directory
                                   ? "Not a directory"
                                   : "Not a regular file";
                return result;
            }
            if (requested_mode == FilePickerMode::OpenFile && accepts) {
                fs::directory_entry entry{resolved, ec};
                if (ec || !accepts(entry)) {
                    result.error = "File does not match the filter";
                    return result;
                }
            }
            result.chosen = std::move(resolved);
            return result;
        },
        directory_, FilePickerState::Validating);
}
auto FilePicker::measure(Area a, esc::WidthPolicy) const -> Area
{
    validate(a);
    return {50, 16};
}
auto FilePicker::get_children() -> zzz::Generator<Widget&>
{
    co_yield location;
    co_yield up;
    co_yield entries_;
    if (mode != FilePickerMode::Directory) co_yield filename;
    co_yield choose;
    co_yield cancel;
}
auto FilePicker::get_children() const -> zzz::Generator<Widget const&>
{
    co_yield location;
    co_yield up;
    co_yield entries_;
    if (mode != FilePickerMode::Directory) co_yield filename;
    co_yield choose;
    co_yield cancel;
}
void FilePicker::on_arrange(Rect)
{
    auto width = size().width, height = size().height;
    auto header = std::min(1, height);
    auto footer = std::min(mode == FilePickerMode::Directory ? 2 : 3,
                           std::max(0, height - header));
    auto body = std::max(0, height - header - footer);
    auto upwidth = std::min(4, width);
    auto placements = std::vector<std::pair<LifetimeView<Widget>, Rect>>{
        {track(static_cast<Widget&>(location)), {{}, {width - upwidth, header}}},
        {track(static_cast<Widget&>(up)), {{width - upwidth, 0}, {upwidth, header}}},
        {track(static_cast<Widget&>(entries_)), {{0, header}, {width, body}}},
    };
    auto y = header + body;
    if (mode != FilePickerMode::Directory) {
        placements.push_back({track(static_cast<Widget&>(filename)),
                              {{0, y}, {width, footer > 0 ? 1 : 0}}});
        y += footer > 0 ? 1 : 0;
    }
    auto buttons = std::min(1, height - y);
    placements.push_back(
        {track(static_cast<Widget&>(choose)), {{0, y}, {width / 2, buttons}}});
    placements.push_back({track(static_cast<Widget&>(cancel)),
                          {{width / 2, y}, {width - width / 2, buttons}}});
    auto lifetime = track(*this);
    for (auto const& [child, rect] : placements) {
        if (!lifetime.valid()) { return; }
        if (child.valid()) { arrange(child.get(), rect); }
    }
}

auto FilePicker::paint(Canvas c) -> Cursor
{
    fill(c, appearance({}));
    auto status =
        is_loading()
            ? std::string{state_ == FilePickerState::Loading ? "Loading…" : "Checking…"}
            : error_;
    if (!status.empty() && c.size().height > 0)
        paint_row_text(c.subcanvas({0, c.size().height - 1}, {c.size().width, 1}),
                       status, Align::Left,
                       error_.empty() ? appearance({}, Surface::Muted)
                                      : appearance(Style{.foreground = colors::red}),
                       error_layout_);
    return {};
}
auto FilePicker::handle(KeyEvent const& e) -> EventResponse
{
    if (!is_enabled()) return {};
    if (is_loading() && e.matches(KeyCode::Escape, {}, KeyAction::Press)) {
        return {.handled = true, .repaint = cancel_loading()};
    }
    if (e.matches(KeyCode::ArrowUp, {.alt = true}, KeyAction::Press))
        return {.handled = true,
                .repaint = set_directory(
                    (is_loading() ? pending_directory_ : directory_).parent_path())};
    if (e.matches(KeyCode::F5, {}, KeyAction::Press))
        return {.handled = true, .repaint = reload()};
    return {};
}
}  // namespace ox
