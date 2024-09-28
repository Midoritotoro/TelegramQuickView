cmake_minimum_required (VERSION 3.24)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

project("TelegramQuickView" LANGUAGES CXX)

set(Td_DIR "D:/Packages/td/tdlib/lib/cmake/Td")


find_package(Td 1.8.36 REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS 
			Core
			Widgets
			Multimedia
			MultimediaWidgets
			Sql
)

qt_standard_project_setup()

SET(PARSER_SOURCES src/parser/main.cpp

	src/parser/UserDataManager.cpp
	src/parser/UserDataManager.h
	src/parser/AuthenticationDialog.cpp
	src/parser/AuthenticationDialog.h
	src/parser/TelegramAuthorizer.cpp
	src/parser/TelegramAuthorizer.h
	src/parser/TelegramParser.cpp
	src/parser/TelegramParser.h
	src/parser/PostSqlManager.cpp
	src/parser/PostSqlManager.h
)

SET(MAIN_SOURCES main.cpp

	src/core/AutoRunUtils.cpp
	src/core/AutoRunUtils.h
	src/core/MouseDetector.cpp
	src/core/MouseDetector.h
	src/media/WidgetsHider.h

	src/media/ClickableLabel.cpp
	src/media/ClickableLabel.h
	src/media/player/EnhancedSlider.cpp
	src/media/player/EnhancedSlider.h
	src/media/player/VolumeController.cpp
	src/media/player/VolumeController.h

	src/media/player/AbstractMediaPlayer.cpp
	src/media/player/AbstractMediaPlayer.h
	src/media/player/MediaPlayerPanel.cpp
	src/media/player/MediaPlayerPanel.h
	src/media/player/MediaPlayer.cpp
	src/media/player/MediaPlayer.h
	src/media/player/VideoStateWidget.cpp
	src/media/player/VideoStateWidget.h
	src/media/player/FullScreenButton.cpp
	src/media/player/FullScreenButton.h

	src/settings/MainWindow.cpp
	src/settings/MainWindow.h

	src/panel/MessageWidget.cpp
	src/panel/MessageWidget.h
	src/panel/TelegramPostQuickView.cpp
	src/panel/TelegramPostQuickView.h
	src/panel/MessageAttachment.cpp
	src/panel/MessageAttachment.h
	src/panel/MessageMediaViewer.cpp
	src/panel/MessageMediaViewer.h
	src/panel/History.cpp
	src/panel/History.h
	src/panel/NavigationButton.cpp
	src/panel/NavigationButton.h
	src/panel/MessageTextView.cpp
	src/panel/MessageTextView.h
	src/panel/ScrollArea.cpp
	src/panel/ScrollArea.h

	src/sql/SqlContentManager.cpp
	src/sql/SqlContentManager.h
)

add_executable(TelegramParser ${PARSER_SOURCES})
add_executable(${PROJECT_NAME} ${MAIN_SOURCES})

set_property(TARGET TelegramParser PROPERTY CXX_STANDARD 20)
set_property(TARGET TelegramQuickView PROPERTY CXX_STANDARD 20)

target_link_libraries(TelegramParser PRIVATE
					Qt6::Core
					Qt6::Widgets
					Qt6::Sql
					Td::TdJson
					Td::TdStatic
)


target_link_libraries(${PROJECT_NAME} PRIVATE 
					Qt6::Core
					Qt6::Widgets
					Qt6::Multimedia
					Qt6::MultimediaWidgets
					Qt6::Sql
)


SET_SOURCE_FILES_PROPERTIES(
  ${OBJS}
  PROPERTIES
  EXTERNAL_OBJECT true
  GENERATED true
)

set(CMAKE_INSTALL_PREFIX "D:/${PROJECT_NAME}/")

target_compile_definitions(TelegramParser PRIVATE PROJECT_NAME="${PROJECT_NAME}")
target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_NAME="${PROJECT_NAME}")

install(TARGETS TelegramParser DESTINATION out/build)
install(TARGETS TelegramQuickView DESTINATION out/build)
