#include <ox/filepicker.hpp>
#include <ox/put.hpp>
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
FilePicker::FilePicker() : FilePicker{Options{}} {}
FilePicker::FilePicker(Options x)
    : Widget{}, mode{x.mode}, show_hidden{x.show_hidden}, filter{std::move(x.filter)}
{
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
                std::error_code ec;
                if (!fs::is_directory(self.paths_[*row], ec))
                    self.filename.set_text(display_path(self.paths_[*row].filename()));
            }
        }}(*this);
    Connection{
        .signal = entries_.on_activate,
        .slot = [](std::size_t row, FilePicker& self) { self.activate(row); }}(*this);
    Connection{.signal = up.on_activate, .slot = [](FilePicker& self) {
                   self.set_directory(self.directory_.parent_path());
               }}(*this);
    Connection{.signal = choose.on_activate,
               .slot = [](FilePicker& self) { self.accept(); }}(*this);
    Connection{.signal = cancel.on_activate,
               .slot = [](FilePicker& self) { self.on_cancel(); }}(*this);
    set_directory(std::move(x.directory));
}
auto FilePicker::fail(std::string error) -> bool
{
    error_ = std::move(error);
    request_repaint();
    auto copy = error_;
    on_error(copy);
    return false;
}
auto FilePicker::set_directory(fs::path path) -> bool
{
    auto lifetime = track(*this);
    auto accepts = filter;
    auto hidden = show_hidden;
    auto directories_only = mode == FilePickerMode::Directory;
    auto reject = [lifetime](std::string error) {
        auto* self = lifetime.try_get();
        return self ? self->fail(std::move(error)) : false;
    };
    std::error_code ec;
    if (path.is_relative() && !directory_.empty()) path = directory_ / path;
    path = fs::canonical(path, ec);
    if (ec) return reject(ec.message());
    if (!fs::is_directory(path, ec) || ec) return reject("Not a directory");
    auto rows = std::vector<std::pair<bool, fs::path>>{};
    auto it = fs::directory_iterator(path, ec);
    if (ec) return reject(ec.message());
    for (; it != fs::directory_iterator{}; it.increment(ec)) {
        if (ec) return reject(ec.message());
        auto const& entry = *it;
        auto name = entry.path().filename().string();
        if (!hidden && !name.empty() && name[0] == '.') continue;
        auto dir = entry.is_directory(ec);
        if (ec) {
            ec.clear();
            continue;
        }
        if (!dir && (directories_only || (accepts && !accepts(entry)))) continue;
        if (!lifetime.valid()) { return false; }
        rows.push_back({dir, entry.path()});
    }
    if (ec) return reject(ec.message());
    std::sort(rows.begin(), rows.end(), [](auto const& a, auto const& b) {
        if (a.first != b.first) return a.first > b.first;
        return a.second.filename() < b.second.filename();
    });
    auto paths = std::vector<fs::path>{};
    auto items = std::vector<ListItem>{};
    for (auto const& [dir, p] : rows) {
        paths.push_back(p);
        items.push_back({display_path(p.filename()) + (dir ? "/" : "")});
    }
    auto* self = lifetime.try_get();
    if (!self) { return false; }
    self->directory_ = std::move(path);
    self->paths_ = std::move(paths);
    self->error_.clear();
    self->entries_.set_items(std::move(items));
    self = lifetime.try_get();
    if (!self) return true;
    self->location.set_text(display_path(self->directory_));
    self = lifetime.try_get();
    if (!self) return true;
    self->filename.set_text("");
    self = lifetime.try_get();
    if (!self) return true;
    self->refresh();
    return true;
}
auto FilePicker::reload() -> bool { return set_directory(directory_); }
void FilePicker::activate(std::size_t row)
{
    if (row >= paths_.size()) return;
    auto path = paths_[row];
    std::error_code ec;
    if (fs::is_directory(path, ec)) {
        set_directory(path);
        return;
    }
    auto lifetime = track(*this);
    filename.set_text(display_path(path.filename()));
    if (auto* self = lifetime.try_get()) self->accept();
}
auto FilePicker::accept() -> bool
{
    if (!is_enabled() || directory_.empty()) return false;
    auto lifetime = track(*this);
    auto accepts = filter;
    auto requested_mode = mode;
    auto path = directory_;
    auto row = entries_.current_row();
    if (requested_mode == FilePickerMode::Directory) {
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
    std::error_code ec;
    path = fs::weakly_canonical(path, ec);
    if (ec) return fail(ec.message());
    auto status = fs::status(path, ec);
    if (requested_mode == FilePickerMode::SaveFile) {
        if (ec && ec != std::errc::no_such_file_or_directory) return fail(ec.message());
        ec.clear();
        if (fs::exists(status) && !fs::is_regular_file(status))
            return fail("Not a regular file");
        if (!fs::is_directory(path.parent_path(), ec) || ec)
            return fail("Parent directory does not exist");
    }
    else if (ec || (requested_mode == FilePickerMode::Directory
                        ? !fs::is_directory(status)
                        : !fs::is_regular_file(status)))
        return fail(requested_mode == FilePickerMode::Directory ? "Not a directory"
                                                                : "Not a regular file");
    if (requested_mode == FilePickerMode::OpenFile && accepts) {
        fs::directory_entry entry{path, ec};
        auto allowed = !ec && accepts(entry);
        auto* self = lifetime.try_get();
        if (!self) { return false; }
        if (!allowed) { return self->fail("File does not match the filter"); }
    }
    auto* self = lifetime.try_get();
    if (!self) { return false; }
    self->error_.clear();
    self->request_repaint();
    self->on_choose(path);
    return true;
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
    arrange(location, {{}, {width - upwidth, header}});
    arrange(up, {{width - upwidth, 0}, {upwidth, header}});
    arrange(entries_, {{0, header}, {width, body}});
    auto y = header + body;
    if (mode != FilePickerMode::Directory) {
        arrange(filename, {{0, y}, {width, footer > 0 ? 1 : 0}});
        y += footer > 0 ? 1 : 0;
    }
    auto buttons = std::min(1, height - y);
    arrange(choose, {{0, y}, {width / 2, buttons}});
    arrange(cancel, {{width / 2, y}, {width - width / 2, buttons}});
}
auto FilePicker::paint(Canvas c) -> Cursor
{
    fill(c, appearance({}));
    if (!error_.empty() && c.size().height > 0)
        paint_row_text(c.subcanvas({0, c.size().height - 1}, {c.size().width, 1}),
                       error_, Align::Left,
                       appearance(Style{.foreground = colors::red}), error_layout_);
    return {};
}
auto FilePicker::handle(KeyEvent const& e) -> EventResponse
{
    if (!is_enabled()) return {};
    if (e.matches(KeyCode::ArrowUp, {.alt = true}, KeyAction::Press))
        return {.handled = true, .repaint = set_directory(directory_.parent_path())};
    if (e.matches(KeyCode::F5, {}, KeyAction::Press))
        return {.handled = true, .repaint = reload()};
    return {};
}
}  // namespace ox
