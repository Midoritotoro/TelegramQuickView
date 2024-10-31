add_library(external_opus INTERFACE IMPORTED GLOBAL)
add_library(app::external_opus ALIAS external_opus)

find_package(PkgConfig REQUIRED)
pkg_check_modules(OPUS IMPORTED_TARGET opus)

target_link_libraries(external_opus INTERFACE PkgConfig::OPUS)

set(libs_loc "D:/Libraries/win64")

target_include_directories(external_opus SYSTEM
INTERFACE
    ${libs_loc}/opus/include
)

set(opus_lib_loc ${libs_loc}/opus/out)
set_target_properties(external_opus PROPERTIES
    IMPORTED_LOCATION "${opus_lib_loc}/Release/opus.lib"
    IMPORTED_LOCATION_DEBUG "${opus_lib_loc}/Debug/opus.lib"
)
