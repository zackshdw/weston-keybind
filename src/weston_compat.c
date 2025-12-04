#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "binder.h"

/* Simple Compile-Time Compatibility Detection.
 * If `-DHAVE_WESTON_14` Is Defined (via pkg-config In The Build), We Can
 * Adapt Behavior Accordingly. This File Is A Place To Centralize Any
 * Differences Between libweston Versions.
 */

#include <dlfcn.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "binder.h"

/* Forward To Real libweston Symbols; Keep Wrappers To Centralize
 * Version-Specific Logic. On Test Builds, The Test Mocks Provide The
 * Underlying Symbols.
 */

int binder_weston_compat_init(void) {
#ifdef HAVE_WESTON_15
    /* Weston 15 Specific Init Here If Needed. */
    return 0;
#elif defined(HAVE_WESTON_14)
    /* Weston 14 Specific Init. */
    return 0;
#else
    /* Weston 13 Or Earlier. */
    return 0;
#endif
}

struct weston_config* binder_wet_get_config(struct weston_compositor* ec) {
    return wet_get_config(ec);
}

int binder_weston_config_next_section(struct weston_config* config,
                                      struct weston_config_section** section, const char** name) {
    return weston_config_next_section(config, section, name);
}

int binder_weston_config_section_get_string(struct weston_config_section* section, const char* key,
                                            char** value, const char* default_value) {
    return weston_config_section_get_string(section, key, value, default_value);
}

struct weston_binding* binder_weston_compositor_add_key_binding(struct weston_compositor* ec,
                                                                unsigned k, unsigned m, void* cb,
                                                                void* data) {
    return weston_compositor_add_key_binding(ec, k, m, cb, data);
}

void binder_weston_log(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char* buf = NULL;
    if (vasprintf(&buf, fmt, ap) != -1 && buf) {
        weston_log("%s", buf);
        free(buf);
    } else {
        weston_log(fmt);
    }
    va_end(ap);
}
