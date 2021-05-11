#include "glyph_selector.hpp"

#include <array>
#include <memory>

#include <termox/painter/color.hpp>
#include <termox/painter/glyph.hpp>
#include <termox/painter/glyph_string.hpp>
#include <termox/terminal/terminal.hpp>
#include <termox/widget/detail/link_lifetimes.hpp>

namespace {
using namespace ox;

auto const box_drawing = Glyph_string{
    U"─ ━ │ ┃ ┄ ┅ ┆ ┇ ┈ ┉ ┊ ┋ ┌ ┍ ┎ ┏ ┐ ┑ ┒ ┓ └ ┕ ┖ ┗ ┘ ┙ ┚ ┛ ├ ┝ ┞ ┟ ┠ ┡ ┢ ┣ "
    U"┤ ┥ ┦ ┧ ┨ ┩ ┪ ┫ ┬ ┭ ┮ ┯ ┰ ┱ ┲ ┳ ┴ ┵ ┶ ┷ ┸ ┹ ┺ ┻ ┼ ┽ ┾ ┿ ╀ ╁ ╂ ╃ ╄ ╅ ╆ ╇ "
    U"╈ ╉ ╊ ╋ ╌ ╍ ╎ ╏ ═ ║ ╒ ╓ ╔ ╕ ╖ ╗ ╘ ╙ ╚ ╛ ╜ ╝ ╞ ╟ ╠ ╡ ╢ ╣ ╤ ╥ ╦ ╧ ╨ ╩ ╪ ╫ "
    U"╬ ╭ ╮ ╯ ╰ ╱ ╲ ╳ ╴ ╵ ╶ ╷ ╸ ╹ ╺ ╻ ╼ ╽ ╾ ╿ "};

auto const blocks = Glyph_string{
    U"▀ ▁ ▂ ▃ ▄ ▅ ▆ ▇ █ ▉ ▊ ▋ ▌ ▍ ▎ ▏ ▐ ░ ▒ ▓ ▔ ▕ ▖ ▗ ▘ ▙ ▚ ▛ ▜ ▝ ▞ ▟ "};

auto const legacy = Glyph_string{
    U"🬀 🬁 🬂 🬃 🬄 🬅 🬆 🬇 🬈 🬉 🬊 🬋 🬌 🬍 "
    U"🬎 🬏 🬐 🬑 🬒 🬓 🬔 🬕 🬖 🬗 🬘 🬙 🬚 🬛 "
    U"🬜 🬝 🬞 🬟 🬠 🬡 🬢 🬣 🬤 🬥 🬦 🬧 🬨 🬩 "
    U"🬪 🬫 🬬 🬭 🬮 🬯 🬰 🬱 🬲 🬳 🬴 🬵 🬶 🬷 "
    U"🬸 🬹 🬺 🬻 🬼 🬽 🬾 🬿 🭀 🭁 🭂 🭃 🭄 🭅 "
    U"🭆 🭇 🭈 🭉 🭊 🭋 🭌 🭍 🭎 🭏 🭐 🭑 🭒 🭓 "
    U"🭔 🭕 🭖 🭗 🭘 🭙 🭚 🭛 🭜 🭝 🭞 🭟 🭠 🭡 "
    U"🭢 🭣 🭤 🭥 🭦 🭧 🭨 🭩 🭪 🭫 🭬 🭭 🭮 🭯 "
    U"🭰 🭱 🭲 🭳 🭴 🭵 🭶 🭷 🭸 🭹 🭺 🭻 🭼 🭽 "
    U"🭾 🭿 🮀 🮁 🮂 🮃 🮄 🮅 🮆 🮇 🮈 🮉 🮊 🮋 "
    U"🮌 🮍 🮎 🮏 🮐 🮑 🮒  🮔 🮕 🮖 🮗 🮘 🮙 🮚 "
    U"🮛 🮜 🮝 🮞 🮟 🮠 🮡 🮢 🮣 🮤 🮥 🮦 🮧 🮨 "
    U"🮩 🮪 🮫 🮬 🮭 🮮 🮯 🮰 🮱 🮲 🮳 🮴 🮵 🮶 "
    U"🮷 🮸 🮹 🮺 🮻 🮼 🮽 🮾 🮿 🯀 🯁 🯂 🯃 🯄 "
    U"🯅 🯆 🯇 🯈 🯉 🯊 🯰 🯱 🯲 🯳 🯴 🯵 🯶 🯷 "
    U"🯸 🯹 "};

auto const dwarf_fortress = Glyph_string{
    U"☺ ☻ ♥ ♦ ♣ ♠ • ◘ ○ ◙ ♂ ♀ ♪ ♫ ☼ ► ◄ ↕ ‼ ¶ § ▬ ↨ ↑ ↓ → ← ∟ ↔ ▲ ▼ ! \" # $ % "
    U"& ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I "
    U"J K L M N O P Q R S T U V W X Y Z [ ] ^ _ ` a b c d e f g h i j k l m n "
    U"o p q r s t u v w x y z { | } ~ ⌂ Ç ü é â ä à å ç ê ë è ï î ì Ä Å É æ Æ "
    U"ô ö ò û ù ÿ Ö Ü ¢ £ ¥ ₧ ƒ á í ó ú ñ Ñ ª º ¿ ⌐ ¬ ½ ¼ ¡ « » ░ ▒ ▓ │ ┤ ╡ ╢ "
    U"╖ ╕ ╣ ║ ╗ ╝ ╜ ╛ ┐ └ ┴ ┬ ├ ─ ┼ ╞ ╟ ╚ ╔ ╩ ╦ ╠ ═ ╬ ╧ ╨ ╤ ╥ ╙ ╘ ╒ ╓ ╫ ╪ ┘ ┌ "
    U"█ ▄ ▌ ▐ ▀ α ß Γ π Σ σ µ τ Φ Θ Ω δ ∞ φ ε ∩ ≡ ± ≥ ≤ ⌠ ⌡ ÷ ≈ ° ∙ · √ ⁿ ² ■"};

auto const arrows = Glyph_string{
    U"← ↑ → ↓ ↔ ↕ ↖ ↗ ↘ ↙ ↚ ↛ ↜ ↝ ↞ ↟ ↠ ↡ ↢ ↣ ↤ ↥ ↦ ↧ ↨ ↩ ↪ ↫ ↬ ↭ ↮ ↯ ↰ ↱ ↲ ↳ "
    U"↴ ↵ ↶ ↷ ↸ ↹ ↺ ↻ ↼ ↽ ↾ ↿ ⇀ ⇁ ⇂ ⇃ ⇄ ⇅ ⇆ ⇇ ⇈ ⇉ ⇊ ⇋ ⇌ ⇍ ⇎ ⇏ ⇐ ⇑ ⇒ ⇓ ⇔ ⇕ ⇖ ⇗ "
    U"⇘ ⇙ ⇚ ⇛ ⇜ ⇝ ⇞ ⇟ ⇠ ⇡ ⇢ ⇣ ⇤ ⇥ ⇦ ⇧ ⇨ ⇩ ⇪ ⇫ ⇬ ⇭ ⇮ ⇯ ⇰ ⇱ ⇲ ⇳ ⇴ ⇵ ⇶ ⇷ ⇸ ⇹ ⇺ ⇻ "
    U"⇼ ⇽ ⇾ ⇿"};

auto const currency = Glyph_string{
    U"₠ ₡ ₢ ₣ ₤ ₥ ₦ ₧ ₨ ₩ ₪ ₫ € ₭ ₮ ₯ ₰ ₱ ₲ ₳ ₴ ₵ ₶ ₷ ₸ ₹ ₺ "
    U"₻ "
    U"₼ "
    U"₽ "
    U"₾ "
    U"₿"};

auto const math_operators = Glyph_string{
    U"∀ ∁ ∂ ∃ ∄ ∅ ∆ ∇ ∈ ∉ ∊ ∋ ∌ ∍ ∎ ∏ ∐ ∑ − ∓ ∔ ∕ ∖ ∗ ∘ ∙ √ ∛ ∜ ∝ ∞ ∟ ∠ ∡ "
    U"∢ ∣ ∤ ∥ ∦ ∧ ∨ ∩ ∪ ∫ ∮ ∱ ∲ ∳ ∴ ∵ ∶ ∷ ∸ ∹ ∺ ∻ ∼ ∽ ∾ ∿ ≀ ≁ ≂ ≃ ≄ ≅ ≆ ≇ ≈ ≉ "
    U"≊ ≋ ≌ ≍ ≎ ≏ ≐ ≑ ≒ ≓ ≔ ≕ ≖ ≗ ≘ ≙ ≚ ≛ ≜ ≝ ≞ ≟ ≠ ≡ ≢ ≣ ≤ ≥ ≦ ≧ ≨ ≩ ≪ ≫ "
    U"≬ ≭ ≮ ≯ ≰ ≱ ≲ ≳ ≴ ≵ ≶ ≷ ≸ ≹ ≺ ≻ ≼ ≽ ≾ ≿ ⊀ ⊁ ⊂ ⊃ ⊄ ⊅ ⊆ ⊇ ⊈ ⊉ ⊊ ⊋ ⊌ ⊍ ⊎ ⊏ "
    U"⊐ ⊑ ⊒ ⊓ ⊔ ⊕ ⊖ ⊗ ⊘ ⊙ ⊚ ⊛ ⊜ ⊝ ⊞ ⊟ ⊠ ⊡ ⊢ ⊣ ⊤ ⊥ ⊦ ⊧ ⊨ ⊩ ⊪ ⊫ ⊬ ⊭ ⊮ ⊯ ⊰ ⊱ ⊲ ⊳ "
    U"⊴ ⊵ ⊶ ⊷ ⊸ ⊹ ⊺ ⊻ ⊼ ⊽ ⊾ ⊿ ⋀ ⋁ ⋂ ⋃ ⋄ ⋅ ⋆ ⋇ ⋈ ⋉ ⋊ ⋋ ⋌ ⋍ ⋎ ⋏ ⋐ ⋑ ⋒ ⋓ ⋔ ⋕ ⋖ ⋗ "
    U"⋘ ⋙ ⋚ ⋛ ⋜ ⋝ ⋞ ⋟ ⋠ ⋡ ⋢ ⋣ ⋤ ⋥ ⋦ ⋧ ⋨ ⋩ ⋪ ⋫ ⋬ ⋭ ⋮ ⋯ ⋰ ⋱ ⋲ ⋳ ⋴ ⋵ ⋶ ⋷ ⋸ ⋹ ⋺ ⋻ "
    U"⋼ ⋽ ⋾ ⋿"};

auto const geometric = Glyph_string{
    U"■ □ ▢ ▣ ▤ ▥ ▦ ▧ ▨ ▩ ▪ ▫ ▬ ▭ ▮ ▯ ▰ ▱ ▲ △ ▴ ▵ ▶ ▷ ▸ ▹ ► ▻ ▼ ▽ ▾ ▿ ◀ ◁ ◂ ◃ "
    U"◄ ◅ ◆ ◇ ◈ ◉ ◊ ○ ◌ ◍ ◎ ● ◐ ◑ ◒ ◓ ◔ ◕ ◖◗ ◘ ◙ ◚ ◛ ◜ ◝ ◞ ◟ ◠ ◡ ◢ ◣ ◤ ◥ ◦ ◧ ◨ "
    U"◩ ◪ ◫ ◬ ◭ ◮ ◯ ◰ ◱ ◲ ◳ ◴ ◵ ◶ ◷ ◸ ◹ ◺ ◻ ◼ ◽ ◾ ◿"};

auto const unicode_symbols = Glyph_string{
    U"‐ ‑ ‒ – — ― ‖ ‗ ‘ ’ ‚ ‛ “ ” „ ‟ † ‡ • ‣ ․ ‥ … ‧ ‰ ‱ ′ ″ ‴ ‵ ‶ ‷ ‸ ‹ › ※ "
    U"‼ ‽ ‾ ‿ ⁀ ⁁ ⁂ ⁃ ⁄ ⁅ ⁆ ⁇ ⁈ ⁉ ⁊ ⁋ ⁌ ⁍ ⁎ ⁏ ⁐ ⁑ ⁒ ⁓ ⁔ ⁕ ⁖ ⁗ ⁘ ⁙ ⁚ ⁛ ⁜ ⁝ ⁞ "};

auto const latin_1 = Glyph_string{
    U"¡ ¢ £ ¤ ¥ ¦ § ¨ © ª « ¬ ­ ® ¯ ° ± ² ³ ´ µ ¶ · ¸ ¹ º » ¼ ½ ¾ ¿ À Á Â Ã Ä "
    U"Å Æ Ç È É Ê Ë Ì Í Î Ï Ð Ñ Ò Ó Ô Õ Ö × Ø Ù Ú Û Ü Ý Þ ß à á â ã ä å æ ç è "
    U"é ê ë ì í î ï ð ñ ò ó ô õ ö ÷ ø ù ú û ü ý þ ÿ"};

auto color_blocks(Palette const& p) -> Glyph_string
{
    auto constexpr blocks = std::array{U'░', U'▒', U'▓', U'█'};

    auto result = Glyph_string{};
    for (auto i = 0uL; i < p.size(); ++i) {
        for (auto j = i + 1; j < p.size(); ++j) {
            result.append(U'█' | fg(p[i].color));
            for (char32_t const block : blocks)
                result.append(block | bg(p[i].color) | fg(p[j].color));
        }
    }
    return result;
}

auto color_shapes(Palette const& p) -> Glyph_string
{
    auto constexpr shapes = std::array{
        U'🮌', U'🮍', U'🮎', U'🮏', U'🮜', U'🮝',
        U'🮞', U'🮟', U'🮑', U'🮒', U'🮔',
    };

    auto result = Glyph_string{};
    for (auto i = 0uL; i < p.size(); ++i) {
        for (auto j = i + 1; j < p.size(); ++j) {
            for (char32_t const shape : shapes) {
                result.append(shape | bg(p[i].color) | fg(p[j].color));
                result.append(U' ');
            }
        }
    }
    return result;
}

}  // namespace

