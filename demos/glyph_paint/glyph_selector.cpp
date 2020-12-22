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
    L"─ ━ │ ┃ ┄ ┅ ┆ ┇ ┈ ┉ ┊ ┋ ┌ ┍ ┎ ┏ ┐ ┑ ┒ ┓ └ ┕ ┖ ┗ ┘ ┙ ┚ ┛ ├ ┝ ┞ ┟ ┠ ┡ ┢ ┣ "
    L"┤ ┥ ┦ ┧ ┨ ┩ ┪ ┫ ┬ ┭ ┮ ┯ ┰ ┱ ┲ ┳ ┴ ┵ ┶ ┷ ┸ ┹ ┺ ┻ ┼ ┽ ┾ ┿ ╀ ╁ ╂ ╃ ╄ ╅ ╆ ╇ "
    L"╈ ╉ ╊ ╋ ╌ ╍ ╎ ╏ ═ ║ ╒ ╓ ╔ ╕ ╖ ╗ ╘ ╙ ╚ ╛ ╜ ╝ ╞ ╟ ╠ ╡ ╢ ╣ ╤ ╥ ╦ ╧ ╨ ╩ ╪ ╫ "
    L"╬ ╭ ╮ ╯ ╰ ╱ ╲ ╳ ╴ ╵ ╶ ╷ ╸ ╹ ╺ ╻ ╼ ╽ ╾ ╿ "};

auto const blocks = Glyph_string{
    L"▀ ▁ ▂ ▃ ▄ ▅ ▆ ▇ █ ▉ ▊ ▋ ▌ ▍ ▎ ▏ ▐ ░ ▒ ▓ ▔ ▕ ▖ ▗ ▘ ▙ ▚ ▛ ▜ ▝ ▞ ▟ "};

auto const legacy = Glyph_string{
    L"🬀 🬁 🬂 🬃 🬄 🬅 🬆 🬇 🬈 🬉 🬊 🬋 🬌 🬍 "
    L"🬎 🬏 🬐 🬑 🬒 🬓 🬔 🬕 🬖 🬗 🬘 🬙 🬚 🬛 "
    L"🬜 🬝 🬞 🬟 🬠 🬡 🬢 🬣 🬤 🬥 🬦 🬧 🬨 🬩 "
    L"🬪 🬫 🬬 🬭 🬮 🬯 🬰 🬱 🬲 🬳 🬴 🬵 🬶 🬷 "
    L"🬸 🬹 🬺 🬻 🬼 🬽 🬾 🬿 🭀 🭁 🭂 🭃 🭄 🭅 "
    L"🭆 🭇 🭈 🭉 🭊 🭋 🭌 🭍 🭎 🭏 🭐 🭑 🭒 🭓 "
    L"🭔 🭕 🭖 🭗 🭘 🭙 🭚 🭛 🭜 🭝 🭞 🭟 🭠 🭡 "
    L"🭢 🭣 🭤 🭥 🭦 🭧 🭨 🭩 🭪 🭫 🭬 🭭 🭮 🭯 "
    L"🭰 🭱 🭲 🭳 🭴 🭵 🭶 🭷 🭸 🭹 🭺 🭻 🭼 🭽 "
    L"🭾 🭿 🮀 🮁 🮂 🮃 🮄 🮅 🮆 🮇 🮈 🮉 🮊 🮋 "
    L"🮌 🮍 🮎 🮏 🮐 🮑 🮒  🮔 🮕 🮖 🮗 🮘 🮙 🮚 "
    L"🮛 🮜 🮝 🮞 🮟 🮠 🮡 🮢 🮣 🮤 🮥 🮦 🮧 🮨 "
    L"🮩 🮪 🮫 🮬 🮭 🮮 🮯 🮰 🮱 🮲 🮳 🮴 🮵 🮶 "
    L"🮷 🮸 🮹 🮺 🮻 🮼 🮽 🮾 🮿 🯀 🯁 🯂 🯃 🯄 "
    L"🯅 🯆 🯇 🯈 🯉 🯊 🯰 🯱 🯲 🯳 🯴 🯵 🯶 🯷 "
    L"🯸 🯹 "};

auto const dwarf_fortress = Glyph_string{
    L"☺ ☻ ♥ ♦ ♣ ♠ • ◘ ○ ◙ ♂ ♀ ♪ ♫ ☼ ► ◄ ↕ ‼ ¶ § ▬ ↨ ↑ ↓ → ← ∟ ↔ ▲ ▼ ! \" # $ % "
    L"& ' ( ) * + , - . / 0 1 2 3 4 5 6 7 8 9 : ; < = > ? @ A B C D E F G H I "
    L"J K L M N O P Q R S T U V W X Y Z [ ] ^ _ ` a b c d e f g h i j k l m n "
    L"o p q r s t u v w x y z { | } ~ ⌂ Ç ü é â ä à å ç ê ë è ï î ì Ä Å É æ Æ "
    L"ô ö ò û ù ÿ Ö Ü ¢ £ ¥ ₧ ƒ á í ó ú ñ Ñ ª º ¿ ⌐ ¬ ½ ¼ ¡ « » ░ ▒ ▓ │ ┤ ╡ ╢ "
    L"╖ ╕ ╣ ║ ╗ ╝ ╜ ╛ ┐ └ ┴ ┬ ├ ─ ┼ ╞ ╟ ╚ ╔ ╩ ╦ ╠ ═ ╬ ╧ ╨ ╤ ╥ ╙ ╘ ╒ ╓ ╫ ╪ ┘ ┌ "
    L"█ ▄ ▌ ▐ ▀ α ß Γ π Σ σ µ τ Φ Θ Ω δ ∞ φ ε ∩ ≡ ± ≥ ≤ ⌠ ⌡ ÷ ≈ ° ∙ · √ ⁿ ² ■"};

