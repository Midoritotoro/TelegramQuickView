cmake_minimum_required (VERSION 3.24)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT6_DIR "D:/Packages/Qt/6.7.0/msvc2019_64/lib/cmake/Qt6")

project("TelegramQuickView" LANGUAGES C CXX)

find_package(Python3 COMPONENTS Development REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS 
			Core
			Widgets
			Qml
			Multimedia
			MultimediaWidgets
			Sql
			Quick
			Xml
)

qt_standard_project_setup()

SET(PARSER_SOURCES src/core/parser/PythonCaller.cpp src/core/parser/main.cpp src/core/parser/TelegramRobber.py)


SET(MAIN_SOURCES  main.cpp
	src/core/PythonQtWrapper.h
	src/core/sql/SqlConnect.cpp
	src/core/sql/SqlConnect.h

	src/core/winapi/AutoRunRegistryUtils.cpp
	src/core/winapi/AutoRunRegistryUtils.h
	src/core/winapi/MouseDetector.cpp
	src/core/winapi/MouseDetector.h
	src/core/winapi/MessageHandler.cpp
	src/core/winapi/MessageHandler.h
	src/gui/core/WidgetsHider.h

	src/gui/media/ClickableLabel.cpp
	src/gui/media/ClickableLabel.h
	src/gui/media/EnhancedSlider.cpp
	src/gui/media/EnhancedSlider.h
	src/gui/media/VolumeClickableLabel.cpp
	src/gui/media/VolumeClickableLabel.h

	src/gui/media/player/MediaPlayer.cpp
	src/gui/media/player/MediaPlayer.h
	src/gui/media/player/VideoStateWidget.cpp
	src/gui/media/player/VideoStateWidget.h

	src/gui/settings/MainWindow.cpp
	src/gui/settings/MainWindow.h
	src/gui/settings/UserDataManager.cpp
	src/gui/settings/UserDataManager.h
	src/gui/settings/TelegramAuthorizationChecker.cpp
	src/gui/settings/TelegramAuthorizationChecker.h
	src/gui/settings/AuthenticationDialog.cpp
	src/gui/settings/AuthenticationDialog.h

	src/gui/panel/MessageWidget.cpp
	src/gui/panel/MessageWidget.h
)

add_executable(Parser ${PARSER_SOURCES})
target_include_directories(Parser PUBLIC ${Python3_INCLUDE_DIRS})
target_link_libraries(Parser PRIVATE ${Python3_LIBRARIES})

add_executable(${PROJECT_NAME} ${MAIN_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
					Qt6::Core
					Qt6::Widgets
					Qt6::Qml
					Qt6::Multimedia
					Qt6::MultimediaWidgets
					Qt6::Sql
					Qt6::Quick
					Qt6::Xml
)
target_link_libraries(${PROJECT_NAME} PUBLIC Python3::Python)

execute_process(COMMAND python -m pip install -r ${CMAKE_SOURCE_DIR}/requirements.txt REQUIRED
               RESULT_VARIABLE result
               OUTPUT_VARIABLE output
               ERROR_VARIABLE error)

SET_SOURCE_FILES_PROPERTIES(
  ${OBJS}
  PROPERTIES
  EXTERNAL_OBJECT true
  GENERATED true
)

set(CMAKE_INSTALL_PREFIX "D:/TelegramQuickView/")
install(TARGETS Parser DESTINATION out/build)
install(TARGETS TelegramQuickView DESTINATION out/build)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TelegramQuickView PROPERTY CXX_STANDARD 20)
endif()