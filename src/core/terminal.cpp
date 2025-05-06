#include <ox/core/terminal.hpp>

#include <algorithm>
#include <cassert>

#include <esc/detail/signals.hpp>
#include <esc/detail/transcode.hpp>
#include <esc/io.hpp>
#include <esc/sequence.hpp>
#include <esc/terminal.hpp>

namespace ox {

ScreenBuffer::ScreenBuffer(Area size)
    : size_{size}, buffer_((std::size_t)(size.width * size.height))
{}

auto ScreenBuffer::operator[](Point p) -> Glyph&
{
    auto const at = (std::size_t)(p.y * size_.width + p.x);
    assert(at < buffer_.size());
    return buffer_[at];
}

auto ScreenBuffer::operator[](Point p) const -> Glyph const&
{
    auto const at = (std::size_t)(p.y * size_.width + p.x);
    assert(at < buffer_.size());
    return buffer_[at];
}

void ScreenBuffer::resize(Area a)
{
    size_ = a;
    buffer_.resize((std::size_t)(a.width * a.height));
}

void ScreenBuffer::fill(Glyph const& g)
{
    for (auto& glyph : buffer_) {
        glyph = g;
    }
}

// -------------------------------------------------------------------------------------

Terminal::Terminal(Options x)
    : Terminal{x.mouse_mode, x.key_mode, x.signals, x.foreground, x.background}
{}

Terminal::Terminal(MouseMode mouse_mode,
                   KeyMode key_mode,
                   Signals signals,
                   Color foreground_,
                   Color background_)
    : foreground{foreground_},
      background{background_},
      terminal_input_thread_{[this](auto st) { this->run_read_loop(st); }}
{
    esc::initialize_interactive_terminal(mouse_mode, key_mode, signals);
}

Terminal::~Terminal()
{
    terminal_input_thread_.request_stop();
    esc::uninitialize_terminal();
}

void Terminal::commit_changes()
{
    escape_sequence_.clear();

    if (this->changes.size() != current_screen_.size()) {
        current_screen_.resize(this->changes.size());
        current_screen_.fill(Glyph{U'\0'});  // Trigger Repaint
    }

    auto brush = Brush{};

    for (auto y = 0; y < this->changes.size().height; ++y) {
        for (auto x = 0; x < this->changes.size().width; ++x) {
            auto const change = [&] {
                auto g = this->changes[{x, y}];
                if (g.brush.background == Color{TermColor::Default}) {
                    g.brush.background = this->background;
                }
                if (g.brush.foreground == Color{TermColor::Default}) {
                    g.brush.foreground = this->foreground;
                }
                return g;
            }();
            auto const& current = current_screen_[{x, y}];
            if (change != current) {
                escape_sequence_ += escape(esc::Cursor{x, y});
                if (change.brush != brush) {
                    escape_sequence_ += escape(change.brush);
                    brush = change.brush;
                }
                escape_sequence_ += esc::detail::u32_to_u8(change.symbol);
                current_screen_[{x, y}] = change;
            }
        }
    }
    // Reset brush for consistency with above optimization.
    escape_sequence_ += escape(Brush{});

    this->changes.fill(Glyph{});

    set(esc::CursorMode::Hide);
    esc::write(escape_sequence_);

    if (cursor.has_value()) {
        set(esc::CursorMode::Show);
        esc::write(escape(esc::Cursor{*cursor}));
    }
    else {
        esc::set(esc::CursorMode::Hide);
    }

    esc::flush();
}

void Terminal::run_read_loop(std::stop_token st)
{
    Terminal::event_queue.enqueue(esc::Resize{Terminal::size()});

    while (!st.stop_requested()) {
        if (esc::sigint_flag == 1) {
            Terminal::event_queue.enqueue(event::Interrupt{});
            return;
        }
        else if (auto const event = esc::read(16); event.has_value()) {
            Terminal::event_queue.enqueue(
                std::visit([](auto const& e) -> Event { return e; }, *event));
            // ^^ Translate from esc::Event to ox::Event ^^
        }
    }
}

auto Terminal::size() -> Area { return esc::terminal_area(); }

// -------------------------------------------------------------------------------------

auto Canvas::operator[](Point p) -> Glyph&
{
    return buffer[{.x = at.x + p.x, .y = at.y + p.y}];
}

auto Canvas::operator[](Point p) const -> Glyph const&
{
    return buffer[{.x = at.x + p.x, .y = at.y + p.y}];
}

}  // namespace ox