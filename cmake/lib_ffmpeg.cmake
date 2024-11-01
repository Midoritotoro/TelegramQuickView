set(libs_loc "D:/Libraries/win64")

add_library(external_ffmpeg INTERFACE IMPORTED GLOBAL)
add_library(app::external_ffmpeg ALIAS external_ffmpeg)

find_package(PkgConfig REQUIRED)

pkg_check_modules(AVFILTER REQUIRED IMPORTED_TARGET libavfilter)
pkg_check_modules(AVFORMAT REQUIRED IMPORTED_TARGET libavformat)
pkg_check_modules(AVCODEC REQUIRED IMPORTED_TARGET libavcodec)
pkg_check_modules(AVUTIL REQUIRED IMPORTED_TARGET libavutil)
pkg_check_modules(SWSCALE REQUIRED IMPORTED_TARGET libswscale)
pkg_check_modules(SWRESAMPLE REQUIRED IMPORTED_TARGET libswresample)

target_link_libraries(external_ffmpeg
INTERFACE
    PkgConfig::AVFILTER
    PkgConfig::AVFORMAT
    PkgConfig::AVCODEC
    PkgConfig::AVUTIL
    PkgConfig::SWSCALE
    PkgConfig::SWRESAMPLE
)

set(ffmpeg_lib_list)
set(ffmpeg_lib_loc ${libs_loc}/ffmpeg)

list(APPEND ffmpeg_lib_list
    app::external_opus
    app::external_openh264
)

list(APPEND ffmpeg_lib_list
    ${libs_loc}/dav1d/builddir-release/src/libdav1d.a
)

#message(${libs_loc}/dav1d/builddir-$<IF:$<CONFIG:Debug>,debug,release>/src/libdav1d.a)

target_include_directories(external_ffmpeg SYSTEM
INTERFACE
    ${ffmpeg_lib_loc}
)

list(APPEND ffmpeg_lib_list
    ${ffmpeg_lib_loc}/libavfilter/libavfilter.a
    ${ffmpeg_lib_loc}/libavformat/libavformat.a
    ${ffmpeg_lib_loc}/libavcodec/libavcodec.a
    ${ffmpeg_lib_loc}/libswresample/libswresample.a
    ${ffmpeg_lib_loc}/libswscale/libswscale.a
    ${ffmpeg_lib_loc}/libavutil/libavutil.a
)

target_link_libraries(external_ffmpeg
INTERFACE
    ${ffmpeg_lib_list}
)
