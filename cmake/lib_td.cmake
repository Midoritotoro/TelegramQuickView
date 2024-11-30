add_library(external_td INTERFACE IMPORTED GLOBAL)
add_library(app::external_td ALIAS external_td)

set(libs_loc "D:/Libraries/win64")
set(Td_DIR ${libs_loc}/td/tdlib/lib/cmake/Td/)

target_include_directories(external_td SYSTEM
INTERFACE
    ${libs_loc}/td/tdlib/include
)

find_package(Td 1.8.40 REQUIRED)

target_link_libraries(external_td
    INTERFACE Td::TdStatic
)