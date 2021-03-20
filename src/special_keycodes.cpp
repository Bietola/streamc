#include <special_keycodes.h>

std::optional<SpecialKeyInfo> get_special_keycode_info(uint16_t key) {
    auto mk = [] (auto... args) {
        return std::optional<SpecialKeyInfo>{ SpecialKeyInfo { args... } };
    };

    switch (key) {
        case VC_ESCAPE:
            return mk(L"esc", false);

        /* SKIP All Function Keys */

        case VC_ENTER:
            return mk(L"cr", false);

        case VC_BACKSPACE:
            return mk(L"bs", false);

        case VC_TAB:
            return mk(L"tab", false);

        case VC_CAPS_LOCK:
            return mk(L"cps", false);

        case VC_PRINTSCREEN:
            return mk(L"stp", false);

        /* SKIP case VC_SCROLL_LOCK: */

        /* SKIP case VC_PAUSE */

        case VC_INSERT:
            return mk(L"ins", false);

        case VC_DELETE:
            return mk(L"del", false);

        case VC_HOME:
            return mk(L"hm", false);

        case VC_END:
            return mk(L"end", false);

        case VC_PAGE_UP:
            return mk(L"pup", false);

        case VC_PAGE_DOWN:
            return mk(L"pdn", false);

        case VC_UP:
            return mk(L"up", false);

        case VC_LEFT:
            return mk(L"lft", false);

        case VC_CLEAR:
            return mk(L"clr", false);

        case VC_RIGHT:
            return mk(L"rgt", false);

        case VC_DOWN:
            return mk(L"dwn", false);

        case VC_SHIFT_L:
            return mk(L"lsh", true);

        case VC_SHIFT_R:
            return mk(L"rsh", true);

        case VC_CONTROL_L:
            return mk(L"lcl", true);

        case VC_CONTROL_R:
            return mk(L"rcl", true);

        case VC_ALT_L:
            return mk(L"lal", true);

        case VC_ALT_R:
            return mk(L"ral", true);

        case VC_META_L:
            return mk(L"lsu", true);

        case VC_META_R:
            return mk(L"rsu", true);

        /* case VC_CONTEXT_MENU: */

        /* SKIP Rest of Them */

        default:
            return std::nullopt;
    }
}
