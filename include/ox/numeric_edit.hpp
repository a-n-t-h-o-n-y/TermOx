#pragma once
#include <cmath>
#include <format>
#include <limits>
#include <locale>
#include <ox/label.hpp>
#include <ox/lineedit.hpp>
#include <ox/selection.hpp>
#include <ox/value_range.hpp>
#include <sstream>

namespace ox {
/** Typed numeric input. Draft text is distinct from the committed value. */
template <Numeric T>
class BasicNumericEdit : public Widget {
   public:
    using Widget::handle;
    using value_type = T;
    using Range = BasicValueRange<T>;
    struct Options {
        Range range = {};
        T value = 0;
        std::string format = "{}";
        std::string prefix = {};
        std::string suffix = {};
        std::locale locale = std::locale::classic();
        bool commit_on_focus_loss = false;
        LineEdit::Options editor = {};
    };
    /** LineEdit with focus notifications, used to switch display/edit presentation. */
    class Editor : public LineEdit {
       public:
        explicit Editor(LineEdit::Options x) : LineEdit{std::move(x)} {}
        sl::Signal<void(bool)> on_focus;
        auto focus_in() -> EventResponse override
        {
            auto result = LineEdit::focus_in();
            on_focus(true);
            return result;
        }
        auto focus_out() -> EventResponse override
        {
            auto result = LineEdit::focus_out();
            on_focus(false);
            return result;
        }
    };
    Editor editor;
    std::string format;
    std::string prefix;
    std::string suffix;
    std::locale locale;
    bool commit_on_focus_loss;
    sl::Signal<void(T)> on_change;
    sl::Signal<void(std::string const&)> on_invalid;

    BasicNumericEdit() : BasicNumericEdit{Options{}} {}
    explicit BasicNumericEdit(Options x)
        : Widget{FocusPolicy::None, SizePolicy::flex(), SizePolicy::fixed(1)},
          editor{std::move(x.editor)},
          format{std::move(x.format)},
          prefix{std::move(x.prefix)},
          suffix{std::move(x.suffix)},
          locale{std::move(x.locale)},
          commit_on_focus_loss{x.commit_on_focus_loss},
          range_{x.range},
          value_{checked_value(x.value, x.range)}
    {
        validate_row_text(prefix);
        validate_row_text(suffix);
        sync();
        Connection{.signal = editor.on_change,
                   .slot = [](auto const&, BasicNumericEdit& self) {
                       if (!self.syncing_) self.draft_ = true;
                   }}(*this);
        Connection{.signal = editor.on_submit,
                   .slot = [](auto const&, BasicNumericEdit& self) {
                       self.commit_draft(false);
                   }}(*this);
        Connection{.signal = editor.on_invalid,
                   .slot = [](std::string const& text, BasicNumericEdit& self) {
                       self.on_invalid(text);
                   }}(*this);
        Connection{.signal = editor.on_focus,
                   .slot = [](bool focused, BasicNumericEdit& self) {
                       if (!focused && self.commit_on_focus_loss && self.draft_)
                           self.commit();
                       else if (!self.draft_)
                           self.sync();
                   }}(*this);
        auto lifetime = track(*this);
        editor.shortcuts = [lifetime](KeyEvent const& e) {
            auto* self = lifetime.try_get();
            return self && self->editor.is_focused() ? self->handle(e)
                                                     : EventResponse{};
        };
    }
    auto range() const -> Range { return range_; }
    auto value() const -> T { return value_; }
    auto has_draft() const -> bool { return draft_; }
    auto set_value(T v) -> bool
    {
        v = checked_value(v, range_);
        // Prepare formatting before changing the committed value.
        auto text = presentation(v);
        auto changed = std::exchange(value_, v) != v;
        auto lifetime = track(*this);
        sync(std::move(text));
        if (auto* self = lifetime.try_get(); self && changed && self->value_ == v)
            self->on_change(v);
        return changed;
    }
    auto set_range(Range r) -> bool
    {
        check_range(r);
        if (r == range_) return false;
        range_ = r;
        set_value(value_);
        return true;
    }
    /** Returns validity, including an unchanged value; invalid input stays visible. */
    auto commit() -> bool { return commit_draft(true); }
    void revert() { sync(); }
    void refresh() override
    {
        validate_row_text(prefix);
        validate_row_text(suffix);
        auto lifetime = track(*this);
        if (!draft_) sync();
        if (auto* self = lifetime.try_get()) {
            self->Widget::refresh();
            self->on_arrange(self->bounds());
        }
    }
    auto measure(Area a, esc::WidthPolicy p = {}) const -> Area override
    {
        validate(a);
        auto [left, right] = affixes(a, p);
        auto result =
            editor.measure({std::max(0, a.width - left - right), a.height}, p);
        result.width = (int)std::min<std::int64_t>(
            a.width, (std::int64_t)result.width + left + right);
        return result;
    }
    auto paint(Canvas c) -> Cursor override
    {
        auto [left, right] = affixes(c.size(), c.width_policy());
        auto style = appearance(editor.style);
        paint_label(c.subcanvas({}, {left, c.size().height}), prefix, Align::Left,
                    style);
        paint_label(c.subcanvas({c.size().width - right, 0}, {right, c.size().height}),
                    suffix, Align::Right, style);
        return {};
    }
    auto get_children() -> zzz::Generator<Widget&> override { co_yield editor; }
    auto get_children() const -> zzz::Generator<Widget const&> override
    { co_yield editor; }
    auto handle(KeyEvent const& e) -> EventResponse override
    {
        if (!is_enabled() || editor.read_only || e.action == KeyAction::Release)
            return {};
        if (e.matches(KeyCode::Escape, {}, KeyAction::Press)) {
            revert();
            return {.handled = true, .repaint = true};
        }
        if (e.matches(KeyCode::ArrowUp, {}, e.action) ||
            e.matches(KeyCode::ArrowDown, {}, e.action)) {
            return {.handled = true,
                    .repaint =
                        set_value(stepped(e.matches(KeyCode::ArrowUp, {}, e.action)))};
        }
        return {};
    }

