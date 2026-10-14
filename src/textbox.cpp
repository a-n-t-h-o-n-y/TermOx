#include <ox/scrollbar.hpp>
#include <ox/textbox.hpp>
namespace ox {
auto link(TextBox& box, ScrollBar& bar) -> std::pair<sl::Identifier, sl::Identifier>
{
    auto synchronize = [](TextBox& box, ScrollBar& bar) {
        auto const content = box.content_size();
        auto const viewport = box.size();
        auto const at = box.scroll_offset();
        bar.set_range(bar.vertical ? content.height : content.width,
                      bar.vertical ? viewport.height : viewport.width,
                      bar.vertical ? at.y : at.x);
    };
    synchronize(box, bar);
    auto first = Connection{
        .signal = box.on_scroll,
        .slot = [synchronize](Point, Area, Area, TextBox& box, ScrollBar& bar) {
            synchronize(box, bar);
        }}(box, bar);
    try {
        return {first, Connection{.signal = bar.on_scroll,
                                  .slot = [](int, TextBox& box, ScrollBar& bar) {
                                      auto at = box.scroll_offset();
                                      if (bar.vertical) { at.y = bar.position(); }
                                      else {
                                          at.x = bar.position();
                                      }
                                      box.set_scroll_offset(at);
                                  }}(box, bar)};
    }
    catch (...) {
        box.on_scroll.disconnect(first);
        throw;
    }
}
}  // namespace ox
