cmake_minimum_required(VERSION 3.24)

project(base
    VERSION 1.0.0
    DESCRIPTION "..."
    LANGUAGES CXX)


set(export_file_name "export_shared.h")

if(NOT BUILD_SHARED_LIBS)
    set(export_file_name "export_static.h")
endif()

generate_export_header(mylib EXPORT_FILE_NAME include/mylib/${export_file_name})