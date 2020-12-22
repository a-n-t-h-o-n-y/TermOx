#ifndef TERMOX_WIDGET_WIDGETS_LOG_HPP
#define TERMOX_WIDGET_WIDGETS_LOG_HPP
#include <signals_light/signal.hpp>

#include <termox/system/key.hpp>
#include <termox/widget/widgets/text_display.hpp>
#include <termox/widget/widgets/textbox.hpp>

namespace ox {
class Glyph_string;

class Log : public Textbox {
   public:
    void post_message(Glyph_string message);

   protected:
    auto key_press_event(Key k) -> bool override;

    using Text_display::append;
    using Text_display::erase;
    using Text_display::insert;
    using Text_display::pop_back;
    using Text_display::set_contents;
};

/// Helper function to create an instance.
template <typename... Args>
auto log(Args&&... args) -> std::unique_ptr<Log>
{
    return std::make_unique<Log>(std::forward<Args>(args)...);
}

}  // namespace ox

namespace ox::slot {

auto post_message(Log& log) -> sl::Slot<void(Glyph_string)>;
auto post_message(Log& log, Glyph_string const& message) -> sl::Slot<void()>;

}  // namespace ox::slot
#endif  // TERMOX_WIDGET_WIDGETS_LOG_HPP
