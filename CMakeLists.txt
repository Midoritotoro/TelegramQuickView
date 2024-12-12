cmake_minimum_required(VERSION 3.24)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,Release>:EditAndContinue>,$<$<CONFIG:Debug,Release>:ProgramDatabase>>")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
project("TelegramQuickView" LANGUAGES CXX)


set(libs_loc "D:/Libraries/win64")

include(cmake/lib_openh264.cmake)
include(cmake/lib_opus.cmake)
include(cmake/lib_ffmpeg.cmake)

#include(cmake/lib_openal.cmake)
include(cmake/lib_td.cmake)
include(cmake/lib_gsl.cmake)

set(Td_DIR "D:/Libraries/win64/td/tdlib/lib/cmake/Td/")

find_package(Td 1.8.41 REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS 
			Core
			Widgets
			Multimedia
			OpenGL
)
find_package(Microsoft.GSL CONFIG REQUIRED)


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

    src/core/String.cpp
    src/core/String.h

	src/core/Time.cpp
	src/core/Time.h

    src/core/Timer.cpp
	src/core/Timer.h

    src/core/StyleTypes.h
    src/core/Types.h

    src/core/InvokeQueued.h

	src/settings/MainWindow.cpp
	src/settings/MainWindow.h

	src/ui/WidgetsHider.h

	src/ui/ClickHandler.cpp
	src/ui/ClickHandler.h

    src/ui/BasicClickHandlers.cpp
	src/ui/BasicClickHandlers.h

    src/view/Message.cpp
	src/view/Message.h

	src/view/MessagesView.cpp
	src/view/MessagesView.h

    src/view/MediaViewer.cpp
	src/view/MediaViewer.h

	src/view/MessageAttachment.cpp
	src/view/MessageAttachment.h

	src/view/History.cpp
	src/view/History.h

	src/ui/widgets/AbstractFlatButton.cpp
	src/ui/widgets/AbstractFlatButton.h
    
    src/ui/widgets/FlatButton.cpp
	src/ui/widgets/FlatButton.h

	src/view/TextView.cpp
	src/view/TextView.h

	src/ui/widgets/ContinuousScroll.cpp
	src/ui/widgets/ContinuousScroll.h

    src/ui/widgets/FlatLabel.cpp
	src/ui/widgets/FlatLabel.h

	src/ui/widgets/ScrollArea.cpp
	src/ui/widgets/ScrollArea.h

	src/media/ffmpeg/Utility.cpp
	src/media/ffmpeg/Utility.h 

	src/media/ffmpeg/video/FrameGenerator.h
    src/media/ffmpeg/video/FrameGenerator.cpp
        
    src/media/ffmpeg/video/HardwareAcceleration.cpp
    src/media/ffmpeg/video/HardwareAcceleration.h
        
    src/media/player/MediaPlayer.h 
    src/media/player/MediaPlayer.cpp

    src/media/Manager.cpp
    src/media/Manager.h

    src/ui/widgets/EnhancedSlider.cpp
    src/ui/widgets/EnhancedSlider.h

    src/media/player/buttons/FullScreenButton.cpp
    src/media/player/buttons/FullScreenButton.h

    src/media/player/MediaPlayerPanel.cpp
    src/media/player/MediaPlayerPanel.h

    src/media/player/buttons/VideoStateWidget.cpp
    src/media/player/buttons/VideoStateWidget.h

    src/media/player/buttons/VolumeController.cpp
    src/media/player/buttons/VolumeController.h

    src/media/player/buttons/SpeedButton.cpp
    src/media/player/buttons/SpeedButton.h
   
    src/media/ffmpeg/audio/AudioReader.cpp
    src/media/ffmpeg/audio/AudioReader.h

    src/media/ffmpeg/video/Scale.cpp
    src/media/ffmpeg/video/Scale.h 

    src/media/ffmpeg/audio/AbstractAudioLoader.cpp
    src/media/ffmpeg/audio/AbstractAudioLoader.h

    src/media/ffmpeg/audio/AbstractAudioReader.cpp
    src/media/ffmpeg/audio/AbstractAudioReader.h

    src/media/ffmpeg/audio/AbstractAudioFFmpegReader.cpp
    src/media/ffmpeg/audio/AbstractAudioFFmpegReader.h

    src/ui/effects/shaders/dual_kawase_down.frag
    src/ui/effects/shaders/dual_kawase_up.frag
    src/ui/effects/shaders/simple.vert

	src/ui/effects/vertex.h

    src/ui/effects/OpenGLBlur.cpp
	src/ui/effects/OpenGLBlur.h

	src/parser/data/UserDataManager.cpp
	src/parser/data/UserDataManager.h

	src/parser/AuthenticationDialog.cpp
	src/parser/AuthenticationDialog.h

	src/parser/utils.h

	src/parser/AbstractTelegramParser.cpp
	src/parser/AbstractTelegramParser.h

	src/parser/TelegramParser.cpp
	src/parser/TelegramParser.h
)

add_executable(${PROJECT_NAME} ${MAIN_SOURCES})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
   set_target_properties(${PROJECT_NAME} PROPERTIES
       MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

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
        $<IF:$<CONFIG:Debug>, /NODEFAULTLIB:LIBCMT, /DEBUG;/OPT:REF>
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
            Qt6::OpenGL
            app::external_ffmpeg
            Microsoft.GSL::GSL
            Td::tdclient
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