#ifndef _SPECIAL_ESCAPE_CODES
#define _SPECIAL_ESCAPE_CODES

#include <string>
#include <optional>

#include <uiohook.h>

struct SpecialKeyInfo {
    std::string escape_code;
    bool is_modifier = false;
};

std::optional<SpecialKeyInfo> get_special_keycode_info(uint16_t key);

#endif
