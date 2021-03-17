#include <locale.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <uiohook.h>
#include <wchar.h>

#include <json.hpp>

using json = nlohmann::json;

const char* special_key_to_escape_code(uint16_t key) {
    switch (key) {
        case VC_ENTER:
            return "<cr>";
        case VC_ESCAPE:
            return "<esc>";
        default:
            return NULL;
    }
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
void dispatch_proc(uiohook_event * const event) {
    char buffer[256] = { 0 };
    size_t length = 0;
    
    auto write_buffer_to_stdout = [&] {
        fprintf(stdout, "%s\n", buffer);
    };

    // Handle special keycodes
    if (event->type == EVENT_KEY_PRESSED) {
        const char* escape_code =
            special_key_to_escape_code(event->data.keyboard.keycode);

        if (escape_code) {
            length = snprintf(
                buffer + length, sizeof(buffer) - length, 
                escape_code
            );
            write_buffer_to_stdout(); return; // return to skip processing this keypress as normal key
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
            length = snprintf(buffer + length, sizeof(buffer) - length, 
                 "%lc",
                 event->data.keyboard.rawcode
            );
            write_buffer_to_stdout(); return;

        // Ignore everything else
        // TODO: Log error
        default:
            return;
    }
}

int main() {
    // Set locale
    setlocale(LC_CTYPE, "");

    // Set the logger callback for library output.
    hook_set_logger_proc(&logger_proc);
    
    // Set the event callback for uiohook events.
    hook_set_dispatch_proc(&dispatch_proc);

    // Start the hook and block.
    // NOTE If EVENT_HOOK_ENABLED was delivered, the status will always succeed.
    int status = hook_run();
    switch (status) {
        case UIOHOOK_SUCCESS:
            // Everything is ok.
            break;

        // System level errors.
        case UIOHOOK_ERROR_OUT_OF_MEMORY:
            logger_proc(LOG_LEVEL_ERROR, "Failed to allocate memory. (%#X)", status);
            break;


        // X11 specific errors.
        case UIOHOOK_ERROR_X_OPEN_DISPLAY:
            logger_proc(LOG_LEVEL_ERROR, "Failed to open X11 display. (%#X)", status);
            break;

        case UIOHOOK_ERROR_X_RECORD_NOT_FOUND:
            logger_proc(LOG_LEVEL_ERROR, "Unable to locate XRecord extension. (%#X)", status);
            break;

        case UIOHOOK_ERROR_X_RECORD_ALLOC_RANGE:
            logger_proc(LOG_LEVEL_ERROR, "Unable to allocate XRecord range. (%#X)", status);
            break;

        case UIOHOOK_ERROR_X_RECORD_CREATE_CONTEXT:
            logger_proc(LOG_LEVEL_ERROR, "Unable to allocate XRecord context. (%#X)", status);
            break;

        case UIOHOOK_ERROR_X_RECORD_ENABLE_CONTEXT:
            logger_proc(LOG_LEVEL_ERROR, "Failed to enable XRecord context. (%#X)", status);
            break;

            
        // Windows specific errors.
        case UIOHOOK_ERROR_SET_WINDOWS_HOOK_EX:
            logger_proc(LOG_LEVEL_ERROR, "Failed to register low level windows hook. (%#X)", status);
            break;


        // Darwin specific errors.
        case UIOHOOK_ERROR_AXAPI_DISABLED:
            logger_proc(LOG_LEVEL_ERROR, "Failed to enable access for assistive devices. (%#X)", status);
            break;

        case UIOHOOK_ERROR_CREATE_EVENT_PORT:
            logger_proc(LOG_LEVEL_ERROR, "Failed to create apple event port. (%#X)", status);
            break;

        case UIOHOOK_ERROR_CREATE_RUN_LOOP_SOURCE:
            logger_proc(LOG_LEVEL_ERROR, "Failed to create apple run loop source. (%#X)", status);
            break;

        case UIOHOOK_ERROR_GET_RUNLOOP:
            logger_proc(LOG_LEVEL_ERROR, "Failed to acquire apple run loop. (%#X)", status);
            break;

        case UIOHOOK_ERROR_CREATE_OBSERVER:
            logger_proc(LOG_LEVEL_ERROR, "Failed to create apple run loop observer. (%#X)", status);
            break;

        // Default error.
        case UIOHOOK_FAILURE:
        default:
            logger_proc(LOG_LEVEL_ERROR, "An unknown hook error occurred. (%#X)", status);
            break;
    }

    return status;
}
