#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "binder.h"

static void run_test(const char* combo) {
    uint32_t key;
    enum weston_keyboard_modifier mod;
    int r = binder_parse_combination(combo, &key, &mod);
    if (r != 0) {
        fprintf(stderr, "FAIL: '%s' -> Parse Error\n", combo);
        exit(1);
    }
    printf("OK: '%s' -> key=%u mod=%u\n", combo, key, (unsigned)mod);
}

int main(void) {
    run_test("ctrl+alt+t");
    run_test("super+F1");
    run_test("alt+F4");
    printf("All Parse Tests Passed.\n");
    return 0;
}
