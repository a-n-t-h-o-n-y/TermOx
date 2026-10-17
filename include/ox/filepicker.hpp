/** @file
 * Asynchronous directory browsing and selected-path validation.
 */
#pragma once
#include <filesystem>
#include <ox/button.hpp>
#include <ox/lineedit.hpp>
#include <ox/listview.hpp>
#include <stop_token>
namespace ox {
/** Whether acceptance validates an existing file, a save-file path, or a directory. */
enum class FilePickerMode {
    OpenFile, ///< Require an existing regular file that passes the optional filter.
    SaveFile, ///< Require a regular-file path with an existing parent directory; do not create or overwrite it.
    Directory ///< Require an existing directory.
};
/** Observable asynchronous directory-listing and selection-validation state. */
enum class FilePickerState {
    Idle, ///< No operation has started.
    Loading, ///< A worker is listing a directory.
    Validating, ///< A worker is checking a selected path.
    Ready, ///< The latest listing or selected-path validation succeeded.
    Error, ///< The latest operation failed; error() holds its message.
    Cancelled ///< Pending work was cancelled and its results invalidated.
};
/** Worker-thread data, containing no widgets. Empty error indicates success. */
struct DirectoryListing {
    /** Directory associated with these entries, supplied by the worker reader. */
    std::filesystem::path directory;
    /** Owned directory entries produced on the worker thread. */
    std::vector<std::filesystem::directory_entry> entries;
    /** Empty on success; otherwise an explanatory directory-listing error. */
    std::string error;
};
/** Read a directory on a worker, honoring cancellation; return owned entries or an error
 * string instead of touching widgets.
 */
auto read_directory(std::filesystem::path const&, std::stop_token) -> DirectoryListing;
/** Asynchronous browser. Worker results are delivered on the application thread. */
class FilePicker : public Widget {
   public:
    using Widget::handle;
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Initial directory to list; listing failures are reported through FilePicker
         * state and error signals.
         */
        std::filesystem::path directory = ".";
        /** Open-file, save-file, or directory-selection policy. */
        FilePickerMode mode = FilePickerMode::OpenFile;
        /** Include hidden directory entries when true; reload after changing it. */
        bool show_hidden = false;
        /** Runs on the worker. Captures must be thread-safe and outlive cancellation.
         */
        std::function<bool(std::filesystem::directory_entry const&)> filter = {};
        /** Worker-thread directory reader; captures must be thread-safe and survive
         * cancellation.
         */
        std::function<DirectoryListing(std::filesystem::path const&, std::stop_token)>
            loader = read_directory;
    };
    /** Owned directory/location editor used by the asynchronous file browser. */
    LineEdit location;
    /** Owned file-name editor used when accepting a file selection. */
    LineEdit filename{{.placeholder = "File name"}};
    /** Owned action button for navigating to the parent directory. */
    Button up{{.label = {.text = "Up"}}};
    /** Owned action button starting selection validation. */
    Button choose{{.label = {.text = "Choose"}}};
    /** Owned action button cancelling pending work or the current picker interaction. */
    Button cancel{{.label = {.text = "Cancel"}}};
    /** Open-file, save-file, or directory-selection policy. */
    FilePickerMode mode;
    /** Include hidden directory entries when true; reload after changing it. */
    bool show_hidden;
    /** Worker functions; reload after changing configuration. Never capture widgets. */
    std::function<bool(std::filesystem::directory_entry const&)> filter;
    /** Worker-thread directory reader; captures must be thread-safe and survive
     * cancellation.
     */
    std::function<DirectoryListing(std::filesystem::path const&, std::stop_token)>
        loader;
    /** Emits the validated selected path on the application thread after asynchronous
     * validation succeeds.
     */
    sl::Signal<void(std::filesystem::path const&)> on_choose;
    /** Emits when the picker interaction is cancelled; the host decides whether to close
     * a surrounding dialog.
     */
    sl::Signal<void()> on_cancel;
    /** Emits an explanatory listing/validation error on the application thread. */
    sl::Signal<void(std::string const&)> on_error;
    /** Emits transitions between listing, validation, ready, error, and cancelled states. */
    sl::Signal<void(FilePickerState)> on_state;
    /** Begin listing the current directory in open-file mode. */
    FilePicker();
    /** Retain browser configuration and start asynchronous listing; errors are reported
     * through state/error signals.
     */
    explicit FilePicker(Options);
    /** Transfer widget identity, retained state, and signal subscriptions; tracked
     * references follow the move.
     */
    FilePicker(FilePicker&&);
    /** Replace this widget's identity and retained state with the source; previous
     * lifetime views of this identity expire.
     */
    auto operator=(FilePicker&&) -> FilePicker&;
    /** Request cancellation and release worker/timer state before destroying child widgets. */
    ~FilePicker();
    /** Borrow the last successfully displayed directory; an in-flight request is
     * available through pending_directory().
     */
    auto directory() const -> std::filesystem::path const& { return directory_; }
    /** Borrow the directory requested by the current or most recent listing operation. */
    auto pending_directory() const -> std::filesystem::path const&
    { return pending_directory_; }
    /** Borrow displayed entry paths in list order until a new listing is applied. */
    auto paths() const -> std::span<std::filesystem::path const> { return paths_; }
    /** Inspect the displayed entry list; directory and selection commands remain with
     * the picker.
     */
    auto entries() const -> ListView const& { return entries_; }
    /** Borrow the last listing/validation error, empty when no error is present. */
    auto error() const -> std::string const& { return error_; }
    /** Return the current asynchronous operation state. */
    auto state() const -> FilePickerState { return state_; }
    /** Return true during either directory loading or selected-path validation. */
    auto is_loading() const -> bool
    {
        return state_ == FilePickerState::Loading ||
               state_ == FilePickerState::Validating;
    }
    /** Return whether work started. Observe on_state/on_error for its result. */
    auto set_directory(std::filesystem::path) -> bool;
    /** Restart listing with current configuration, cancelling superseded work; observe
     * on_state/on_error for completion.
     */
    auto reload() -> bool;
    /** Start asynchronous selected-path validation; on_choose reports success later.
     * Return whether work started.
     */
    auto accept() -> bool;
    /** Cancel pending work, invalidate stale results, and emit on_state(Cancelled).
     * Return false without pending work. This does not emit on_cancel.
     */
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
