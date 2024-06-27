# CMakeList.txt: проект CMake для TelegramQuickView; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

# Включение горячей перезагрузки для компиляторов MSVC, если поддерживается.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(AUTORCC ON)
set(AUTOMOC ON)

set(project_langs C CXX PYTHON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# set(SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src )



project ("TelegramQuickView")

find_package (Python3 COMPONENTS Interpreter Development)

target_include_directories(TelegramQuickView PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

add_executable(TelegramQuickView main.cpp
	src/core/parser/python/call/PythonCaller.cpp
	src/core/winapi/AutoRunRegsitryUtils.cpp
	src/core/winapi/MessageHandler.cpp
	src/core/winapi/FileIO.cpp
	src/core/winapi/MouseDetector.cpp)	
	

find_package(Qt6 REQUIRED COMPONENTS Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

qt_standard_project_setup()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TelegramQuickView PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
