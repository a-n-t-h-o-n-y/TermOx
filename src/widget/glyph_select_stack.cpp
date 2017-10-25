#include <cppurses/painter/glyph_matrix.hpp>
#include <cppurses/system/mouse_button.hpp>
#include <cppurses/widget/widgets/glyph_select_stack.hpp>
#include <cppurses/widget/widgets/matrix_display.hpp>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace cppurses {

Glyph_select_stack::Glyph_select_stack() {}

void Glyph_select_stack::set_symbols(Glyph_string symbols) {
    symbols_ = std::move(symbols);
    this->update_stack();
}

void Glyph_select_stack::append_symbols(Glyph_string symbols) {
    symbols_.append(std::move(symbols));
    this->update_stack();
}

void Glyph_select_stack::set_page_percent(float percent) {
    // set active page by determining the page from the percent
    std::size_t size{this->size()};
    if (size == 0) {
        return;
    }
    --size;
    std::size_t page_index = std::round(percent * size);
    this->set_active_page(page_index);
}

bool Glyph_select_stack::mouse_press_event_filter(Event_handler* receiver,
                                                  Mouse_button button,
                                                  std::size_t global_x,
                                                  std::size_t global_y,
                                                  std::size_t local_x,
                                                  std::size_t local_y,
                                                  std::uint8_t device_id) {
    if (button == Mouse_button::Left) {
        Matrix_display* md = static_cast<Matrix_display*>(this->active_page());
        if (md != nullptr) {
            Glyph_matrix& matrix{md->matrix};
            if (matrix(local_x, local_y).str() != " ") {
                glyph_selected(matrix(local_x, local_y));
            }
        }
    }
    return Widget_stack::mouse_press_event(button, global_x, global_y, local_x,
                                           local_y, device_id);
}

bool Glyph_select_stack::resize_event(std::size_t new_width,
                                      std::size_t new_height,
                                      std::size_t old_width,
                                      std::size_t old_height) {
    Widget_stack::resize_event(new_width, new_height, old_width, old_height);
    this->update_stack();
    return true;
}

void Glyph_select_stack::update_stack() {
    if (this->width() == 0 || this->height() == 0) {
        return;
    }
    this->clear();
    std::size_t count{0};
    do {
        Matrix_display& md{
            this->make_page<Matrix_display>(this->width(), this->height())};
        md.install_event_filter(this);
        Glyph_matrix& matrix{md.matrix};
        for (std::size_t y{0}; y < matrix.height(); ++y) {
            for (std::size_t x{0}; x < matrix.width(); ++x) {
                if (count < symbols_.size()) {
                    matrix(x, y) = symbols_.at(count++);
                } else {
                    this->update();
                    this->set_active_page(0);
                    return;
                }
            }
        }
    } while (count < symbols_.size());
    if (this->size() > 0) {
        this->set_active_page(0);
    }
    this->update();
}

namespace slot {

sig::Slot<void(float)> set_page_percent(Glyph_select_stack& gss) {
    sig::Slot<void(float)> slot{
        [&gss](float percent) { gss.set_page_percent(percent); }};
    slot.track(gss.destroyed);
    return slot;
}

sig::Slot<void()> set_page_percent(Glyph_select_stack& gss, float percent) {
    sig::Slot<void()> slot{[&gss, percent] { gss.set_page_percent(percent); }};
    slot.track(gss.destroyed);
    return slot;
}

}  // namespace slot

}  // namespace cppurses
