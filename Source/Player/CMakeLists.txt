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

# Always export Player without entry point as PlayerLibrary.
add_library (PlayerLibrary STATIC PlayerApplication.cpp PlayerApplication.h)
target_include_directories(PlayerLibrary PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>
    $<INSTALL_INTERFACE:${DEST_INCLUDE_DIR}/Tools>
)
target_link_libraries(PlayerLibrary PUBLIC Urho3D)

if (NOT URHO3D_NO_EDITOR_PLAYER_EXE)
    if (ANDROID OR URHO3D_CSHARP)
        add_library (libPlayer MODULE ${SOURCE_FILES} Player.cpp)
        set_target_properties(libPlayer PROPERTIES PREFIX "")
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
    elseif (NOT WEB)
        # Player executable is not usefil in web builds, because there would be no way to load user code.
        if (NOT URHO3D_WIN32_CONSOLE)
            set (TARGET_TYPE WIN32)
        endif ()
        add_executable (Player ${TARGET_TYPE} Player.cpp)
        set (PLAYER_TARGET Player)
    endif ()

    if (PLAYER_TARGET)
        target_link_libraries(${PLAYER_TARGET} PUBLIC PlayerLibrary)
        # target_link_plugins(${PLAYER_TARGET} "${URHO3D_PLUGIN_LIST}")
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

if (WIN32)
    set_property(TARGET ${PLAYER_TARGET} APPEND PROPERTY LINK_FLAGS "/SUBSYSTEM:CONSOLE")
    add_custom_command(TARGET ${PLAYER_TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/fmod/windows/core/lib/fmod.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/fmod/windows/studio/lib/fmodstudio.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/curl/prebuilt/win/libcurl.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty/curl/prebuilt/win/libcurl.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/zlib1.dll
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/LuaPanda.lua
        $<TARGET_FILE_DIR:${PLAYER_TARGET}>)
    add_custom_command(TARGET ${PLAYER_TARGET} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/../../tools/socket/core.dll
        $<TARGET_FILE_DIR:${PLAYER_TARGET}>/socket/core.dll)
endif ()

# SDK installation
# if (PLAYER_TARGET)
#     install (TARGETS ${PLAYER_TARGET}
#         LIBRARY DESTINATION ${DEST_LIBRARY_DIR_CONFIG}
#         RUNTIME DESTINATION ${DEST_BIN_DIR_CONFIG}
#         ARCHIVE DESTINATION ${DEST_ARCHIVE_DIR_CONFIG}
#         BUNDLE  DESTINATION ${DEST_BIN_DIR_CONFIG}
#     )
# endif ()

# install (TARGETS PlayerLibrary EXPORT Urho3D ARCHIVE DESTINATION ${DEST_ARCHIVE_DIR_CONFIG})
# install (DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#     DESTINATION ${DEST_INCLUDE_DIR}/Tools
#     FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
# )
