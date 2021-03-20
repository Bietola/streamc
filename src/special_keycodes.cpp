#include <special_keycodes.h>

std::optional<SpecialKeyInfo> get_special_keycode_info(uint16_t key) {
    auto mk = [] (auto... args) {
        return std::optional<SpecialKeyInfo>{ SpecialKeyInfo { args... } };
    };

    switch (key) {
        case VC_ESCAPE:
            return mk("<esc>", false);

        /* SKIP All Function Keys */

        case VC_ENTER:
            return mk("<cr>", false);

        case VC_BACKSPACE:
            return mk("<bs>", false);

        case VC_TAB:
            return mk("<tab>", false);

        case VC_CAPS_LOCK:
            return mk("<cps>", false);

        case VC_PRINTSCREEN:
            return mk("<stp>", false);

        /* SKIP case VC_SCROLL_LOCK: */

        /* SKIP case VC_PAUSE */

        case VC_INSERT:
            return mk("<ins>", false);

        case VC_DELETE:
            return mk("<del>", false);

        case VC_HOME:
            return mk("<hm>", false);

        case VC_END:
            return mk("<end>", false);

        case VC_PAGE_UP:
            return mk("<pup>", false);

        case VC_PAGE_DOWN:
            return mk("<pdn>", false);

        case VC_UP:
            return mk("<up>", false);

        case VC_LEFT:
            return mk("<lft>", false);

        case VC_CLEAR:
            return mk("<clr>", false);

        case VC_RIGHT:
            return mk("<rgt>", false);

        case VC_DOWN:
            return mk("<dwn>", false);

        case VC_SHIFT_L:
            return mk("<lsh>", true);

        case VC_SHIFT_R:
            return mk("<rsh>", true);

        case VC_CONTROL_L:
            return mk("<lcl>", true);

        case VC_CONTROL_R:
            return mk("<rcl>", true);

        case VC_ALT_L:
            return mk("<lal>", true);

        case VC_ALT_R:
            return mk("<ral>", true);

        case VC_META_L:
            return mk("<lsu>", true);

        case VC_META_R:
            return mk("<rsu>", true);

        /* case VC_CONTEXT_MENU: */

        /* SKIP Rest of Them */

        default:
            return std::nullopt;
    }
}