auto const arrows = Glyph_string{
    L"← ↑ → ↓ ↔ ↕ ↖ ↗ ↘ ↙ ↚ ↛ ↜ ↝ ↞ ↟ ↠ ↡ ↢ ↣ ↤ ↥ ↦ ↧ ↨ ↩ ↪ ↫ ↬ ↭ ↮ ↯ ↰ ↱ ↲ ↳ "
    L"↴ ↵ ↶ ↷ ↸ ↹ ↺ ↻ ↼ ↽ ↾ ↿ ⇀ ⇁ ⇂ ⇃ ⇄ ⇅ ⇆ ⇇ ⇈ ⇉ ⇊ ⇋ ⇌ ⇍ ⇎ ⇏ ⇐ ⇑ ⇒ ⇓ ⇔ ⇕ ⇖ ⇗ "
    L"⇘ ⇙ ⇚ ⇛ ⇜ ⇝ ⇞ ⇟ ⇠ ⇡ ⇢ ⇣ ⇤ ⇥ ⇦ ⇧ ⇨ ⇩ ⇪ ⇫ ⇬ ⇭ ⇮ ⇯ ⇰ ⇱ ⇲ ⇳ ⇴ ⇵ ⇶ ⇷ ⇸ ⇹ ⇺ ⇻ "
    L"⇼ ⇽ ⇾ ⇿"};

auto const currency = Glyph_string{
    L"₠ ₡ ₢ ₣ ₤ ₥ ₦ ₧ ₨ ₩ ₪ ₫ € ₭ ₮ ₯ ₰ ₱ ₲ ₳ ₴ ₵ ₶ ₷ ₸ ₹ ₺ "
    L"₻ "
    L"₼ "
    L"₽ "
    L"₾ "
    L"₿"};

auto const math_operators = Glyph_string{
    L"∀ ∁ ∂ ∃ ∄ ∅ ∆ ∇ ∈ ∉ ∊ ∋ ∌ ∍ ∎ ∏ ∐ ∑ − ∓ ∔ ∕ ∖ ∗ ∘ ∙ √ ∛ ∜ ∝ ∞ ∟ ∠ ∡ "
    L"∢ ∣ ∤ ∥ ∦ ∧ ∨ ∩ ∪ ∫ ∮ ∱ ∲ ∳ ∴ ∵ ∶ ∷ ∸ ∹ ∺ ∻ ∼ ∽ ∾ ∿ ≀ ≁ ≂ ≃ ≄ ≅ ≆ ≇ ≈ ≉ "
    L"≊ ≋ ≌ ≍ ≎ ≏ ≐ ≑ ≒ ≓ ≔ ≕ ≖ ≗ ≘ ≙ ≚ ≛ ≜ ≝ ≞ ≟ ≠ ≡ ≢ ≣ ≤ ≥ ≦ ≧ ≨ ≩ ≪ ≫ "
    L"≬ ≭ ≮ ≯ ≰ ≱ ≲ ≳ ≴ ≵ ≶ ≷ ≸ ≹ ≺ ≻ ≼ ≽ ≾ ≿ ⊀ ⊁ ⊂ ⊃ ⊄ ⊅ ⊆ ⊇ ⊈ ⊉ ⊊ ⊋ ⊌ ⊍ ⊎ ⊏ "
    L"⊐ ⊑ ⊒ ⊓ ⊔ ⊕ ⊖ ⊗ ⊘ ⊙ ⊚ ⊛ ⊜ ⊝ ⊞ ⊟ ⊠ ⊡ ⊢ ⊣ ⊤ ⊥ ⊦ ⊧ ⊨ ⊩ ⊪ ⊫ ⊬ ⊭ ⊮ ⊯ ⊰ ⊱ ⊲ ⊳ "
    L"⊴ ⊵ ⊶ ⊷ ⊸ ⊹ ⊺ ⊻ ⊼ ⊽ ⊾ ⊿ ⋀ ⋁ ⋂ ⋃ ⋄ ⋅ ⋆ ⋇ ⋈ ⋉ ⋊ ⋋ ⋌ ⋍ ⋎ ⋏ ⋐ ⋑ ⋒ ⋓ ⋔ ⋕ ⋖ ⋗ "
    L"⋘ ⋙ ⋚ ⋛ ⋜ ⋝ ⋞ ⋟ ⋠ ⋡ ⋢ ⋣ ⋤ ⋥ ⋦ ⋧ ⋨ ⋩ ⋪ ⋫ ⋬ ⋭ ⋮ ⋯ ⋰ ⋱ ⋲ ⋳ ⋴ ⋵ ⋶ ⋷ ⋸ ⋹ ⋺ ⋻ "
    L"⋼ ⋽ ⋾ ⋿"};

