#include "binder.h"

#include <ctype.h>
#include <fcntl.h>
#include <libevdev/libevdev.h>
#include <libweston/libweston.h>
#include <libweston/weston-log.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static pid_t system_nowait(const char* file, char* const argv[]) {
    pid_t pid = fork();

    if (pid) return pid;

    int fd = open("/dev/null", O_WRONLY);

    dup2(fd, 1);
    dup2(fd, 2);

    close(fd);

    execvp(file, argv);
    exit(1);
}

static void binder_callback(struct weston_keyboard* keyboard, const struct timespec* time,
                            uint32_t key, void* data) {
    pid_t spawn = system_nowait("sh", (char* const[]){"sh", "-c", data, NULL});
    if (spawn == -1) {
        binder_weston_log("Failed Spawning Process %s\n", (char*)data);
        return;
    }
}

static void binder_add_bindings(struct weston_compositor* ec) {
    struct weston_config* config = binder_wet_get_config(ec);
    struct weston_config_section* s = NULL;
    const char* name = NULL;
    char *exec = NULL, *key = NULL;

    while (binder_weston_config_next_section(config, &s, &name)) {
        if (strcmp(name, "keybind")) continue;

        binder_weston_config_section_get_string(s, "key", &key, NULL);
        binder_weston_config_section_get_string(s, "exec", &exec, NULL);

        if (!key || !exec) {
            free(key);
            free(exec);

            binder_weston_log("Ignoring Incomplete Keybind Section.\n");
            continue;
        }

        uint32_t k;
        enum weston_keyboard_modifier m;
        int code = binder_parse_combination(key, &k, &m);

        if (code == -1) {
            binder_weston_log("Invalid Key For Keybind: '%s'.", key);
            free(key);
            free(exec);
            continue;
        }

        binder_weston_log("Adding Keybind %s -> %s\n", key, exec);
        binder_weston_compositor_add_key_binding(ec, k, m, binder_callback, exec);
        free(key);
        /* Note: exec Is NOT freed - It's Used By The Callback */
    }
}

WL_EXPORT int wet_module_init(struct weston_compositor* ec, int* argc, char* argv[]) {
    (void)argc;
    (void)argv;
    binder_weston_compat_init();
    binder_add_bindings(ec);
    return 0;
}
