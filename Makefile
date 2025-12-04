# Detect libweston Version (Prefer 14+, Then 14, Then 13)
LIBWESTON_MOD:=$(shell pkg-config --exists libweston-15 && echo libweston-15 || (pkg-config --exists libweston-14 && echo libweston-14 || echo libweston-13))
WESTON_VERSION:=$(shell echo $(LIBWESTON_MOD) | sed 's/libweston-//')
WESTON_MAJOR:=$(shell echo $(WESTON_VERSION) | cut -d. -f1)
CFLAGS_WESTON_VER:=$(shell [ "$(WESTON_MAJOR)" = "15" ] && echo "-DHAVE_WESTON_15" || ([ "$(WESTON_MAJOR)" = "14" ] && echo "-DHAVE_WESTON_14" || echo "-DHAVE_WESTON_13"))

SRC_DIR:=src
INC_DIR:=include
TEST_DIR:=tests

# Build Flags - Using Proper pkg-config
PKG_CFLAGS:=$(shell pkg-config --cflags $(LIBWESTON_MOD) libevdev 2>/dev/null)
PKG_LIBS:=$(shell pkg-config --libs $(LIBWESTON_MOD) libevdev 2>/dev/null)

CFLAGS:=$(PKG_CFLAGS) -fPIC -Wall $(CFLAGS_WESTON_VER)
LDFLAGS:=-shared $(PKG_LIBS)

.PHONY: all clean install uninstall check-deps test

all: check-deps keybind.so

check-deps:
	@echo "Checking Dependencies..."
	@pkg-config --exists libweston-15 || pkg-config --exists libweston-14 || pkg-config --exists libweston-13 || (echo "Error: libweston Development Files Not Found. Install One Of: libweston-15-0, libweston-14-0, or libweston-13-0" && false)
	@pkg-config --exists libevdev || (echo "Error: libevdev Not Found. Install With: sudo apt install libevdev-dev" && false)
	@echo "Dependencies OK"
	@echo "Weston Version: $(WESTON_VERSION) (major: $(WESTON_MAJOR))"
	@echo "Compile Flags: $(CFLAGS_WESTON_VER)"
	@echo "Module Prefix: $(shell pkg-config --variable=libdir $(LIBWESTON_MOD) 2>/dev/null || echo /usr/lib/$(shell gcc -dumpmachine))"

keybind.so: $(SRC_DIR)/binder.c $(SRC_DIR)/parse.c $(SRC_DIR)/weston_compat.c | Makefile
	gcc ${CFLAGS} -I$(INC_DIR) $(SRC_DIR)/binder.c $(SRC_DIR)/parse.c $(SRC_DIR)/weston_compat.c ${LDFLAGS} -o $@

format:
	@if ! command -v clang-format >/dev/null 2>&1; then \
		echo "clang-format Not Found. Install It (e.g. sudo apt install clang-format) Or Run Format Manually."; \
		exit 1; \
	fi; \
	clang-format -i $(shell find src include tests -name "*.c" -o -name "*.h")
	@git diff --exit-code || (echo "Code Is Not Formatted. Run 'make format' To Fix." && false)

format-check:
	@echo "Checking Code Formatting With clang-format..."
	@if ! command -v clang-format >/dev/null 2>&1; then \
		echo "clang-format Not Found. Skipping Format Check. Install clang-format To Enforce Formatting."; \
		exit 0; \
	fi; \
	@clang-format -i $(shell find src include tests -name "*.c" -o -name "*.h") || true
	@git diff --exit-code || (echo "Code Is Not Formatted. Run 'make format' To Fix." && false)
	gcc ${CFLAGS} -I$(INC_DIR) $(TEST_DIR)/test_parse.c $(SRC_DIR)/binder.c $(SRC_DIR)/parse.c $(SRC_DIR)/weston_compat.c $(TEST_DIR)/weston_mocks.c -o $@ ${PKG_LIBS}

clean:
	rm -f keybind.so $(TEST_DIR)/test_parse format-check

install: keybind.so
	install -d $(DESTDIR)$(shell pkg-config --variable=libdir $(LIBWESTON_MOD) 2>/dev/null || echo /usr/lib/$(shell gcc -dumpmachine))/weston
	install -m 755 keybind.so $(DESTDIR)$(shell pkg-config --variable=libdir $(LIBWESTON_MOD) 2>/dev/null || echo /usr/lib/$(shell gcc -dumpmachine))/weston

uninstall:
	rm -f $(DESTDIR)$(shell pkg-config --variable=libdir $(LIBWESTON_MOD) 2>/dev/null || echo /usr/lib/$(shell gcc -dumpmachine))/weston/keybind.so
