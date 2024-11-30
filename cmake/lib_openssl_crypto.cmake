add_library(external_openssl_crypto STATIC IMPORTED GLOBAL)
add_library(app::external_openssl_crypto ALIAS external_openssl_crypto)

set(libs_loc "D:/Libraries/win64")

set_target_properties(external_openssl_crypto PROPERTIES
    IMPORTED_LOCATION "${libs_loc}/openssl3/out/libcrypto.lib"
    IMPORTED_LOCATION_DEBUG "${libs_loc}/openssl3/out.dbg/libcrypto.lib"
)

target_link_libraries(external_openssl_crypto
INTERFACE
   app::external_openssl_common
)
