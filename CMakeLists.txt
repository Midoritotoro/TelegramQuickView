cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(AUTORCC ON)
set(AUTOMOC ON)

set(project_langs C CXX PYTHON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

project ("TelegramQuickView")

find_package (Python3 COMPONENTS Development REQUIRED)
find_package(PYTHON_LIBRARIES)

include_directories(src/)

add_executable(TelegramQuickView main.cpp
				src/core/parser/python/call/PythonCaller.cpp)


find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Qml)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Qml)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Python3::Python)

qt_standard_project_setup()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TelegramQuickView PROPERTY CXX_STANDARD 20)
endif()
