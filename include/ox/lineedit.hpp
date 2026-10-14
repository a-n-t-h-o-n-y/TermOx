#pragma once
#include <ox/detail/text_control.hpp>
namespace ox {
/** Single-line input; Enter submits. */
class LineEdit : public detail::TextControl<false> {
   public:
    using Options = detail::TextControl<false>::Options;
    inline static Options const init = {};
    LineEdit(Options options = init) : detail::TextControl<false>{std::move(options)} {}
};
}  // namespace ox
