/** @file
 * Popup choice selectors and editable combo boxes.
 */
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
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Owned choices used by the dropdown; disabled rows cannot be committed. */
        std::vector<ListItem> items = {};
        /** Optional initial committed dropdown index. */
        std::optional<std::size_t> selected = {};
        /** Text displayed when the current editable or selected value is empty. */
        std::string placeholder = "Select…";
    };
    /** Owned dropdown button; its activation opens the associated popup. */
    Button button;
    /** Owned overlay containing the dropdown choices; compose through this control's
     * child traversal.
     */
    Popup<ListView> popup;
    /** Emits the changed committed choice index, or nullopt when selection is cleared. */
    sl::Signal<void(std::optional<std::size_t>)> on_change;
    /** Create an empty read-only dropdown with its default placeholder. */
    Select();
    /** Own validated choices and the initial committed selection; navigation previews do
     * not commit.
     */
    explicit Select(Options);
    /** Borrow original dropdown choices until model replacement. */
    auto items() const -> std::span<ListItem const> { return popup.child.items(); }
    /** Return the committed index in the original choices, independent of popup
     * filtering/navigation.
     */
    auto selected_index() const -> std::optional<std::size_t> { return selected_; }
    /** Commit an enabled original choice or clear with nullopt; notify on selection
     * change. Disabled choices return false; invalid indices throw.
     */
    auto select(std::optional<std::size_t>) -> bool;
    /** Validate and replace choices, close any popup, and reconcile committed selection
     * and displayed text.
     */
    void set_items(std::vector<ListItem>);
    /** Open the choices popup when enabled and available; return false when disabled,
     * empty, or already open.
     */
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
    /** Construction-time values; field initializers below define the defaults. */
    struct Options {
        /** Owned choices used by the dropdown; disabled rows cannot be committed. */
        std::vector<ListItem> items = {};
        /** Initial editor configuration; text edits filter the available choices. */
        LineEdit::Options editor = {};
    };
    /** Owned text editor; text edits filter the available choices. */
    LineEdit editor;
    /** Owned dropdown button; its activation opens the associated popup. */
    Button button;
    /** Owned overlay containing the dropdown choices; compose through this control's
     * child traversal.
     */
    Popup<ListView> popup;
    /** Emits edited UTF-8 text after editor changes; the argument is borrowed during
     * emission.
     */
    sl::Signal<void(std::string const&)> on_change;
    /** Emits the optional committed choice index; the index refers to the original
     * unfiltered choices.
     */
    sl::Signal<void(std::optional<std::size_t>)> on_select;
    /** Create an empty editable dropdown with default editor options. */
    ComboBox();
    /** Own validated choices and a text editor; user edits filter visible choices
     * without changing their original indices.
     */
    explicit ComboBox(Options);
    /** Borrow original dropdown choices until model replacement. */
    auto items() const -> std::span<ListItem const> { return items_; }
    /** Return the committed index in the original choices, independent of popup
     * filtering/navigation.
     */
    auto selected_index() const -> std::optional<std::size_t> { return selected_; }
    /** Borrow current editor UTF-8 text until the next document mutation. */
    auto text() const -> std::string const& { return editor.text(); }
    /** Set editor text through its normal mutation/notification path; return whether the
     * document changed.
     */
    auto set_text(std::string_view value) -> bool { return editor.set_text(value); }
    /** Validate and replace choices, close any popup, and reconcile committed selection
     * and displayed text.
     */
    void set_items(std::vector<ListItem>);
    /** Commit an enabled original choice or clear with nullopt; notify on selection
     * change. Disabled choices return false; invalid indices throw.
     */
    auto select(std::optional<std::size_t>) -> bool;
    /** Open the choices popup when enabled and available; return false when disabled,
     * empty, or already open.
     */
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
