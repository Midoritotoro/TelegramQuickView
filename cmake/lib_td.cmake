add_library(external_td STATIC IMPORTED GLOBAL)
add_library(app::external_td ALIAS external_td)

set(libs_loc "D:/Libraries/win64")

target_include_directories(external_td SYSTEM
INTERFACE
    ${libs_loc}/td/tdlib/include
)

set_target_properties(external_td PROPERTIES
    IMPORTED_LOCATION ${libs_loc}/td/build/Release/tdjson.lib  ${libs_loc}/td/build/Release/tdjson_static.lib
    IMPORTED_LOCATION_DEBUG ${libs_loc}/td/build/Release/tdjson.lib  ${libs_loc}/td/build/Release/tdjson_static.lib
)