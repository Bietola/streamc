#include <keymap.hpp>

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <rosetta_utf8.h>
#include <unordered_set>

using KeySym = uint16_t;

namespace mod {
    std::string tolower(const std::string& str) {
        std::string res;
        for (char c : str) {
            res.push_back(std::tolower(c));
        }
        return res;
    }

    Mod from_str(const std::string& str) {
        if (tolower(str) == "shift") {
            return Shift;
        } else if (tolower(str) == "ctrl") {
            return Ctrl;
        } else if (tolower(str) == "alt") {
            return Alt;
        } else if (tolower(str) == "super") {
            return Super;
        } else {
            std::cerr << "Malformed modifier: " << str << std::endl;
            assert(false);
        }
    }

    Mod from_abbr(const std::string& str) {
        if (tolower(str) == "s") {
            return Shift;
        } else if (tolower(str) == "c") {
            return Ctrl;
        } else if (tolower(str) == "a") {
            return Alt;
        } else if (tolower(str) == "m") {
            return Super;
        } else {
            std::cerr << "Malformed abbreviation: " << str << std::endl;
            assert(false);
        }
    }

    Mod from_escape_code(const std::string& str) {
        if (tolower(str) == "shf") {
            return Shift;
        } else if (tolower(str) == "c") {
            return Ctrl;
        } else if (tolower(str) == "a") {
            return Alt;
        } else if (tolower(str) == "m") {
            return Super;
        } else {
            std::cerr << "Malformed abbreviation: " << str << std::endl;
            assert(false);
        }
    }

    char to_abbr(Mod mod) {
        switch (mod) {
            case mod::Shift:
                return 's';
            case mod::Ctrl:
                return 'c';
            case mod::Alt:
                return 'a';
            case mod::Super:
                return 'm';
            default:
                assert(false);
        }
    };

    std::string to_abbr_str(std::unordered_set<Mod> lst) {
        // NB. Exploits enum to int implicit conversion
        std::sort(lst.begin(), lst.end());

        std::string res;
        for (const auto& ele : lst) {
            res.push_back(to_abbr(ele));
        }
        return res;
    }
}

const std::string Key::to_dasher_code() const& {
    bool close_mod = false;
    std::ostringstream ss;

    auto is_special = special_escape_code.has_value();

    // Construct modifier prefix
    auto mod_prefix = mod::to_abbr_str(modifiers);
    if (!mod_prefix.empty()) {
        close_mod = true;
        ss << '<' << mod_prefix;
        
        if (is_special) {
            ss << '+';
        } else {
            ss << '-';
        }
    } else if (is_special) {
        close_mod = true;
        ss << '<';
    }

    if (is_special) {
        ss << special_escape_code.value();
    } else {
        // Use more robust/popular utf8 library
        ss << to_utf8(keysym);
    }

    if (close_mod) {
        ss << '>';
    }

    return ss.str();
};

Keymap Keymap::from_config_file(const std::string& config_file) {
    Map mappings = {};

    auto fi = std::ifstream(config_file);

    std::string line;
    while (std::getline(fi, line)) {
        std::stringstream ss;
        ss << line;

        // Parse line into keymap entry
        std::unordered_set<mod::Mod> kcomb_mods = {};
        std::string kcomb_sym;
        std::string resulting_keysym;
        while (true) {
            std::string next;
            ss >> next;

            if (next == ":") {
                ss >> kcomb_sym;
                ss >> resulting_keysym;
                break;
            } else {
                kcomb_mods.insert(mod::from_abbr(next));
            }
        }

        auto key_comb = Key {
            // TODO/DANGER: Check if this actually works...
            .keysym = uint16_t(to_cp(kcomb_sym.c_str())),

            .modifiers = kcomb_mods
        };

        mappings.push_back(MapPair{ 
            key_comb,
            uint16_t(to_cp(resulting_keysym.c_str()))
        });
    }

    return Keymap {
        .mappings = mappings
    };
}
