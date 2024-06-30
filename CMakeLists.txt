cmake_minimum_required (VERSION 3.14)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(QT6_DIR "D:/Packages/Qt/6.7.0/msvc2019_64/lib/cmake/Qt6")

project("TelegramQuickView" LANGUAGES C CXX)

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
find_package(Python3 COMPONENTS Development REQUIRED)


add_executable(${PROJECT_NAME} main.cpp
			src/core/sql/SqlContactModel.cpp
			src/core/sql/SqlContactModel.h
			src/core/sql/SqlConversationModel.cpp
			src/core/sql/SqlConversationModel.h

			src/core/parser/python/call/PythonCaller.cpp
			src/core/parser/python/call/PythonCaller.h

			src/core/winapi/AutoRunRegistryUtils.cpp
			src/core/winapi/AutoRunRegistryUtils.h
			src/core/winapi/MouseDetector.cpp
			src/core/winapi/MouseDetector.h
			src/core/winapi/FileIO.cpp
			src/core/winapi/FileIO.h
			src/core/winapi/MessageHandler.cpp
			src/core/winapi/MessageHandler.h

			src/gui/core/ScrollBar.cpp
			src/gui/core/ScrollBar.h
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
)

set(qml_resource_files
    "src/gui/qml/ChatToolBar.qml"
    "src/gui/qml/ContactPage.qml"
    "src/gui/qml/ConversationPage.qml"
    "src/gui/qml/main.qml"
    "src/gui/qml/themes.conf"
)

qt6_add_resources(TelegramQuickView "qml"
    PREFIX
        "/"
    FILES
        ${qml_resource_files}
)
				

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

set_source_files_properties("assets/images/avatar.jpg"
    PROPERTIES QT_RESOURCE_ALIAS "avatar.jpg"
)

target_link_libraries(${PROJECT_NAME} PRIVATE Python3::Python)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET TelegramQuickView PROPERTY CXX_STANDARD 20)
endif()