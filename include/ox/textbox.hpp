#pragma once
#include <ox/detail/text_control.hpp>
namespace ox {
/** Multiline input; Ctrl+Enter submits. Scrolling is independent of the caret. */
class TextBox : public detail::TextControl<true> {
   public:
    using Options = detail::TextControl<true>::Options;
    inline static Options const init = {};
    TextBox(Options options = init) : detail::TextControl<true>{std::move(options)} {}
};
class ScrollBar;
auto link(TextBox&, ScrollBar&) -> std::pair<sl::Identifier, sl::Identifier>;
}  // namespace ox
