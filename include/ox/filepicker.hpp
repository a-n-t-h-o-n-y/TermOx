#pragma once
#include <filesystem>
#include <ox/button.hpp>
#include <ox/lineedit.hpp>
#include <ox/listview.hpp>
#include <stop_token>
namespace ox {
enum class FilePickerMode { OpenFile, SaveFile, Directory };
enum class FilePickerState { Idle, Loading, Validating, Ready, Error, Cancelled };
/** Worker-thread data, containing no widgets. Empty error indicates success. */
struct DirectoryListing {
    std::filesystem::path directory;
    std::vector<std::filesystem::directory_entry> entries;
    std::string error;
};
auto read_directory(std::filesystem::path const&, std::stop_token) -> DirectoryListing;
/** Asynchronous browser. Worker results are delivered on the application thread. */
class FilePicker : public Widget {
   public:
    using Widget::handle;
    struct Options {
        std::filesystem::path directory = ".";
        FilePickerMode mode = FilePickerMode::OpenFile;
        bool show_hidden = false;
        /** Runs on the worker. Captures must be thread-safe and outlive cancellation.
         */
        std::function<bool(std::filesystem::directory_entry const&)> filter = {};
        std::function<DirectoryListing(std::filesystem::path const&, std::stop_token)>
            loader = read_directory;
    };
    LineEdit location;
    LineEdit filename{{.placeholder = "File name"}};
    Button up{{.label = {.text = "Up"}}};
    Button choose{{.label = {.text = "Choose"}}};
    Button cancel{{.label = {.text = "Cancel"}}};
    FilePickerMode mode;
    bool show_hidden;
    /** Worker functions; reload after changing configuration. Never capture widgets. */
    std::function<bool(std::filesystem::directory_entry const&)> filter;
    std::function<DirectoryListing(std::filesystem::path const&, std::stop_token)>
        loader;
    sl::Signal<void(std::filesystem::path const&)> on_choose;
    sl::Signal<void()> on_cancel;
    sl::Signal<void(std::string const&)> on_error;
    sl::Signal<void(FilePickerState)> on_state;
    FilePicker();
    explicit FilePicker(Options);
    FilePicker(FilePicker&&);
    auto operator=(FilePicker&&) -> FilePicker&;
    ~FilePicker();
    auto directory() const -> std::filesystem::path const& { return directory_; }
    auto pending_directory() const -> std::filesystem::path const&
    { return pending_directory_; }
    auto paths() const -> std::span<std::filesystem::path const> { return paths_; }
    auto entries() const -> ListView const& { return entries_; }
    auto error() const -> std::string const& { return error_; }
    auto state() const -> FilePickerState { return state_; }
    auto is_loading() const -> bool
    {
        return state_ == FilePickerState::Loading ||
               state_ == FilePickerState::Validating;
    }
    /** Return whether work started. Observe on_state/on_error for its result. */
    auto set_directory(std::filesystem::path) -> bool;
    auto reload() -> bool;
    auto accept() -> bool;
    auto cancel_loading() -> bool;
    /** Apply a completed worker result; the attached widget timer calls this
     * automatically. */
    auto poll() -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto paint(Canvas) -> Cursor override;
    auto get_children() -> zzz::Generator<Widget&> override;
    auto get_children() const -> zzz::Generator<Widget const&> override;
    auto handle(KeyEvent const&) -> EventResponse override;

   protected:
    void on_arrange(Rect) override;

   private:
    struct Result;
    struct Load;
    std::filesystem::path directory_;
    std::filesystem::path pending_directory_;
    ListView entries_;
    std::vector<std::filesystem::path> paths_;
    std::vector<bool> directories_;
    std::string error_;
    FilePickerState state_ = FilePickerState::Idle;
    std::uint64_t revision_ = 0;
    TextLayout error_layout_;
    std::unique_ptr<Load> load_;
    Timer poll_timer_;
    auto start(std::function<Result(std::stop_token)>,
               std::filesystem::path,
               FilePickerState) -> bool;
    auto fail(std::string) -> bool;
    void activate(std::size_t);
};
}  // namespace ox