namespace paint {

auto populated_glyph_selector() -> std::unique_ptr<Glyph_selector>
{
    auto result = glyph_selector();
    result->add_sheet(U"Box Drawing", box_drawing);
    result->add_sheet(U"Blocks", blocks);
    result->add_sheet(U"Legacy", legacy);
    result->add_sheet(U"Dwarf Fortress", dwarf_fortress);
    result->add_sheet(U"Arrows", arrows);
    result->add_sheet(U"Currency", currency);
    result->add_sheet(U"Math Operators", math_operators);
    result->add_sheet(U"Geometric", geometric);
    auto& blocks = result->add_sheet(U"Color Blocks", U"");
    auto& shapes = result->add_sheet(U"Color Shapes", U"");
    result->add_sheet(U"Latin-1", latin_1);

    auto const build_blocks = ox::slot::link_lifetimes(
        [&blocks](ox::Palette const& p) { blocks.set_text(color_blocks(p)); },
        blocks);

    auto const build_shapes = ox::slot::link_lifetimes(
        [&shapes](ox::Palette const& p) { shapes.set_text(color_shapes(p)); },
        shapes);

    ox::Terminal::palette_changed.connect(build_blocks);
    ox::Terminal::palette_changed.connect(build_shapes);

    return result;
}

}  // namespace paint
