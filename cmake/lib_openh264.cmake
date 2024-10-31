add_library(external_openh264 INTERFACE IMPORTED GLOBAL)
add_library(app::external_openh264 ALIAS external_openh264)

find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBOPENH264 IMPORTED_TARGET)

target_link_libraries(external_openh264 INTERFACE PkgConfig::LIBOPENH264)

set(libs_loc "D:/Libraries/win64")

target_include_directories(external_openh264 SYSTEM
INTERFACE
    ${libs_loc}/local/include
)
set_target_properties(external_openh264 PROPERTIES
    IMPORTED_LOCATION "${libs_loc}/openh264/builddir-release/libopenh264.a"
    IMPORTED_LOCATION_DEBUG "${libs_loc}/openh264/builddir-debug/libopenh264.a"
)