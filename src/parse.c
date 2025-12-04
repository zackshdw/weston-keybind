#include <ctype.h>
#include <libevdev/libevdev.h>
#include <stdlib.h>
#include <string.h>

#include "binder.h"

/*
 * Parses A Key Combo To Keycode (linux/input-event-codes.h KEY_%) And Modifier
 * (libweston/libweston.h: Enum weston_keyboard_modifier).
 *
 * Returns -1 On Failure, 0 On Success.
 */
int binder_parse_combination(const char* combo, uint32_t* key, enum weston_keyboard_modifier* mod) {
    uint32_t k = 0;
    enum weston_keyboard_modifier m = 0;

    char keysym[29];
    int code;

    const char* i = combo;
    while (*i) {
        if (*i == '+') i++;

        const char* start = i;
        for (; *i && *i != '+'; i++);
        if (i - start > sizeof(keysym) - sizeof("KEY_")) return -1;

        strcpy(keysym, "KEY_");
        memcpy(keysym + 4, start, i - start);
        keysym[4 + i - start] = '\0';

        for (int j = 4; j < i - start + 4; j++) keysym[j] = toupper(keysym[j]);

        if (!strcmp("CTRL", keysym + 4)) {
            m |= MODIFIER_CTRL;
        } else if (!strcmp("ALT", keysym + 4)) {
            m |= MODIFIER_ALT;
        } else if (!strcmp("SUPER", keysym + 4)) {
            m |= MODIFIER_SUPER;
        } else if ((code = libevdev_event_code_from_name(EV_KEY, keysym))) {
            if (code == -1 || k != 0) return -1;

            k = code;
        } else {
            return -1;
        }
    }

    *key = k;
    *mod = m;
    return 0;
}
