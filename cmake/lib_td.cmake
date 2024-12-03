add_library(external_td INTERFACE IMPORTED GLOBAL)
add_library(app::external_td ALIAS external_td)

set(libs_loc "D:/Libraries/win64")
set(Td_DIR ${libs_loc}/td/tdlib/lib)

target_include_directories(external_td SYSTEM
INTERFACE
    ${libs_loc}/td/tdlib/include
)

target_link_libraries(external_td
INTERFACE
    ${libs_loc}/td/build/$<IF:$<CONFIG:Debug>,Debug,Release>/tdclient.lib
)