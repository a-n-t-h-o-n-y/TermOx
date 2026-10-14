#pragma once
#include <filesystem>
#include <ox/button.hpp>
#include <ox/lineedit.hpp>
#include <ox/listview.hpp>
namespace ox {
enum class FilePickerMode { OpenFile, SaveFile, Directory };
/** A synchronous filesystem browser. Choosing returns a path; never creates files. */
class FilePicker : public Widget {
   public:
    using Widget::handle;
    struct Options {
        std::filesystem::path directory = ".";
        FilePickerMode mode = FilePickerMode::OpenFile;
        bool show_hidden = false;
        std::function<bool(std::filesystem::directory_entry const&)> filter = {};
    };
    LineEdit location;
    LineEdit filename{{.placeholder = "File name"}};
    Button up{{.label = {.text = "Up"}}};
    Button choose{{.label = {.text = "Choose"}}};
    Button cancel{{.label = {.text = "Cancel"}}};
    FilePickerMode mode;
    bool show_hidden;
    /** Filters files, never directories; call reload() after changing options. */
    std::function<bool(std::filesystem::directory_entry const&)> filter;
    sl::Signal<void(std::filesystem::path const&)> on_choose;
    sl::Signal<void()> on_cancel;
    sl::Signal<void(std::string const&)> on_error;
    FilePicker();
    explicit FilePicker(Options);
    auto directory() const -> std::filesystem::path const& { return directory_; }
    auto paths() const -> std::span<std::filesystem::path const> { return paths_; }
    auto entries() const -> ListView const& { return entries_; }
    auto error() const -> std::string const& { return error_; }
    auto set_directory(std::filesystem::path) -> bool;
    auto reload() -> bool;
    auto accept() -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto get_children() -> zzz::Generator<Widget&> override;
    auto get_children() const -> zzz::Generator<Widget const&> override;
    auto handle(KeyEvent const&) -> EventResponse override;

   protected:
    void on_arrange(Rect) override;

   private:
    std::filesystem::path directory_;
    ListView entries_;
    std::vector<std::filesystem::path> paths_;
    std::string error_;
    TextLayout error_layout_;
    auto fail(std::string) -> bool;
    void activate(std::size_t);

   public:
    auto paint(Canvas) -> Cursor override;
};
}  // namespace ox
