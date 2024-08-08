cmake_minimum_required (VERSION 3.24)

# "C:\Windows\system32\cmd.exe" /c "%SYSTEMROOT%\System32\chcp.com 65001 >NUL && "D:\VS\2022\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\CMake\bin\cmake.exe"  -G "Ninja"  -DCMAKE_PREFIX_PATH:STRING="D:/Packages/Qt/6.7.0/msvc2019_64" -DCMAKE_BUILD_TYPE:STRING="Release"   -DCMAKE_MAKE_PROGRAM="D:\VS\2022\COMMON7\IDE\COMMONEXTENSIONS\MICROSOFT\CMAKE\Ninja\ninja.exe" "D:\TelegramQuickView" 2>&1"

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
			Multimedia
			MultimediaWidgets
			Sql
)

qt_standard_project_setup()

SET(PARSER_SOURCES src/parser/TelegramRobber.cpp src/parser/main.cpp src/parser/TelegramRobber.py src/parser/SqlParserContentManager.py)

SET(MAIN_SOURCES main.cpp

	src/core/AutoRunUtils.cpp
	src/core/AutoRunUtils.h
	src/core/MouseDetector.cpp
	src/core/MouseDetector.h
	src/media/WidgetsHider.h

	src/media/ClickableLabel.cpp
	src/media/ClickableLabel.h
	src/media/EnhancedSlider.cpp
	src/media/EnhancedSlider.h
	src/media/VolumeClickableLabel.cpp
	src/media/VolumeClickableLabel.h

	src/media/player/MediaPlayer.cpp
	src/media/player/MediaPlayer.h
	src/media/player/VideoStateWidget.cpp
	src/media/player/VideoStateWidget.h

	src/settings/MainWindow.cpp
	src/settings/MainWindow.h
	src/settings/UserDataManager.cpp
	src/settings/UserDataManager.h
	src/settings/TelegramAuthorizationChecker.cpp
	src/settings/TelegramAuthorizationChecker.h
	src/settings/AuthenticationDialog.cpp
	src/settings/AuthenticationDialog.h

	src/panel/MessageWidget.cpp
	src/panel/MessageWidget.h
	src/panel/TelegramPostQuickView.cpp
	src/panel/TelegramPostQuickView.h
	src/panel/MessageAttachment.cpp
	src/panel/MessageAttachment.h
	src/panel/MessageMediaViewer.cpp
	src/panel/MessageMediaViewer.h

	src/sql/SqlContentManager.cpp
	src/sql/SqlContentManager.h
)

add_executable(Parser ${PARSER_SOURCES})
target_include_directories(Parser PUBLIC ${Python3_INCLUDE_DIRS})
target_link_libraries(Parser PRIVATE ${Python3_LIBRARIES})

add_executable(${PROJECT_NAME} ${MAIN_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE 
					Qt6::Core
					Qt6::Widgets
					Qt6::Multimedia
					Qt6::MultimediaWidgets
					Qt6::Sql
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