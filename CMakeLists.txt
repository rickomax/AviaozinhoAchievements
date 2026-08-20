cmake_minimum_required(VERSION 3.20)

project(quakespasm_sdl2 C CXX)

option(QS_COPY_RUNTIME_DLLS "Copy runtime DLLs after build" ON)
option(QS_FLESH "Enable Flesh definition" OFF)
option(QS_BDD_PRE_4 "Enable BDDPRE4 definition" OFF)

set(QS_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Build" CACHE PATH "Directory for the final executable")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT WIN32 AND NOT APPLE)
    message(FATAL_ERROR "This CMakeLists supports Windows and macOS builds.")
endif()

if(APPLE)
    enable_language(OBJC)
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(QS_64BIT ON)
else()
    set(QS_64BIT OFF)
endif()

if(QS_BDD_PRE_4)
    set(QS_DEFAULT_GAMENAMEEX "Aviao35")
    set(QS_DEFAULT_PIPE_NAME "\\\\\\\\.\\\\pipe\\\\Aviaozinho35")
    set(QS_DEFAULT_SIGNAL_URL "wss://aviaozinho.duckdns.org/")
elseif(QS_FLESH)
    set(QS_DEFAULT_GAMENAMEEX "Flesh")
    set(QS_DEFAULT_PIPE_NAME "\\\\\\\\.\\\\pipe\\\\Flesh")
    set(QS_DEFAULT_SIGNAL_URL "wss://fleshcancer.duckdns.org/")
else()
    set(QS_DEFAULT_GAMENAMEEX "Aviao")
    set(QS_DEFAULT_PIPE_NAME "\\\\\\\\.\\\\pipe\\\\Aviaozinho")
    set(QS_DEFAULT_SIGNAL_URL "wss://aviaozinho.duckdns.org/")
endif()

set(QS_COMMON_SOURCES
    Quake/bgmusic.c
    Quake/cd_sdl.c
    Quake/cfgfile.c
    Quake/chase.c
    Quake/cl_demo.c
    Quake/cl_input.c
    Quake/cl_main.c
    Quake/cl_parse.c
    Quake/cl_tent.c
    Quake/cmd.c
    Quake/common.c
    Quake/console.c
    Quake/crc.c
    Quake/cvar.c
    Quake/font.c
    Quake/fs_zip.c
    Quake/gl_draw.c
    Quake/gl_fog.c
    Quake/gl_mesh.c
    Quake/gl_model.c
    Quake/gl_refrag.c
    Quake/gl_rlight.c
    Quake/gl_rmain.c
    Quake/gl_rmisc.c
    Quake/gl_screen.c
    Quake/gl_sky.c
    Quake/gl_texmgr.c
    Quake/gl_vidsdl.c
    Quake/gl_warp.c
    Quake/host.c
    Quake/host_cmd.c
    Quake/image.c
    Quake/in_sdl.c
    Quake/iplog.c
    Quake/json.c
    Quake/keys.c
    Quake/location.c
    Quake/main_sdl.c
    Quake/mathlib.c
    Quake/mdfour.c
    Quake/menu.c
    Quake/net_dgrm.c
    Quake/net_loop.c
    Quake/net_main.c
    Quake/pipe.c
    Quake/pmove.c
    Quake/pmovetst.c
    Quake/pr_cmds.c
    Quake/pr_edict.c
    Quake/pr_exec.c
    Quake/pr_ext.c
    Quake/r_alias.c
    Quake/r_brush.c
    Quake/r_part.c
    Quake/r_part_fte.c
    Quake/r_sprite.c
    Quake/r_world.c
    Quake/sbar.c
    Quake/snd_codec.c
    Quake/snd_dma.c
    Quake/snd_flac.c
    Quake/snd_mem.c
    Quake/snd_mikmod.c
    Quake/snd_mix.c
    Quake/snd_modplug.c
    Quake/snd_mp3.c
    Quake/snd_mp3tag.c
    Quake/snd_opus.c
    Quake/snd_sdl.c
    Quake/snd_umx.c
    Quake/snd_voip.c
    Quake/snd_vorbis.c
    Quake/snd_wave.c
    Quake/snd_xmp.c
    Quake/strlcat.c
    Quake/strlcpy.c
    Quake/sv_main.c
    Quake/sv_move.c
    Quake/sv_phys.c
    Quake/sv_user.c
    Quake/tiny-json.c
    Quake/view.c
    Quake/wad.c
    Quake/world.c
    Quake/zone.c
)

set(QS_WIN_SOURCES
    Windows/QuakeSpasm.rc
    Quake/net_win.c
    Quake/net_wins.c
    Quake/net_wipx.c
    Quake/net_gns.c
    Quake/pl_win.c
    Quake/sys_sdl_win.c
    Quake/GNSInterface.cpp
)
set(QS_MAC_SOURCES
    Mac/net_bsd.c
    Mac/net_udp.c
    Mac/pl_osx.m
    Mac/sys_sdl_unix.c
    Mac/main_osx.c
)

if(WIN32)
    set(QS_PLATFORM_SOURCES ${QS_WIN_SOURCES})
elseif(APPLE)
    set(QS_PLATFORM_SOURCES ${QS_MAC_SOURCES})
endif()

add_executable(quakespasm WIN32 ${QS_COMMON_SOURCES} ${QS_PLATFORM_SOURCES})

set_target_properties(quakespasm PROPERTIES
    OUTPUT_NAME "AVIAO3GAME"
    RUNTIME_OUTPUT_DIRECTORY "${QS_RUNTIME_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${QS_RUNTIME_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${QS_RUNTIME_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${QS_RUNTIME_OUTPUT_DIRECTORY}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${QS_RUNTIME_OUTPUT_DIRECTORY}"
)

if(WIN32)
if(NOT DEFINED OPENSSL_ROOT_DIR AND DEFINED ENV{OPENSSL_ROOT_DIR})
    set(OPENSSL_ROOT_DIR "$ENV{OPENSSL_ROOT_DIR}")
endif()
if(NOT DEFINED OPENSSL_ROOT_DIR)
    if(QS_64BIT AND EXISTS "C:/Program Files/OpenSSL-Win64")
        set(OPENSSL_ROOT_DIR "C:/Program Files/OpenSSL-Win64")
    elseif(NOT QS_64BIT AND EXISTS "C:/Program Files (x86)/OpenSSL-Win32")
        set(OPENSSL_ROOT_DIR "C:/Program Files (x86)/OpenSSL-Win32")
    endif()
endif()

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

include(FetchContent)

FetchContent_Declare(SDL2
    URL "https://github.com/libsdl-org/SDL/releases/download/release-2.26.3/SDL2-devel-2.26.3-VC.zip"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(SDL2_ttf
    URL "https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-2.24.0.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(libdatachannel
    GIT_REPOSITORY "https://github.com/paullouisageneau/libdatachannel.git"
    GIT_TAG "v0.24.1"
    GIT_SHALLOW TRUE
)
FetchContent_Declare(zlib
    URL "https://github.com/madler/zlib/releases/download/v1.3.1/zlib131.zip"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_Declare(curl
    URL "https://github.com/curl/curl/releases/download/curl-8_9_1/curl-8.9.1.tar.gz"
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(SDL2)
set(SDL2_DIR "${sdl2_SOURCE_DIR}/cmake")
find_package(SDL2 REQUIRED CONFIG)

set(NO_EXAMPLES ON)
set(NO_TESTS ON)
set(NO_MEDIA ON)
set(SDL2TTF_SAMPLES OFF)
set(SDL2TTF_INSTALL OFF)
set(SDL2TTF_VENDORED ON)
set(ZLIB_BUILD_EXAMPLES OFF)
set(BUILD_CURL_EXE OFF)
set(BUILD_TESTING OFF)
set(BUILD_LIBCURL_DOCS OFF)
set(BUILD_MISC_DOCS OFF)
set(ENABLE_CURL_MANUAL OFF)
set(CURL_USE_SCHANNEL ON)
set(CURL_USE_LIBPSL OFF)
set(CURL_USE_LIBSSH2 OFF)
set(CURL_ZLIB OFF)
set(CURL_BROTLI OFF)
set(CURL_ZSTD OFF)
set(USE_NGHTTP2 OFF)
set(USE_LIBIDN2 OFF)
FetchContent_MakeAvailable(libdatachannel SDL2_ttf zlib curl)

set(CODECS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/codecs/include" CACHE PATH "Path to codec headers")
set(CODECS_X64_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/codecs/x64" CACHE PATH "Path to x64 codec libs/dlls")
set(CODECS_X86_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/codecs/x86" CACHE PATH "Path to x86 codec libs/dlls")

set(MISC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/misc/include" CACHE PATH "Path to misc headers")
endif()

target_include_directories(quakespasm PRIVATE
    Quake
    "${CODECS_INCLUDE_DIR}"
    "${MISC_INCLUDE_DIR}"
    "${zlib_SOURCE_DIR}"
    "${zlib_BINARY_DIR}"
)

target_compile_definitions(quakespasm PRIVATE
    _CRT_NONSTDC_NO_DEPRECATE
    _CRT_SECURE_NO_WARNINGS
    _WINSOCK_DEPRECATED_NO_WARNINGS
    _USE_WINSOCK2
    USE_SDL2
    USE_SDL2_TTF
    USE_CURL
    USE_CODEC_MP3
    USE_CODEC_VORBIS
    USE_CODEC_WAVE
    USE_CODEC_FLAC
    USE_CODEC_OPUS
    USE_CODEC_XMP
    USE_CODEC_UMX
    GAMENAMEEX="${QS_DEFAULT_GAMENAMEEX}"
    PIPE_NAME="${QS_DEFAULT_PIPE_NAME}"
    SIGNAL_URL="${QS_DEFAULT_SIGNAL_URL}"
)

if(QS_BDD_PRE_4)
    target_compile_definitions(quakespasm PRIVATE
        BDDPRE4
    )
endif()

if(APPLE)
    find_program(HOMEBREW_EXECUTABLE brew)
    if(HOMEBREW_EXECUTABLE)
        function(qs_brew_prefix out_var formula)
            execute_process(
                COMMAND brew --prefix ${formula}
                OUTPUT_VARIABLE _qs_prefix
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            set(${out_var} "${_qs_prefix}" PARENT_SCOPE)
        endfunction()

        qs_brew_prefix(QS_SDL2_PREFIX sdl2)
        qs_brew_prefix(QS_SDL2_TTF_PREFIX sdl2_ttf)
        qs_brew_prefix(QS_FLAC_PREFIX flac)
        qs_brew_prefix(QS_VORBIS_PREFIX libvorbis)
        qs_brew_prefix(QS_OPUS_PREFIX opus)
        qs_brew_prefix(QS_OPUSFILE_PREFIX opusfile)
        qs_brew_prefix(QS_OGG_PREFIX libogg)
        qs_brew_prefix(QS_MAD_PREFIX mad)
        qs_brew_prefix(QS_XMP_PREFIX libxmp)
        qs_brew_prefix(QS_CURL_PREFIX curl)

        target_include_directories(quakespasm PRIVATE
            "${QS_SDL2_PREFIX}/include/SDL2"
            "${QS_SDL2_TTF_PREFIX}/include/SDL2"
            "${QS_FLAC_PREFIX}/include"
            "${QS_VORBIS_PREFIX}/include"
            "${QS_OPUS_PREFIX}/include/opus"
            "${QS_OPUS_PREFIX}/include"
            "${QS_OPUSFILE_PREFIX}/include/opus"
            "${QS_OPUSFILE_PREFIX}/include"
            "${QS_OGG_PREFIX}/include"
            "${QS_MAD_PREFIX}/include"
            "${QS_XMP_PREFIX}/include"
            "${QS_CURL_PREFIX}/include"
        )

        target_link_directories(quakespasm PRIVATE
            "${QS_SDL2_PREFIX}/lib"
            "${QS_SDL2_TTF_PREFIX}/lib"
            "${QS_FLAC_PREFIX}/lib"
            "${QS_VORBIS_PREFIX}/lib"
            "${QS_OPUS_PREFIX}/lib"
            "${QS_OPUSFILE_PREFIX}/lib"
            "${QS_OGG_PREFIX}/lib"
            "${QS_MAD_PREFIX}/lib"
            "${QS_XMP_PREFIX}/lib"
            "${QS_CURL_PREFIX}/lib"
        )
    else()
        target_include_directories(quakespasm PRIVATE
            "${QS_PREFIX}/include/SDL2"
            "${QS_PREFIX}/include"
            "${QS_PREFIX}/include/opus"
        )

        target_link_directories(quakespasm PRIVATE
            "${QS_PREFIX}/lib"
        )
    endif()

    target_compile_definitions(quakespasm PRIVATE GL_SILENCE_DEPRECATION=1)

    target_link_libraries(quakespasm PRIVATE
        SDL2
        SDL2_ttf
        vorbisfile
        vorbis
        ogg
        opusfile
        opus
        FLAC
        mad
        xmp
        curl
        z
        "-framework Cocoa"
        "-framework OpenGL"
        "-framework IOKit"
    )
endif()

if(WIN32)
    set(LIBDATACHANNEL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/libdatachannel/include" CACHE PATH "Path to libdatachannel headers")
    set(LIBDATACHANNEL_X64_DEBUG_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/libdatachannel/debug" CACHE PATH "Path to x64 libdatachannel debug libs/dlls")
    set(LIBDATACHANNEL_X64_RELEASE_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/libdatachannel/release" CACHE PATH "Path to x64 libdatachannel release libs/dlls")
    set(LIBDATACHANNEL_X86_DEBUG_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/libdatachannel/debug_x86" CACHE PATH "Path to x86 libdatachannel debug libs/dlls")
    set(LIBDATACHANNEL_X86_RELEASE_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/libdatachannel/release_x86" CACHE PATH "Path to x86 libdatachannel release libs/dlls")

    set(SDL2_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/SDL2/include" CACHE PATH "Path to SDL2 headers")
    set(SDL2_X64_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/SDL2/lib64" CACHE PATH "Path to x64 SDL2 libs/dlls")
    set(SDL2_X86_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/SDL2/lib" CACHE PATH "Path to x86 SDL2 libs/dlls")

    set(SDL2_TTF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/SDL2TTF/include" CACHE PATH "Path to SDL2_ttf headers")
    set(SDL2_TTF_X64_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/SDL2TTF/Release" CACHE PATH "Path to x64 SDL2_ttf libs/dlls")
    set(SDL2_TTF_X86_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/SDL2TTF/lib" CACHE PATH "Path to x86 SDL2_ttf libs/dlls")

    set(CODECS_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/codecs/include" CACHE PATH "Path to codec headers")
    set(CODECS_X64_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/codecs/x64" CACHE PATH "Path to x64 codec libs/dlls")
    set(CODECS_X86_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/codecs/x86" CACHE PATH "Path to x86 codec libs/dlls")

    set(MISC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/misc/include" CACHE PATH "Path to misc headers")

    set(ZLIB_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/zlib/include" CACHE PATH "Path to zlib headers")
    set(ZLIB_X64_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/zlib/x64" CACHE PATH "Path to x64 zlib libs/dlls")
    set(ZLIB_X86_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/zlib/x86" CACHE PATH "Path to x86 zlib libs/dlls")

    set(CURL_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/curl/include" CACHE PATH "Path to curl headers")
    set(CURL_X64_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/curl/lib/x64" CACHE PATH "Path to x64 curl libs/dlls")
    set(CURL_X86_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Windows/curl/lib/x86" CACHE PATH "Path to x86 curl libs/dlls")

    target_include_directories(quakespasm PRIVATE
        "${SDL2_INCLUDE_DIR}"
        "${SDL2_TTF_INCLUDE_DIR}"
        "${LIBDATACHANNEL_INCLUDE_DIR}"
        "${CODECS_INCLUDE_DIR}"
        "${MISC_INCLUDE_DIR}"
        "${ZLIB_INCLUDE_DIR}"
        "${CURL_INCLUDE_DIR}"
    )

    target_compile_definitions(quakespasm PRIVATE
        _CRT_NONSTDC_NO_DEPRECATE
        _CRT_SECURE_NO_WARNINGS
        _WINSOCK_DEPRECATED_NO_WARNINGS
        _USE_WINSOCK2
        USE_SDL2_TTF
    )
endif()

if(MSVC)
    target_compile_options(quakespasm PRIVATE /W3)

    set_source_files_properties(
        Quake/bgmusic.c
        Quake/cd_sdl.c
        Quake/cfgfile.c
        Quake/chase.c
        Quake/cl_demo.c
        Quake/cl_input.c
        Quake/cl_main.c
        Quake/cl_parse.c
        Quake/cl_tent.c
        Quake/cmd.c
        Quake/common.c
        Quake/console.c
        Quake/crc.c
        Quake/cvar.c
        Quake/font.c
        Quake/fs_zip.c
        Quake/gl_draw.c
        Quake/gl_fog.c
        Quake/gl_mesh.c
        Quake/gl_model.c
        Quake/gl_refrag.c
        Quake/gl_rlight.c
        Quake/gl_rmain.c
        Quake/gl_rmisc.c
        Quake/gl_screen.c
        Quake/gl_sky.c
        Quake/gl_texmgr.c
        Quake/gl_vidsdl.c
        Quake/gl_warp.c
        Quake/host.c
        Quake/host_cmd.c
        Quake/image.c
        Quake/in_sdl.c
        Quake/iplog.c
        Quake/json.c
        Quake/keys.c
        Quake/location.c
        Quake/main_sdl.c
        Quake/mathlib.c
        Quake/mdfour.c
        Quake/menu.c
        Quake/net_dgrm.c
        Quake/net_loop.c
        Quake/net_main.c
        Quake/net_win.c
        Quake/net_wins.c
        Quake/net_wipx.c
        Quake/net_gns.c
        Quake/pipe.c
        Quake/pl_win.c
        Quake/pmove.c
        Quake/pmovetst.c
        Quake/pr_cmds.c
        Quake/pr_edict.c
        Quake/pr_exec.c
        Quake/pr_ext.c
        Quake/r_alias.c
        Quake/r_brush.c
        Quake/r_part.c
        Quake/r_part_fte.c
        Quake/r_sprite.c
        Quake/r_world.c
        Quake/sbar.c
        Quake/snd_codec.c
        Quake/snd_dma.c
        Quake/snd_flac.c
        Quake/snd_mem.c
        Quake/snd_mikmod.c
        Quake/snd_mix.c
        Quake/snd_modplug.c
        Quake/snd_mp3.c
        Quake/snd_mp3tag.c
        Quake/snd_opus.c
        Quake/snd_sdl.c
        Quake/snd_umx.c
        Quake/snd_voip.c
        Quake/snd_vorbis.c
        Quake/snd_wave.c
        Quake/snd_xmp.c
        Quake/strlcat.c
        Quake/strlcpy.c
        Quake/sv_main.c
        Quake/sv_move.c
        Quake/sv_phys.c
        Quake/sv_user.c
        Quake/sys_sdl_win.c
        Quake/tiny-json.c
        Quake/view.c
        Quake/wad.c
        Quake/world.c
        Quake/zone.c
        PROPERTIES LANGUAGE C
    )

    set_source_files_properties(
        Quake/GNSInterface.cpp
        PROPERTIES LANGUAGE CXX
    )

    set_property(TARGET quakespasm PROPERTY
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
    )

    target_link_options(quakespasm PRIVATE /SUBSYSTEM:WINDOWS)

    if(QS_64BIT)
        target_link_directories(quakespasm PRIVATE
            "${CODECS_X64_LIB_DIR}"
        )
    else()
        target_link_directories(quakespasm PRIVATE
            "${CODECS_X86_LIB_DIR}"
        )
    endif()

    target_link_libraries(quakespasm PRIVATE
        libvorbisfile.lib
        libvorbis.lib
        libopusfile.lib
        libopus.lib
        libFLAC.lib
        libogg.lib
        libmad.lib
        libxmp.lib
        ws2_32.lib
        opengl32.lib
        winmm.lib
        zlibstatic
        CURL::libcurl
        datachannel
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_ttf
    )
endif()

if(WIN32 AND QS_COPY_RUNTIME_DLLS)
    if(QS_64BIT)
        add_custom_command(TARGET quakespasm POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CODECS_X64_LIB_DIR}" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:CURL::libcurl>" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:SDL2_ttf>" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:datachannel>" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND_EXPAND_LISTS
        )
    else()
        add_custom_command(TARGET quakespasm POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${CODECS_X86_LIB_DIR}" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:CURL::libcurl>" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:SDL2::SDL2>" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:SDL2_ttf>" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:datachannel>" "$<TARGET_FILE_DIR:quakespasm>"
            COMMAND_EXPAND_LISTS
        )
    endif()
endif()