   protected:
    void on_arrange(Rect) override
    {
        auto [left, right] = affixes(size(), glyph_width_policy());
        arrange(editor, {{left, 0}, {size().width - left - right, size().height}});
    }

   private:
    auto commit_draft(bool validate_editor) -> bool
    {
        if (!draft_) return true;  // Never parse a rounded display value.
        auto text = editor.text();
        auto lifetime = track(*this);
        auto check = editor.validator;
        auto valid = !validate_editor || !check || check(text);
        auto* self = lifetime.try_get();
        if (!self) return false;
        auto parsed = valid ? self->parse(text) : std::optional<T>{};
        if (!parsed) {
            self->on_invalid(text);
            return false;
        }
        self->set_value(*parsed);
        return lifetime.valid();
    }
    Range range_;
    T value_;
    bool draft_ = false;
    bool syncing_ = false;
    static void check_range(Range r)
    {
        if (r.minimum > r.maximum || r.step <= 0)
            throw std::invalid_argument{"Invalid numeric range"};
        if constexpr (std::floating_point<T>) {
            if (!std::isfinite(r.minimum) || !std::isfinite(r.maximum) ||
                !std::isfinite(r.step))
                throw std::invalid_argument{"Numeric range must be finite"};
        }
    }
    static auto checked_value(T v, Range r) -> T
    {
        check_range(r);
        if constexpr (std::floating_point<T>) {
            if (!std::isfinite(v)) throw std::invalid_argument{"Value must be finite"};
        }
        return std::clamp(v, r.minimum, r.maximum);
    }
    auto presentation(T v) const -> std::string
    {
        if (!editor.is_focused()) {
            auto result = std::vformat(locale, format, std::make_format_args(v));
            validate_row_text(result);
            return result;
        }
        // Default floating formatting uses the shortest round-trip representation.
        // L applies the same locale that parse() uses, including decimal separators.
        return std::vformat(locale, "{:L}", std::make_format_args(v));
    }
    void sync() { sync(presentation(value_)); }
    void sync(std::string text)
    {
        auto lifetime = track(*this);
        draft_ = false;
        syncing_ = true;
        try {
            editor.set_text(text);
        }
        catch (...) {
            if (auto* self = lifetime.try_get()) self->syncing_ = false;
            throw;
        }
        if (auto* self = lifetime.try_get()) {
            self->syncing_ = false;
            self->request_layout();
            self->request_repaint();
        }
    }
    auto parse(std::string const& text) const -> std::optional<T>
    {
        auto input = std::istringstream{text};
        input.imbue(locale);
        input >> std::noskipws;
        if constexpr (std::integral<T>) {
            if constexpr (std::unsigned_integral<T>) {
                if (text.find('-') != std::string::npos) return {};
            }
            using Wide = std::conditional_t<std::signed_integral<T>, std::intmax_t,
                                            std::uintmax_t>;
            Wide v{};
            input >> v;
            if (!input || !input.eof() || v < std::numeric_limits<T>::lowest() ||
                v > std::numeric_limits<T>::max())
                return {};
            auto result = static_cast<T>(v);
            if (result < range_.minimum || result > range_.maximum) return {};
            return result;
        }
        else {
            T v{};
            input >> v;
            if (!input || !input.eof() || !std::isfinite(v) || v < range_.minimum ||
                v > range_.maximum)
                return {};
            return v;
        }
    }
    auto stepped(bool up) const -> T
    {
        if constexpr (std::integral<T>) {
            // Unsigned subtraction gives an exact distance even across signed zero.
            using U = std::make_unsigned_t<T>;
            auto distance = up ? U(U(range_.maximum) - U(value_))
                               : U(U(value_) - U(range_.minimum));
            if (U(range_.step) >= distance) return up ? range_.maximum : range_.minimum;
            return up ? T(value_ + range_.step) : T(value_ - range_.step);
        }
        else {
            auto next = (long double)value_ +
                        (up ? (long double)range_.step : -(long double)range_.step);
            return (T)std::clamp(next, (long double)range_.minimum,
                                 (long double)range_.maximum);
        }
    }
    auto affixes(Area a, esc::WidthPolicy p) const -> std::pair<int, int>
    {
        auto left = measure_text(prefix, a, TextWrap::None, p).width;
        left = std::clamp(left, 0, a.width);
        auto right = measure_text(suffix, a, TextWrap::None, p).width;
        return {left, std::clamp(right, 0, a.width - left)};
    }
};
using NumericEdit = BasicNumericEdit<double>;
using IntegerEdit = BasicNumericEdit<int>;
}  // namespace ox
