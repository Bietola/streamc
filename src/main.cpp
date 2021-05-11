extern "C" {
    #include <inttypes.h>
    #include <stdarg.h>
    #include <stdbool.h>
    #include <stdio.h>
    #include <string.h>
    #include <uiohook.h>
    #include <wchar.h>
    
    // TODO: Use more popular utf8 library
    #include <rosetta_utf8.h>
}

#include <locale>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <unordered_set>

#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include <special_keycodes.h>
#include <keymap.hpp>

using json = nlohmann::json;

auto parse_modifiers_from_keymask(uint16_t keymask) {
    std::unordered_set<mod::Mod> res;

    if (keymask & uint16_t(MASK_SHIFT)) {
        res.insert(mod::Shift);
    }
    if (keymask & uint16_t(MASK_CTRL)) {
        res.insert(mod::Ctrl);
    }
    if (keymask & uint16_t(MASK_ALT)) {
        res.insert(mod::Alt);
    }
    if (keymask & uint16_t(MASK_META)) {
        res.insert(mod::Super);
    }

    return res;
}

std::vector<Key> extract_real_key_combination(
        const Key& keypress,
        const Keymap& keymap
    ) {
    std::unordered_set<mod::Mod> mods;

    // auto mappings = keymap.mappings_for(keypress);
    auto mappings = std::vector<Key> {};

    std::vector<Key> possible_results;
    for (const auto& mping : mappings) {
        auto kmap_mods = mping.modifiers;

        auto diff1 = std::unordered_set<mod::Mod> {};
        /* auto diff1 = std::set_difference( */
        /*     mods, */
        /*     kmap_mods */
        /* ); */
        /* auto diff2 = std::set_difference( */
        /*     kmap_mods, */
        /*     mods */
        /* ); */

        // TODO: Report error below
        /* if (!diff2.empty()) { */
        /*     std::cerr << fmt::format( */
        /*         "WARNING: Keypress {} doesn't reflect any keymap configuration {}\n", */
        /*         keypress, keymap */
        /*     ); */
        /* } */

        possible_results.push_back(
            Key {
                .keysym = keypress.keysym,
                .modifiers = diff1
            }
        );
    }

    return possible_results;
}

bool logger_proc(unsigned int level, const char *format, ...) {
    return true;
}

// NOTE: The following callback executes on the same thread that hook_run() is called 
// from.  This is important because hook_run() attaches to the operating systems
// event dispatcher and may delay event delivery to the target application.
// Furthermore, some operating systems may choose to disable your hook if it 
// takes too long to process.  If you need to do any extended processing, please 
// do so by copying the event to your own queued dispatch thread.
auto make_dispatch_proc(bool json_mode) {
    // TODO: Fix json_mode switch (ignored for now since lambda can't
    //       decay to funptrs if they capture stuff).
    return [/*json_mode*/] (uiohook_event * const event) {
        Key res;
        
        // press mod -> (mods<-)
        // press a key w/ mod -> (mods->)
        // rel mod            -> (mods->)
        std::unordered_set<mod::Mod> curr_pressed_mods;

        auto write_res_to_stdout = [/*json_mode,*/ &] {
            /* if (json_mode) { */
            /*     std::cout << res << '\n'; */
            /* } else { */
                std::cout << res.to_dasher_code() << '\n';
            /* } */
        };
    
        // Handle special keycodes
        std::optional<std::string> special_escape_code = std::nullopt;
        if (event->type == EVENT_KEY_PRESSED) {
            // If the shift+alt+c key combination is pressed, naturally terminate the program.
            if (event->data.keyboard.keycode == VC_C &&
                    event->mask & uint16_t(MASK_SHIFT) &&
                    event->mask & uint16_t(MASK_ALT)) {
                int status = hook_stop();
                switch (status) {
                    case UIOHOOK_SUCCESS:
                        // Everything is ok.
                        break;

                    // System level errors.
                    case UIOHOOK_ERROR_OUT_OF_MEMORY:
                        logger_proc(LOG_LEVEL_ERROR, "Failed to allocate memory. (%#X)", status);
                        break;
            
                    case UIOHOOK_ERROR_X_RECORD_GET_CONTEXT:
                        // NOTE This is the only platform specific error that occurs on hook_stop().
                        logger_proc(LOG_LEVEL_ERROR, "Failed to get XRecord context. (%#X)", status);
                        break;

                    // Default error.
                    case UIOHOOK_FAILURE:
                    default:
                        logger_proc(LOG_LEVEL_ERROR, "An unknown hook error occurred. (%#X)", status);
                        break;
                }
            }

            auto maybe_keyinfo =
                get_special_keycode_info(event->data.keyboard.keycode);

            if (maybe_keyinfo.has_value()) {
                auto keyinfo = maybe_keyinfo.value();

                if (!keyinfo.is_modifier) {
                    const auto modifiers = parse_modifiers_from_keymask(event->mask);

                    // Currently pressed mods from the event mask are not recorded separately
                    {
                        std::unordered_set<mod::Mod> mods_in_common = {};

                        // Find currently pressed mods which are part of the mod mask
                        std::set_intersection(
                            modifiers.begin(), modifiers.end(),
                            curr_pressed_mods.begin(), curr_pressed_mods.end(),
                            std::inserter(mods_in_common, mods_in_common.begin())
                        );

                        // Remove them so as to not record them later
                        for (const auto& mod : mods_in_common) {
                            curr_pressed_mods.erase(
                                std::remove_if(
                                    curr_pressed_mods.begin(), curr_pressed_mods.end(),
                                    [&] (const auto ele) {
                                        return mods_in_common.find(ele) != mods_in_common.end();
                                    }
                                ),
                                curr_pressed_mods.end()
                            );
                        }
                    }

                    res.keysym = event->data.keyboard.rawcode;
                    res.modifiers = parse_modifiers_from_keymask(event->mask);
                    res.special_escape_code = std::optional<std::string> { keyinfo.escape_code };

                    write_res_to_stdout(); return;
                } else {
                    // Ignore modifiers as they are already prefixed to other keys
                    // TODO: Give option to not ignore
                    curr_pressed_mods.insert(mod::from_escape_code(keyinfo.escape_code));

                    write_res_to_stdout(); return;
                }
            } else {
                // Continue since key presses can be normal keys
                ;
            }
        }

        switch (event->type) {
            // Ignore release events
            case EVENT_KEY_RELEASED:
                /* Ignore */ return;

            // Handle "normal" or "alphanumerical" keypresses
            case EVENT_KEY_TYPED:
                res.modifiers = parse_modifiers_from_keymask(event->mask);

                // TODO: Research more secure way to convert `uint16_t` to `std::string`
                res.keysym = event->data.keyboard.rawcode;

                write_res_to_stdout(); return;
            // Ignore everything else
            // TODO: Log error
            default:
                return;
        }
    };
}

