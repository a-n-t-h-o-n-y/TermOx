#ifndef CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECTOR_HPP
#define CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECTOR_HPP
#include <memory>
#include <utility>

#include <signals_light/signal.hpp>

#include <cppurses/painter/color.hpp>
#include <cppurses/painter/glyph.hpp>
#include <cppurses/painter/glyph_string.hpp>
#include <cppurses/widget/layouts/horizontal.hpp>
#include <cppurses/widget/layouts/stack.hpp>
#include <cppurses/widget/layouts/vertical.hpp>
#include <cppurses/widget/widgets/cycle_stack.hpp>
#include <cppurses/widget/widgets/scrollbar.hpp>
#include <cppurses/widget/widgets/textbox.hpp>

namespace paint {

class Glyph_sheet : public cppurses::Textbox {
   public:
    /// Emitted when a Glyph is clicked on; sends the Glyph.
    sl::Signal<void(cppurses::Glyph)> selected;

   public:
    Glyph_sheet(cppurses::Glyph_string symbols) : Textbox{std::move(symbols)}
    {
        this->disable_input();
        this->disable_word_wrap();
        this->cursor.disable();
    }

   protected:
    auto mouse_press_event(cppurses::Mouse const& m) -> bool override
    {
        auto const& contents = this->contents();
        if (m.button == cppurses::Mouse::Button::Left && !contents.empty())
            selected(contents[this->index_at(m.local)]);
        return Textbox::mouse_press_event(m);
    }
};

/// A vertically scrollable display of Glyphs.
class Glyph_sheet_scrollable : public cppurses::layout::Horizontal<> {
   public:
    Glyph_sheet& sheet;
    cppurses::VScrollbar& scrollbar = make_child<cppurses::VScrollbar>();

   public:
    /// Emitted when a Glyph is clicked on; sends the Glyph.
    sl::Signal<void(cppurses::Glyph)>& selected = sheet.selected;

   public:
    Glyph_sheet_scrollable(cppurses::Glyph_string symbols)
        : sheet{make_child<Glyph_sheet>(std::move(symbols))}
    {
        link(scrollbar, sheet);
    }
};

/// Pages of Glyph_sheets that allows you to select a Glyph by clicking on it.
class Glyph_selector : public cppurses::Cycle_stack<Glyph_sheet_scrollable> {
   public:
    /// Emitted when a Glyph is clicked on; sends the Glyph.
    sl::Signal<void(cppurses::Glyph)> selected;

   public:
    /// Creates new page in stack, associated with title and hooks up signals.
    auto add_sheet(cppurses::Glyph_string title, cppurses::Glyph_string symbols)
        -> Glyph_sheet&
    {
        auto& result =
            this->Cycle_stack::make_page(std::move(title), std::move(symbols));
        result.selected.connect(
            [this](cppurses::Glyph g) { this->selected(g); });
        return result.sheet;
    }
};

inline auto glyph_selector() -> std::unique_ptr<Glyph_selector>
{
    return std::make_unique<Glyph_selector>();
}

auto populated_glyph_selector() -> std::unique_ptr<Glyph_selector>;

}  // namespace paint
#endif  // CPPURSES_DEMOS_GLYPH_PAINT_GLYPH_SELECTOR_HPP
