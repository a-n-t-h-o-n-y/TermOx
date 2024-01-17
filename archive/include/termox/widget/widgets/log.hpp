#ifndef TERMOX_WIDGET_WIDGETS_LOG_HPP
#define TERMOX_WIDGET_WIDGETS_LOG_HPP
#include <memory>

#include <termox/painter/glyph_string.hpp>
#include <termox/system/key.hpp>
#include <termox/widget/widgets/text_view.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace ox {

/// A scrollable list of logged messages.
/** Received messages are posted at the bottom of the Log. */
class Log : public Textbox {
   public:
    void post_message(Glyph_string message);

   protected:
    auto key_press_event(Key k) -> bool override;

   private:
    using Text_view::append;
    using Text_view::erase;
    using Text_view::insert;
    using Text_view::pop_back;
    using Text_view::set_text;
};

/// Helper function to create a Log instance.
[[nodiscard]] auto log() -> std::unique_ptr<Log>;

}  // namespace ox
#endif  // TERMOX_WIDGET_WIDGETS_LOG_HPP