int main(int argc, char** argv) {
    auto keys = Keymap::from_config_file(std::string(argv[0]));

    /* bool json_mode = false; */
    /* if (argc > 2 && strcmp(argv[1], "--json") == 0) { */
    /*     std::cerr << "json mode is broken!" << std::endl; // TODO/WIP */
    /*     json_mode = true; */
    /* } */

    /* // set locale */
    /* setlocale(LC_ALL, ""); */

    /* // Set the logger callback for library output. */
    /* hook_set_logger_proc(&logger_proc); */
    
    /* // Set the event callback for uiohook events. */
    /* hook_set_dispatch_proc(make_dispatch_proc(json_mode)); */

    /* // Start the hook and block. */
    /* // NOTE If EVENT_HOOK_ENABLED was delivered, the status will always succeed. */
    /* int status = hook_run(); */
    /* switch (status) { */
    /*     case UIOHOOK_SUCCESS: */
    /*         // Everything is ok. */
    /*         break; */

    /*     // System level errors. */
    /*     case UIOHOOK_ERROR_OUT_OF_MEMORY: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Failed to allocate memory. (%#X)", status); */
    /*         break; */


    /*     // X11 specific errors. */
    /*     case UIOHOOK_ERROR_X_OPEN_DISPLAY: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Failed to open X11 display. (%#X)", status); */
    /*         break; */

    /*     case UIOHOOK_ERROR_X_RECORD_NOT_FOUND: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Unable to locate XRecord extension. (%#X)", status); */
    /*         break; */

    /*     case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Unable to allocate XRecord range. (%#X)", status); */
    /*         break; */

    /*     case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Unable to allocate XRecord context. (%#X)", status); */
    /*         break; */

    /*     case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Failed to enable XRecord context. (%#X)", status); */
    /*         break; */

            
    /*     // Windows specific errors. */
    /*     case UIOHOOK_ERROR_SET_WINDOWS_HOOK_EX: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Failed to register low level windows hook. (%#X)", status); */
    /*         break; */


    /*     // Darwin specific errors. */
    /*     case UIOHOOK_ERROR_AXAPI_DISABLED: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Failed to enable access for assistive devices. (%#X)", status); */
    /*         break; */

    /*     case UIOHOOK_ERROR_CREATE_EVENT_PORT: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Failed to create apple event port. (%#X)", status); */
    /*         break; */

    /*     case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Failed to create apple run loop source. (%#X)", status); */
    /*         break; */

    /*     case UIOHOOK_ERROR_GET_RUNLOOP: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Failed to acquire apple run loop. (%#X)", status); */
    /*         break; */

    /*     case UIOHOOK_ERROR_CREATE_OBSERVER: */
    /*         logger_proc(LOG_LEVEL_ERROR, "Failed to create apple run loop observer. (%#X)", status); */
    /*         break; */

    /*     // Default error. */
    /*     case UIOHOOK_FAILURE: */
    /*     default: */
    /*         logger_proc(LOG_LEVEL_ERROR, "An unknown hook error occurred. (%#X)", status); */
    /*         break; */
    /* } */

    /* return status; */
}
