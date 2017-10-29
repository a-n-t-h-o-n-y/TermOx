#include "populated_glyph_stack.hpp"

namespace {

const Glyph_string box_drawing{
    "─ ━ │ ┃ ┄ ┅ ┆ ┇ ┈ ┉ ┊ ┋ ┌ ┍ ┎ ┏ ┐ ┑ ┒ ┓ └ ┕ ┖ ┗ ┘ ┙ ┚ ┛ ├ ┝ ┞ ┟ ┠ ┡ ┢ ┣ ┤ "
    "┥ ┦ ┧ ┨ ┩ ┪ ┫ ┬ ┭ ┮ ┯ ┰ ┱ ┲ ┳ ┴ ┵ ┶ ┷ ┸ ┹ ┺ ┻ ┼ ┽ ┾ ┿ ╀ ╁ ╂ ╃ ╄ ╅ ╆ ╇ ╈ ╉ "
    "╊ ╋ ╌ ╍ ╎ ╏ ═ ║ ╒ ╓ ╔ ╕ ╖ ╗ ╘ ╙ ╚ ╛ ╜ ╝ ╞ ╟ ╠ ╡ ╢ ╣ ╤ ╥ ╦ ╧ ╨ ╩ ╪ ╫ ╬ ╭ ╮ "
    "╯ ╰ ╱ ╲ ╳ ╴ ╵ ╶ ╷ ╸ ╹ ╺ ╻ ╼ ╽ ╾ ╿ "};

const Glyph_string blocks{
    "▀ ▁ ▂ ▃ ▄ ▅ ▆ ▇ █ ▉ ▊ ▋ ▌ ▍ ▎ ▏ ▐ ░ ▒ ▓ ▔ ▕ ▖ ▗ ▘ ▙ ▚ ▛ ▜ ▝ ▞ ▟ "};

const Glyph_string misc_symbols{
    " ☀ ☁ ☂ ☃ ☄ ★ ☆ ☇ ☈ ☉ ☊ ☋ ☌ ☍ ☎ ☏ ☐ ☑ ☒ ☓☔☕☖ ☗ ☘ ☙ ☚ ☛ ☜ ☝ ☞ ☟ ☠ ☡ ☢ ☣ ☤ ☥ "
    "☦ ☧ ☨☩ ☪ ☫ ☬ ☭ ☮ ☯ ☰ ☱ ☲ ☳ ☴ ☵ ☶ ☷ ☸ ☹ ☺ ☻ ☼☽ ☾ ☿ ♀ ♁ ♂ ♃ ♄ ♅ ♆ ♇ "
    "♈♉♊♋♌♍♎♏♐♑♒♓♔ ♕ ♖ ♗ ♘ ♙ ♚ ♛ ♜ ♝ ♞ ♟ ♠ ♡ ♢ ♣ ♤ ♥ ♦ ♧ ♨ ♩ ♪♫ ♬ ♭ ♮ ♯ ♰ ♱ ♲ "
    "♳ ♴ ♵ ♶ ♷ ♸ ♹ ♺ ♻ ♼ ♽ ♾ ♿⚀ ⚁ ⚂ ⚃ ⚄ ⚅ ⚆ ⚇ ⚈ ⚉ ⚊ ⚋ ⚌ ⚍ ⚎ ⚏ ⚐ ⚑ ⚒ ⚓⚔ ⚕ ⚖ ⚗ ⚘ "
    "⚙ ⚚ ⚛ ⚜ ⚝ ⚞ ⚟ ⚠ ⚡⚢ ⚣ ⚤ ⚥ ⚦ ⚧ ⚨ ⚩ ⚪⚫⚬ ⚭ ⚮ ⚯ ⚰ ⚱ ⚲ ⚳ ⚴ ⚵ ⚶ ⚷ ⚸ ⚹ ⚺ ⚻ ⚼ ⚽⚾⚿ "
    "⛀ ⛁ ⛂ ⛃ ⛄⛅⛆ ⛇ ⛈ ⛉ ⛊ ⛋ ⛌ ⛍ ⛎⛏ ⛐ ⛑ ⛒ ⛓ ⛔⛕ ⛖ ⛗ ⛘ ⛙ ⛚ ⛛ ⛜ ⛝ ⛞ ⛟  ⛠ ⛡ ⛢ ⛣ ⛤ ⛥ "
    "⛦ ⛧ ⛨ ⛩ ⛪⛫ ⛬ ⛭ ⛮ ⛯ ⛰ ⛱ ⛲⛳⛴ ⛵ ⛶ ⛷ ⛸ ⛹ ⛺ ⛻ ⛼ ⛽⛾ ⛿ "};

}  // namespace

namespace demos {
namespace glyph_paint {

Populated_glyph_stack::Populated_glyph_stack() {
    signal_refs.emplace_back(
        this->make_page<Glyph_select>("Box Drawing", box_drawing)
            .glyph_selected);

    signal_refs.emplace_back(
        this->make_page<Glyph_select>("Blocks", blocks).glyph_selected);

    signal_refs.emplace_back(
        this->make_page<Glyph_select>("Misc. Symbols", misc_symbols)
            .glyph_selected);
}

void Populated_glyph_stack::make_connections(sig::Slot<void(Glyph)> slot) {
    for (auto& signal_ref : signal_refs) {
        signal_ref.get().connect(slot);
    }
}

}  // namespace glyph_paint
}  // namespace demos
