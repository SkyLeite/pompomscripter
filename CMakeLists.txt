<<<<<<< HEAD
cmake_minimum_required(VERSION 3.8)
project(ponscripter)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_CURRENT_SOURCE_DIR}/cmake/sdl2)

option(USE_STEAM "Enable Steam Support" OFF)
option(USE_CPU_GFX "Custom graphics using intrinsics" ON)
option(ENABLE_GAME_CONTROLLERS "Enable support for game controllers" ON)

# Workaround for bug on macOS where if you have Mono installed, CMake will prefer the (old) versions of JPEG and PNG that ship with it, even though you'll be linking with the dylibs in /usr/local/
set(CMAKE_FIND_FRAMEWORK LAST)

if(USE_STEAM)
	add_subdirectory(src/extlib/src/steam-sdk)
endif()
find_package(BZip2 1.0.4 REQUIRED)
find_package(Freetype 2.3.5 REQUIRED)
find_package(JPEG 62 REQUIRED)
find_package(PNG 1.2.24 REQUIRED)
find_package(OGG 1.1.3 REQUIRED)
find_package(Vorbis 1.3.4 REQUIRED)
find_package(VorbisFile 1.3.4 REQUIRED)
find_package(SDL2 2.0.3 REQUIRED)
find_package(SDL2_image 2.0.0 REQUIRED)
find_package(SDL2_mixer 2.0.0 REQUIRED)
find_package(SMPEG2 2.0.0 REQUIRED)
find_package(ZLIB 1.2.3 REQUIRED)

add_subdirectory(src)
=======
cmake_minimum_required(VERSION 3.31.6)

project(POMPOM)

add_executable(POMPOM)

set_target_properties(POMPOM PROPERTIES OUTPUT_NAME pompom)
target_compile_features(POMPOM PRIVATE cxx_std_20)

# Find sources
set(POMPOM_SOURCES
    src/AnimationInfo.cpp
    src/Dbg.cpp
    src/DirPaths.cpp
    src/DirectReader.cpp
    src/DirtyRect.cpp
    src/Fontinfo.cpp
    src/NsaReader.cpp
    src/Ponscripter.cpp
    src/PonscripterLabel.cpp
    src/PonscripterLabel_animation.cpp
    src/PonscripterLabel_command.cpp
    src/PonscripterLabel_effect.cpp
    src/PonscripterLabel_effect_breakup.cpp
    src/PonscripterLabel_effect_cascade.cpp
    src/PonscripterLabel_effect_trig.cpp
    src/PonscripterLabel_event.cpp
    src/PonscripterLabel_ext.cpp
    src/PonscripterLabel_file.cpp
    src/PonscripterLabel_file2.cpp
    src/PonscripterLabel_image.cpp
    src/PonscripterLabel_rmenu.cpp
    src/PonscripterLabel_sound.cpp
    src/PonscripterLabel_text.cpp
    src/PonscripterMessage.cpp
    src/SarReader.cpp
    src/ScriptHandler.cpp
    src/ScriptParser.cpp
    src/ScriptParser_command.cpp
    src/bstrwrap.cpp
    src/cp932_encoding.cpp
    src/encoding.cpp
    src/expression.cpp
    src/font.cpp
    src/graphics_altivec.cpp
    src/graphics_mmx.cpp
    src/graphics_sse2.cpp
    src/prng.cpp
    src/pstring.cpp
    src/resize_image.cpp
    src/resources.cpp
    src/bstrlib.c
)

if(ENABLE_AVIFILE)
    list(APPEND POMPOM_SOURCES src/AVIWrapper.cpp)
endif()

if(ENABLE_MP3_MAD)
    target_compile_definitions(POMPOM PRIVATE ENABLE_MP3_MAD=1)
    list(APPEND POMPOM_SOURCES src/MadWrapper.cpp)
else()
    find_library(SMPEG_LIBRARY NAMES smpeg SMPEG Smpeg SMpeg smpeg2 PATHS /opt/homebrew/lib /usr/local/lib REQUIRED)
    target_link_libraries(POMPOM PRIVATE ${SMPEG_LIBRARY})
    target_include_directories(POMPOM PUBLIC src/extlib/src/smpeg-2.0.0)
endif()

if(WINDOWS)
    list(APPEND POMPOM_SOURCES src/SDL_win32_main.c)
endif()

target_sources(POMPOM PRIVATE ${POMPOM_SOURCES})

# Find dependencies
find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(POMPOM PRIVATE SDL2::SDL2)

find_package(SDL2_image CONFIG REQUIRED)
target_link_libraries(POMPOM PRIVATE SDL2_image::SDL2_image)

find_package(SDL2_mixer CONFIG REQUIRED)
target_link_libraries(POMPOM PRIVATE SDL2_mixer::SDL2_mixer)

find_package(Freetype REQUIRED)
target_link_libraries(POMPOM PRIVATE Freetype::Freetype)

find_package(BZip2 REQUIRED)
target_link_libraries(POMPOM PRIVATE BZip2::BZip2)

include(FetchContent)
FetchContent_Declare(imgui_external
    URL https://github.com/SkyLeite/imgui/archive/bc51d937f348cb884dfb702b7e5f7522cdda51f2.zip
	EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui_external)

add_library(imgui
	${imgui_external_SOURCE_DIR}/imgui.cpp
	${imgui_external_SOURCE_DIR}/imgui_demo.cpp
	${imgui_external_SOURCE_DIR}/imgui_draw.cpp
	${imgui_external_SOURCE_DIR}/imgui_tables.cpp
	${imgui_external_SOURCE_DIR}/imgui_widgets.cpp
	${imgui_external_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
	${imgui_external_SOURCE_DIR}/backends/imgui_impl_sdlrenderer2.cpp
)
target_link_libraries(imgui PUBLIC SDL2::SDL2)
target_include_directories(imgui PUBLIC ${imgui_external_SOURCE_DIR})

target_link_libraries(POMPOM PUBLIC imgui)


# MacOS-specific
if(APPLE)
    target_compile_definitions(POMPOM PRIVATE MACOSX=1)

    target_link_libraries(POMPOM PRIVATE
        "-framework CoreFoundation"
        "-framework ApplicationServices"
        "-framework CoreServices"
    )
endif()
>>>>>>> 7a9b563 (Build with CMake)
