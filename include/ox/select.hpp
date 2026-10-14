#pragma once
#include <ox/button.hpp>
#include <ox/lineedit.hpp>
#include <ox/listview.hpp>
#include <ox/popup.hpp>
namespace ox {
/** Read-only dropdown: navigation previews, activation commits, Escape cancels. */
class Select : public Widget {
   public:
    using Widget::handle;
    struct Options {
        std::vector<ListItem> items = {};
        std::optional<std::size_t> selected = {};
        std::string placeholder = "Select…";
    };
    Button button;
    Popup<ListView> popup;
    sl::Signal<void(std::optional<std::size_t>)> on_change;
    Select();
    explicit Select(Options);
    auto items() const -> std::span<ListItem const> { return popup.child.items(); }
    auto selected_index() const -> std::optional<std::size_t> { return selected_; }
    auto select(std::optional<std::size_t>) -> bool;
    void set_items(std::vector<ListItem>);
    auto open() -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto get_children() -> zzz::Generator<Widget&> override
    {
        co_yield button;
        co_yield popup;
    }
    auto get_children() const -> zzz::Generator<Widget const&> override
    {
        co_yield button;
        co_yield popup;
    }
    auto handle(KeyEvent const&) -> EventResponse override;

   protected:
    void on_arrange(Rect) override;

   private:
    std::optional<std::size_t> selected_;
    std::string placeholder_;
};
/** Editable dropdown. Typed text filters choices; selection commits the full label. */
class ComboBox : public Widget {
   public:
    using Widget::handle;
    struct Options {
        std::vector<ListItem> items = {};
        LineEdit::Options editor = {};
    };
    LineEdit editor;
    Button button;
    Popup<ListView> popup;
    sl::Signal<void(std::string const&)> on_change;
    sl::Signal<void(std::optional<std::size_t>)> on_select;
    ComboBox();
    explicit ComboBox(Options);
    auto items() const -> std::span<ListItem const> { return items_; }
    auto selected_index() const -> std::optional<std::size_t> { return selected_; }
    auto text() const -> std::string const& { return editor.text(); }
    auto set_text(std::string_view value) -> bool { return editor.set_text(value); }
    void set_items(std::vector<ListItem>);
    auto select(std::optional<std::size_t>) -> bool;
    auto open() -> bool;
    auto measure(Area, esc::WidthPolicy = {}) const -> Area override;
    auto get_children() -> zzz::Generator<Widget&> override
    {
        co_yield editor;
        co_yield button;
        co_yield popup;
    }
    auto get_children() const -> zzz::Generator<Widget const&> override
    {
        co_yield editor;
        co_yield button;
        co_yield popup;
    }
    auto handle(KeyEvent const&) -> EventResponse override;

   protected:
    void on_arrange(Rect) override;

   private:
    std::vector<ListItem> items_;
    std::vector<std::size_t> matches_;
    std::optional<std::size_t> selected_;
    void filter();
    void edited(std::string const&);
};
}  // namespace ox
