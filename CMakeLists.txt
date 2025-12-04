cmake_minimum_required(VERSION 3.5)
project(weston-keybind C)

find_package(PkgConfig REQUIRED)

# Prefer libweston-15, Then 14, Then 13
pkg_check_modules(WESTON15 QUIET libweston-15)
pkg_check_modules(WESTON14 QUIET libweston-14)
pkg_check_modules(WESTON13 QUIET libweston-13)

if (WESTON15_FOUND)
    set(WESTON_PKG libweston-15)
    set(WESTON_MAJOR 15)
elseif (WESTON14_FOUND)
    set(WESTON_PKG libweston-14)
    set(WESTON_MAJOR 14)
else()
    set(WESTON_PKG libweston-13)
    set(WESTON_MAJOR 13)
endif()

pkg_check_modules(WESTON REQUIRED ${WESTON_PKG})
pkg_check_modules(LIBEVDEV REQUIRED libevdev)

add_compile_definitions($<$<BOOL:${WESTON15_FOUND}>:HAVE_WESTON_15>)
add_compile_definitions($<$<BOOL:${WESTON14_FOUND}>:HAVE_WESTON_14>)
add_compile_definitions($<$<BOOL:${WESTON13_FOUND}>:HAVE_WESTON_13>)
message(STATUS "Weston Version: ${WESTON_MAJOR}")

include_directories(${CMAKE_SOURCE_DIR}/include ${WESTON_INCLUDE_DIRS} ${LIBEVDEV_INCLUDE_DIRS})

add_library(binder SHARED src/binder.c src/parse.c src/weston_compat.c)
target_link_libraries(binder ${WESTON_LIBRARIES} ${LIBEVDEV_LIBRARIES})

add_executable(test_parse tests/test_parse.c tests/weston_mocks.c src/binder.c src/parse.c src/weston_compat.c)
target_include_directories(test_parse PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(test_parse ${WESTON_LIBRARIES} ${LIBEVDEV_LIBRARIES})
