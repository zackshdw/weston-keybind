#include <stdarg.h>
#include <stdio.h>

struct weston_compositor {
    int dummy;
};
struct weston_config {
    int dummy;
};
struct weston_config_section {
    int dummy;
};

struct weston_config* wet_get_config(struct weston_compositor* ec) {
    (void)ec;
    return NULL;
}

int weston_config_next_section(struct weston_config* config, struct weston_config_section** section,
                               const char** name) {
    (void)config;
    (void)section;
    (void)name;
    return 0;
}

int weston_config_section_get_string(struct weston_config_section* section, const char* key,
                                     char** value, const char* default_value) {
    (void)section;
    (void)key;
    (void)value;
    (void)default_value;
    return 0;
}

int weston_compositor_add_key_binding(struct weston_compositor* ec, unsigned k, unsigned m,
                                      void* cb, void* data) {
    (void)ec;
    (void)k;
    (void)m;
    (void)cb;
    (void)data;
    return 0;
}

void weston_log(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}