auto const geometric = Glyph_string{
    L"■ □ ▢ ▣ ▤ ▥ ▦ ▧ ▨ ▩ ▪ ▫ ▬ ▭ ▮ ▯ ▰ ▱ ▲ △ ▴ ▵ ▶ ▷ ▸ ▹ ► ▻ ▼ ▽ ▾ ▿ ◀ ◁ ◂ ◃ "
    L"◄ ◅ ◆ ◇ ◈ ◉ ◊ ○ ◌ ◍ ◎ ● ◐ ◑ ◒ ◓ ◔ ◕ ◖◗ ◘ ◙ ◚ ◛ ◜ ◝ ◞ ◟ ◠ ◡ ◢ ◣ ◤ ◥ ◦ ◧ ◨ "
    L"◩ ◪ ◫ ◬ ◭ ◮ ◯ ◰ ◱ ◲ ◳ ◴ ◵ ◶ ◷ ◸ ◹ ◺ ◻ ◼ ◽ ◾ ◿"};

auto const unicode_symbols = Glyph_string{
    L"‐ ‑ ‒ – — ― ‖ ‗ ‘ ’ ‚ ‛ “ ” „ ‟ † ‡ • ‣ ․ ‥ … ‧ ‰ ‱ ′ ″ ‴ ‵ ‶ ‷ ‸ ‹ › ※ "
    L"‼ ‽ ‾ ‿ ⁀ ⁁ ⁂ ⁃ ⁄ ⁅ ⁆ ⁇ ⁈ ⁉ ⁊ ⁋ ⁌ ⁍ ⁎ ⁏ ⁐ ⁑ ⁒ ⁓ ⁔ ⁕ ⁖ ⁗ ⁘ ⁙ ⁚ ⁛ ⁜ ⁝ ⁞ "};

auto const latin_1 = Glyph_string{
    L"¡ ¢ £ ¤ ¥ ¦ § ¨ © ª « ¬ ­ ® ¯ ° ± ² ³ ´ µ ¶ · ¸ ¹ º » ¼ ½ ¾ ¿ À Á Â Ã Ä "
    L"Å Æ Ç È É Ê Ë Ì Í Î Ï Ð Ñ Ò Ó Ô Õ Ö × Ø Ù Ú Û Ü Ý Þ ß à á â ã ä å æ ç è "
    L"é ê ë ì í î ï ð ñ ò ó ô õ ö ÷ ø ù ú û ü ý þ ÿ"};

auto color_blocks(Palette const& p) -> Glyph_string
{
    auto constexpr blocks = std::array{L'░', L'▒', L'▓', L'█'};

    auto result = Glyph_string{};
    for (auto i = 0uL; i < p.size(); ++i) {
        for (auto j = i + 1uL; j < p.size(); ++j) {
            result.append(L'█' | fg(p[i].color));
            for (wchar_t const block : blocks)
                result.append(block | bg(p[i].color) | fg(p[j].color));
        }
    }
    return result;
}

auto color_shapes(Palette const& p) -> Glyph_string
{
    auto constexpr shapes = std::array{
        L'🮌', L'🮍', L'🮎', L'🮏', L'🮜', L'🮝',
        L'🮞', L'🮟', L'🮑', L'🮒', L'🮔',
    };

    auto result = Glyph_string{};
    for (auto i = 0uL; i < p.size(); ++i) {
        for (auto j = i + 1uL; j < p.size(); ++j) {
            for (wchar_t const shape : shapes) {
                result.append(shape | bg(p[i].color) | fg(p[j].color));
                result.append(L' ');
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
    result->add_sheet(L"Box Drawing", box_drawing);
    result->add_sheet(L"Blocks", blocks);
    result->add_sheet(L"Legacy", legacy);
    result->add_sheet(L"Dwarf Fortress", dwarf_fortress);
    result->add_sheet(L"Arrows", arrows);
    result->add_sheet(L"Currency", currency);
    result->add_sheet(L"Math Operators", math_operators);
    result->add_sheet(L"Geometric", geometric);
    auto& blocks = result->add_sheet(L"Color Blocks", L"");
    auto& shapes = result->add_sheet(L"Color Shapes", L"");
    result->add_sheet(L"Latin-1", latin_1);

    auto const build_blocks = ox::slot::link_lifetimes(
        [&blocks](ox::Palette const& p) {
            blocks.set_contents(color_blocks(p));
        },
        blocks);

    auto const build_shapes = ox::slot::link_lifetimes(
        [&shapes](ox::Palette const& p) {
            shapes.set_contents(color_shapes(p));
        },
        shapes);

    ox::System::terminal.palette_changed.connect(build_blocks);
    ox::System::terminal.palette_changed.connect(build_shapes);

    return result;
}

}  // namespace paint
