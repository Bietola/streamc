#ifndef _KEYMAP
#define _KEYMAP

#include <string>
#include <utility>
#include <vector>
#include <unordered_set>
#include <optional>

using KeySym = uint16_t;

namespace mod {
    // NB. The order of the listed items is reflected in an abbreviated mod string
    enum Mod {
        Shift = 1,
        Ctrl,
        Alt,
        Super
    };

    std::string tolower(const std::string& str);
    Mod from_str(const std::string& str);
    char to_abbr(Mod mod);
    Mod from_abbr(const std::string& str);
    Mod from_escape_code(const std::string& str);
    std::string to_abbr_str(std::vector<Mod> lst);
}

struct Key {
    uint16_t keysym = 0;
    std::unordered_set<mod::Mod> modifiers = {};
    std::optional<std::string> special_escape_code;

    const std::string to_dasher_code() const&;
};

struct Keymap {
    using MapPair = std::pair<Key, KeySym>;
    using Map = std::vector<MapPair>;

    const Map mappings;

    static Keymap from_config_file(const std::string& config_file);

};

#endif
