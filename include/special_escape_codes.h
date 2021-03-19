#include <uiohook.h>

const char* special_key_to_escape_code(uint16_t key) {
    switch (key) {
        case VC_ESCAPE:
            return "<esc>";

        /* SKIP All Function Keys */

        case VC_ENTER:
            return "<cr>";

        case VC_BACKSPACE:
            return "<bs>";

        case VC_TAB:
            return "<tab>";

        case VC_CAPS_LOCK:
            return "<cps>";

        case VC_PRINTSCREEN:
            return "<stp>";

        /* SKIP case VC_SCROLL_LOCK: */

        /* SKIP case VC_PAUSE */

        case VC_INSERT:
            return "<ins>";

        case VC_DELETE:
            return "<del>";

        case VC_HOME:
            return "<hm>";

        case VC_END:
            return "<end>";

        case VC_PAGE_UP:
            return "<pup>";

        case VC_PAGE_DOWN:
            return "<pdn>";

        case VC_UP:
            return "<up>";

        case VC_LEFT:
            return "<lft>";

        case VC_CLEAR:
            return "<clr>";

        case VC_RIGHT:
            return "<rgt>";

        case VC_DOWN:
            return "<dwn>";

        case VC_SHIFT_L:
            return "<lsh>";

        case VC_SHIFT_R:
            return "<rsh>";

        case VC_CONTROL_L:
            return "<lcl>";

        case VC_CONTROL_R:
            return "<rcl>";

        case VC_ALT_L:
            return "<lal>";

        case VC_ALT_R:
            return "<ral>";

        case VC_META_L:
            return "<lsu>";

        case VC_META_R:
            return "<rsu>";

        /* case VC_CONTEXT_MENU: */

        /* SKIP Rest of Them */

        default:
            return NULL;
    }
}

