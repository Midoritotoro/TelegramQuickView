add_library(external_openssl_ssl STATIC IMPORTED GLOBAL)
add_library(app::external_openssl_ssl ALIAS external_openssl_ssl)

set(libs_loc "D:/Libraries/win64")

set_target_properties(external_openssl_ssl PROPERTIES
    IMPORTED_LOCATION "${libs_loc}/openssl3/out/libssl.lib"
    IMPORTED_LOCATION_DEBUG "${libs_loc}/openssl3/out/libssl.lib"
)

target_link_libraries(external_openssl_ssl
INTERFACE
    app::external_openssl_common
)
