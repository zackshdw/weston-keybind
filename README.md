# weston-keybind

Lightweight Weston Plugin That Binds Key Combinations To Commands. Designed For Easy Maintenance Across Multiple Weston Versions (13–14+) With A Clean Build System And Comprehensive Tests.

## Quick Start

### Install Current dev Dependency

```bash
sudo apt install libweston-14-dev libevdev-dev
```

### Build & Test (make)

```bash
make
make test
```

### Build With CMake (Out-Of-Source)

```bash
mkdir build && cd build
cmake ..
make
./tests/test_parse
```

### Install

```bash
sudo make install
```

## Keybind Example

Add Key Binding For `urxvt` Works With `CTRL` `ALT` `T`:

```ini
[core]
xwayland=true
binding-modifier=ctrl
modules=keybind.so

[shell]
panel-position=none
background-image=/home/fank/Pictures/1297444.jpg
background-type=scale-crop
cursor-size=24
cursor-theme=Adwaita

[autolaunch]
path=/home/fank/.urxvt/weston.sh

[libinput]
enable-tap=true

[keybind]
key=ctrl+alt+t
exec=urxvt
```

Notes:
- The `modules=keybind.so` Line Loads Keybind Plugin Into Weston.
- `binding-modifier` Sets The Modifier Used In Binding Definitions (Here `ctrl`).
- Place This File At `~/.config/weston.ini` (Create File If It Doesn't Exist).

## Project Tree

- **`src/`** — Plugin Source And Compatibility Layer:
  - `binder.c` — Main Plugin Entry Point; Reads Config And Binds Keys.
  - `parse.c` — Key Combo Parsing (Split For Reusability And Testability).
  - `weston_compat.c` — Centralized libweston API Wrappers For Version Differences.
- **`include/`** — Public Headers:
  - `binder.h` — Plugin Interface And Wrappers.
- **`tests/`** — Unit Tests:
  - `test_parse.c` — Tests For Key Parsing.
  - `weston_mocks.c` — Mock Weston Symbols So Tests Run Without A Compositor.
- **`Makefile`** — Traditional Build, Auto-Detects libweston Version.
- **`CMakeLists.txt`** — CMake Build Alternative.
- **`.clang-format`** — Code Style Configuration (Google-Based).
- **`.github/workflows/ci.yml`** — GitHub Actions CI (Format Checks, Build, Tests).

## Weston Version Support

### Supported Versions

- **Weston 14+** (Preferred If Available)
- **Weston 14** (Current)
- **Weston 13** (Fallback)

Both `Makefile` And `CMakeLists.txt` Automatically Detect Available Versions In Order (14+ → 14 → 13) And Set Compile-Time Defines (`-DHAVE_WESTON_15`, `-DHAVE_WESTON_14`, or `-DHAVE_WESTON_13`).

### Checking Detected Version

```bash
make check-deps
```

Output Example:
```
Weston Version: 14 (major: 14)
Compile Flags: -DHAVE_WESTON_14
```

### Adding Support For A New Weston Version (e.g., Weston 16)

#### 1. Update `Makefile`

Modify The Version Detection Chain (Highest First):

```makefile
LIBWESTON_MOD:=$(shell pkg-config --exists libweston-16 && echo libweston-16 || (pkg-config --exists libweston-15 && echo libweston-15 || (pkg-config --exists libweston-14 && echo libweston-14 || echo libweston-13)))
```

The `CFLAGS_WESTON_VER` Variable Will Automatically Set The Appropriate `-DHAVE_WESTON_*` Flag.

#### 2. Update `CMakeLists.txt`

Add A New Version Check And Update Selection Logic:

```cmake
pkg_check_modules(WESTON16 QUIET libweston-16)
# ... Existing Checks ...

if (WESTON16_FOUND)
    set(WESTON_PKG libweston-16)
    set(WESTON_MAJOR 16)
elseif (WESTON15_FOUND)
    # ... Existing Logic ...
endif()

add_compile_definitions($<$<BOOL:${WESTON16_FOUND}>:HAVE_WESTON_16>)
```

#### 3. Update `src/weston_compat.c` If APIs Changed

Add Version-Specific Behavior In Wrapper Functions:

```c
struct weston_config* binder_wet_get_config(struct weston_compositor* ec) {
#ifdef HAVE_WESTON_16
    /* Hypothetical: Weston 16 Renamed This Function. */
    return wet_compositor_get_config(ec);
#elif defined(HAVE_WESTON_15)
    return wet_get_config(ec);
#else
    return wet_get_config(ec);
#endif
}
```

Similarly, Update `binder_weston_compat_init()` With Version-Specific Initialization:

```c
int binder_weston_compat_init(void) {
#ifdef HAVE_WESTON_16
    /* Weston 16 Specific Init. */
    return 0;
#elif defined(HAVE_WESTON_15)
    /* Weston 15 Specific Init. */
    return 0;
#else
    /* Weston 13/14. */
    return 0;
#endif
}
```

#### 4. Update Forward Declarations In `include/binder.h` If Needed

If A Symbol Was Renamed:

```c
extern struct weston_config* wet_compositor_get_config(struct weston_compositor* ec);
    /* Weston 16+ */
extern struct weston_config* wet_get_config(struct weston_compositor* ec);
    /* Weston <16 */
```

### Design Rationale

- **Centralized Compatibility Layer:** All Version-Specific Code Lives In `src/weston_compat.c` And Wrapper Functions In `include/binder.h`. This Makes It Easy To Review And Update When Weston Changes.
- **Compile-Time Detection:** Using `#ifdef HAVE_WESTON_*` Means Zero Runtime Overhead For Version Checks.
- **Thin Wrappers:** The Wrappers Simply Forward To libweston Symbols, Adapting Only When APIs Diverge Between Versions.
- **Tests Run Everywhere:** Unit Tests Use Mocks And Don't Require A Running Weston Compositor, So They Work On Any System With libweston Headers.

## Code Style

Run `make format` To Apply `clang-format` To All Source And Header Files. The CI Checks Formatting On Each Push/PR.

```bash
make format
```

## Contributing

- Follow The Code Style (See `.clang-format`).
- Add Tests For New Parsing Logic Or Compatibility Wrappers.
- Update `src/weston_compat.c` If Adding Support For A New Weston Version.
- Run `make test` Locally Before Submitting PRs.
