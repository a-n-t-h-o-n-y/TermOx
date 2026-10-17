/** @file
 * Multiline UTF-8 editing with wrapping and optional line numbers.
 */
#pragma once
#include <ox/scrollbar.hpp>
#include <ox/detail/text_control.hpp>
namespace ox {
/** Multiline input; Ctrl+Enter submits. Scrolling is independent of the caret. */
class TextBox : public detail::TextControl<true> {
   public:
    /** Initial text, editing policy, appearance, and widget-allocation options from the
     * shared editor implementation.
     */
    using Options = detail::TextControl<true>::Options;
    /** Default options used by construction without an explicit argument. */
    inline static Options const init = {};
    /** Own an editor configured by options; input and edit notifications follow the
     * shared text-control contract.
     */
    TextBox(Options options = init) : detail::TextControl<true>{std::move(options)} {}
};
class ScrollBar;
}  // namespace ox
