cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(AUTORCC ON)

set(project_langs C CXX PYTHON)

project ("TelegramQuickView")

include_directories(src/)

qt6_wrap_cpp(GUI_MOC ${HW_SOURCE_H})

add_executable(TelegramQuickView main.cpp
			#cpp

			src/core/parser/python/call/PythonCaller.cpp
			src/core/winapi/AutoRunRegistryUtils.cpp
			src/core/winapi/FileIO.cpp
			src/core/winapi/MessageHandler.cpp
			src/core/winapi/MouseDetector.cpp
			src/gui/core/ScrollBar.cpp
			src/gui/core/WidgetsHider.h
			src/gui/media/ClickableLabel.cpp
			src/gui/media/EnhancedSlider.cpp
			src/gui/media/VolumeClickableLabel.cpp
			src/gui/media/player/MediaPlayer.cpp
			src/gui/media/player/VideoStateWidget.cpp
			src/gui/settings/MainWindow.cpp
			
			#h

			src/core/parser/python/call/PythonCaller.h
			src/core/winapi/AutoRunRegistryUtils.h
			src/core/winapi/FileIO.h
			src/core/winapi/MessageHandler.h
			src/core/winapi/MouseDetector.h
			src/gui/core/ScrollBar.h
			src/gui/media/ClickableLabel.h
			src/gui/media/EnhancedSlider.h
			src/gui/media/VolumeClickableLabel.h
			src/gui/media/player/MediaPlayer.h
			src/gui/media/player/VideoStateWidget.h
			src/gui/settings/MainWindow.h
			
			#qml

			src/gui/qml/ChatToolBar.qml
			src/gui/qml/ContactPage.qml
			src/gui/qml/ConversationPage.qml
			src/gui/qml/main.qml

			#qrc

			src/gui/qml/qml.qrc

			#python

			src/core/parser/python/code/TelegramRobber.py
)

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Qml)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS MultimediaWidgets)
find_package(Qt6 REQUIRED COMPONENTS Sql)
find_package(Qt6 REQUIRED COMPONENTS Quick)

find_package(Python3 COMPONENTS Development REQUIRED)
find_package(REQUIRED PYTHON_LIBRARIES)

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Qml)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Multimedia)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::MultimediaWidgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Sql)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Quick)

target_link_libraries(${PROJECT_NAME} PRIVATE Python3::Python)

qt_standard_project_setup()

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TelegramQuickView PROPERTY CXX_STANDARD 20)
endif()
