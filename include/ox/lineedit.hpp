/** @file
 * Single-line UTF-8 editing with selection and undo history.
 */
#pragma once
#include <ox/detail/text_control.hpp>
namespace ox {
/** Single-line input; Enter submits. */
class LineEdit : public detail::TextControl<false> {
   public:
    /** Initial text, editing policy, appearance, and widget-allocation options from the
     * shared editor implementation.
     */
    using Options = detail::TextControl<false>::Options;
    /** Default options used by construction without an explicit argument. */
    inline static Options const init = {};
    /** Own an editor configured by options; input and edit notifications follow the
     * shared text-control contract.
     */
    LineEdit(Options options = init) : detail::TextControl<false>{std::move(options)} {}
};
}  // namespace ox
