#
# Copyright (c) 2017-2022 the rbfx project.
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

if (URHO3D_NO_EDITOR_PLAYER_EXE)
    add_library (libPlayer STATIC ${SOURCE_FILES})
    set (PLAYER_TARGET libPlayer)
elseif (ANDROID OR URHO3D_CSHARP)
    add_library (libPlayer MODULE ${SOURCE_FILES} Player.cpp)
    set (PLAYER_TARGET libPlayer)
    if (URHO3D_CSHARP)
        add_target_csharp(
            TARGET Player
            EXE
            PROJECT ${CMAKE_CURRENT_SOURCE_DIR}/ManagedHost/Player.csproj
            OUTPUT ${CMAKE_BINARY_DIR}/${DEST_BIN_DIR_CONFIG}/Player
            DEPENDS Urho3DNet)
        if (TARGET Player)
            add_dependencies(Player libPlayer Urho3DNet)
        endif ()
        install(FILES "${CMAKE_BINARY_DIR}/${DEST_BIN_DIR_CONFIG}/Player${CMAKE_EXECUTABLE_SUFFIX}" DESTINATION ${DEST_BIN_DIR_CONFIG})
    endif ()
else ()
    if (WEB)
        set(CMAKE_EXECUTABLE_SUFFIX ".html")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_C_FLAGS}")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_C_FLAGS}")
        set(USE_L_FLAGS "-s FETCH=1 -s MAX_WEBGL_VERSION=2 -s MIN_WEBGL_VERSION=2 -s FULL_ES3=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_RUNTIME_METHODS=['ccall','cwrap','setValue','getValue'] --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/../../KFPlayerAssets@/ --bind")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_L_FLAGS}")
        add_executable(main)
        target_sources(main PRIVATE Player.cpp)
        set (PLAYER_TARGET main)
    else()
        if (NOT URHO3D_WIN32_CONSOLE)
            set (TARGET_TYPE WIN32)
        endif ()
        if (WIN32)
            set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
        endif ()
        if (APPLE)
            set(ASSETS_FOLDER
                "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/Assets"
                )
            # cocos_mark_multi_resources(kf_common_res RES_TO "Resources" FOLDERS ${ASSETS_FOLDER})
            if(IOS)
                set(APP_UI_RES
                    # ios/Info.plist
                    ios/LaunchScreen.storyboard
                    ios/LaunchScreenBackground.png
                    ios/Images.xcassets
                    )
                list(APPEND SOURCE_FILES
                    ${APP_UI_RES}
                    )
            elseif(MACOSX)
                set(APP_UI_RES
                    # mac/Icon.icns
                    mac/Info.plist
                    # mac/en.lproj/InfoPlist.strings
                    )
                list(APPEND SOURCE_FILES
                    ${APP_UI_RES}
                    )
            endif()
            # list(APPEND SOURCE_FILES ${kf_common_res})
        endif()
        add_executable (Player ${TARGET_TYPE} ${SOURCE_FILES} Player.cpp)
        set (PLAYER_TARGET Player)
    endif ()
endif ()

if (APPLE)
    set_target_properties(${PLAYER_TARGET} PROPERTIES RESOURCE "${APP_UI_RES}")
    if(IOS)
        set_xcode_property(${PLAYER_TARGET} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ios/Info.plist")
        set_xcode_property(${PLAYER_TARGET} ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon-${PLAYER_TARGET}")
        find_library(CORE_TEXT_LIBRARY CoreText)
        find_library(ZLIB_LIBRARY z)
        target_link_libraries(${PLAYER_TARGET} PRIVATE ${CORE_TEXT_LIBRARY})
        target_link_libraries(${PLAYER_TARGET} PRIVATE ${ZLIB_LIBRARY})
        target_link_libraries(${PLAYER_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/fmod/lib/ios/libfmod_iphoneos.a)
        target_link_libraries(${PLAYER_TARGET} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/fmod/lib/ios/libfmodstudio_iphoneos.a)
    elseif(MACOSX)
        set_xcode_property(${PLAYER_TARGET} INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist")
    endif()
endif ()

if (TARGET libPlayer)
    set_target_properties(libPlayer PROPERTIES PREFIX "")
endif ()

target_link_libraries(${PLAYER_TARGET} PUBLIC Urho3D)
target_include_directories(${PLAYER_TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if (WIN32)
    set_property(TARGET ${PLAYER_TARGET} APPEND PROPERTY LINK_FLAGS "/SUBSYSTEM:CONSOLE")
    add_custom_command(TARGET ${PLAYER_TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/fmod/windows/core/lib/fmod.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/fmod/windows/studio/lib/fmodstudio.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/curl/prebuilt/win/libcurl.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/curl/prebuilt/win/libcurl.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/zlib1.dll
        $<TARGET_FILE_DIR:${PLAYER_TARGET}>)
endif ()
# install(TARGETS ${PLAYER_TARGET}
#     LIBRARY DESTINATION ${DEST_LIBRARY_DIR_CONFIG}
#     RUNTIME DESTINATION ${DEST_BIN_DIR_CONFIG}
#     ARCHIVE DESTINATION ${DEST_ARCHIVE_DIR_CONFIG}
#     BUNDLE  DESTINATION ${DEST_BIN_DIR_CONFIG}
# )
