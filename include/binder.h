#ifndef BINDER_H
#define BINDER_H

#include <libweston/libweston.h>
#include <stdint.h>

struct weston_config;
struct weston_config_section;
extern struct weston_config* wet_get_config(struct weston_compositor* ec);
extern int weston_config_next_section(struct weston_config* config,
                                      struct weston_config_section** section, const char** name);
extern int weston_config_section_get_string(struct weston_config_section* section, const char* key,
                                            char** value, const char* default_value);

/* Expose Parsing Helper For Testing And Reuse. Returns 0 On Success, -1 On Failure */
int binder_parse_combination(const char* combo, uint32_t* key, enum weston_keyboard_modifier* mod);

/* Initialize Compatibility Layer If Needed. Returns 0 On Success. */
int binder_weston_compat_init(void);

/* Wrapper Functions To Centralize libweston API Differences. Implementations
 * Simply Forward To The libweston Symbols But Let Us Adapt Behavior Per
 * Version In One Place.
 */
struct weston_config* binder_wet_get_config(struct weston_compositor* ec);
int binder_weston_config_next_section(struct weston_config* config,
                                      struct weston_config_section** section, const char** name);
int binder_weston_config_section_get_string(struct weston_config_section* section, const char* key,
                                            char** value, const char* default_value);
struct weston_binding* binder_weston_compositor_add_key_binding(struct weston_compositor* ec,
                                                                unsigned k, unsigned m, void* cb,
                                                                void* data);
void binder_weston_log(const char* fmt, ...);

#endif /* BINDER_H */
