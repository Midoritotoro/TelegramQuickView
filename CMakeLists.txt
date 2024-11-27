cmake_minimum_required(VERSION 3.24)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
project("TelegramQuickView" LANGUAGES CXX)

include(cmake/lib_openh264.cmake)
include(cmake/lib_opus.cmake)
include(cmake/lib_ffmpeg.cmake)
include(cmake/lib_openal.cmake)

#include(cmake/lib_td.cmake)

find_package(Qt6 REQUIRED COMPONENTS 
			Core
			Widgets
			Multimedia
			MultimediaWidgets
			Sql
			OpenGL
)

qt_standard_project_setup()

SET(MAIN_SOURCES main.cpp

	src/Application.cpp
	src/Application.h

	src/core/AutoRunUtils.cpp
	src/core/AutoRunUtils.h
	src/core/MouseDetector.cpp
	src/core/MouseDetector.h
	src/core/StyleCore.cpp
	src/core/StyleCore.h
	
	src/media/ffmpeg/Time.cpp
	src/media/ffmpeg/Time.h
	src/media/ffmpeg/Utility.cpp
	src/media/ffmpeg/Utility.h 
	src/media/ffmpeg/FrameGenerator.h
    src/media/ffmpeg/FrameGenerator.cpp
    src/media/ffmpeg/Guard.h
        
    src/media/ffmpeg/HardwareAcceleration.cpp
    src/media/ffmpeg/HardwareAcceleration.h
        
    src/media/player/MediaPlayer.h 
    src/media/player/MediaPlayer.cpp
    src/media/Manager.cpp
    src/media/Manager.h

    src/media/player/EnhancedSlider.cpp
    src/media/player/EnhancedSlider.h
    src/media/player/FullScreenButton.cpp
    src/media/player/FullScreenButton.h
    src/media/player/MediaPlayerPanel.cpp
    src/media/player/MediaPlayerPanel.h
    src/media/player/VideoStateWidget.cpp
    src/media/player/VideoStateWidget.h
    src/media/player/VolumeController.cpp
    src/media/player/VolumeController.h
    src/media/player/SpeedButton.cpp
    src/media/player/SpeedButton.h
    src/media/player/WidgetsHider.h

    src/media/ClickableLabel.cpp
    src/media/ClickableLabel.h
   
    src/media/ffmpeg/audio/AudioReader.cpp
    src/media/ffmpeg/audio/AudioReader.h
    src/media/ffmpeg/Scale.cpp
    src/media/ffmpeg/Scale.h 
    src/media/ffmpeg/audio/AbstractAudioLoader.cpp
    src/media/ffmpeg/audio/AbstractAudioLoader.h
    src/media/ffmpeg/audio/AbstractAudioReader.cpp
    src/media/ffmpeg/audio/AbstractAudioReader.h
    src/media/ffmpeg/audio/AbstractAudioFFmpegReader.cpp
    src/media/ffmpeg/audio/AbstractAudioFFmpegReader.h
    src/media/ffmpeg/ThumbnailGenerator.cpp
    src/media/ffmpeg/ThumbnailGenerator.h

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
	src/panel/ContinuousScroll.cpp
	src/panel/ContinuousScroll.h
	src/panel/ScrollArea.cpp
	src/panel/ScrollArea.h
	src/panel/data/SqlReader.cpp
	src/panel/data/SqlReader.h

#	src/parser/effects/shaders/dual_kawase_down.frag
	#src/parser/effects/shaders/dual_kawase_up.frag
#	src/parser/effects/shaders/simple.vert

#	src/parser/effects/vertex.h
#	src/parser/effects/OpenGLBlur.cpp
#	src/parser/effects/OpenGLBlur.h

#	src/parser/data/UserDataManager.cpp
#	src/parser/data/UserDataManager.h
#	src/parser/AuthenticationDialog.cpp
#	src/parser/AuthenticationDialog.h

#	src/parser/utils.h
#	src/parser/AbstractTelegramParser.cpp
#	src/parser/AbstractTelegramParser.h
#	src/parser/TelegramParser.cpp
#	src/parser/TelegramParser.h

#	src/parser/data/PostSqlManager.cpp
#	src/parser/data/PostSqlManager.h
)

add_executable(${PROJECT_NAME} ${MAIN_SOURCES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

    target_compile_options(${PROJECT_NAME}
    INTERFACE
        /permissive-
        /utf-8
        /O3
        /W4
        /MP
        /EHsc   
        /w15038 
        /w14265 
        /wd4018
        /wd4100
        /wd4242
        /wd4244
        /wd4245
        /wd4267
        /wd4305
        /wd4324
        /wd4389
        /wd4456
        /wd4457
        /wd4458
        /wd4459
        /wd4611
        /wd4702
        /Zi
        /Zc:__cplusplus
    )

    target_link_options(${PROJECT_NAME}
    INTERFACE
        $<IF:$<CONFIG:Debug>,/NODEFAULTLIB:LIBCMT,/DEBUG;/OPT:REF>
        /INCREMENTAL:NO
    )

    if (WIN32)
        target_link_libraries(${PROJECT_NAME}
        PRIVATE
            winmm.dll
            kernel32.dll
            user32.dll
            bcrypt.dll
            shlwapi.dll
            kernel32.dll
        )
    endif()
endif()


target_link_libraries(${PROJECT_NAME} PRIVATE 
			Qt6::Core
			Qt6::Widgets
			Qt6::Multimedia
			Qt6::MultimediaWidgets
			Qt6::Sql
            Qt6::OpenGL
            app::external_ffmpeg
            app::external_openal		
			#app::external_td
)


SET_SOURCE_FILES_PROPERTIES(
  ${OBJS}
  PROPERTIES
  EXTERNAL_OBJECT true
  GENERATED true
)

set(CMAKE_INSTALL_PREFIX "D:/${PROJECT_NAME}/")

target_compile_definitions(${PROJECT_NAME} PRIVATE PROJECT_NAME="${PROJECT_NAME}")
install(TARGETS TelegramQuickView DESTINATION out/build)