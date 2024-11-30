add_library(external_openssl_common INTERFACE IMPORTED GLOBAL)
add_library(app::external_openssl_common ALIAS external_openssl_common)

set(libs_loc "D:/Libraries/win64")

target_include_directories(external_openssl_common SYSTEM
INTERFACE
    ${libs_loc}/openssl3/include
)
  