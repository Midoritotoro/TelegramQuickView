add_library(external_gsl INTERFACE IMPORTED GLOBAL)
add_library(app::external_gsl ALIAS external_gsl)

target_include_directories(external_gsl SYSTEM
INTERFACE
    ${libs_loc}/GSL/include
)
