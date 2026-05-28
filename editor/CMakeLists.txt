cmake_minimum_required(VERSION 3.16)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_definitions(-DWIN32_LEAN_AND_MEAN -DNOMINMAX)

set(BIN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/editor)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BIN_OUTPUT_DIR})

file(GLOB_RECURSE EDITOR_SOURCE "src/*.cpp" "main.cpp")
file(GLOB_RECURSE EDITOR_HEADER "include/*.h" "include/*.inl")

add_custom_target(UpdateResources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${ENGINE_SHADERS_DIR}" "${BIN_OUTPUT_DIR}/shaders"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${SCRIPTING_DIR}" "${BIN_OUTPUT_DIR}/scripting"
)

set(SCRIPT_SDK_DIR "${BIN_OUTPUT_DIR}/scripting/sdk")
set(SCRIPT_SDK_INCLUDE_DIR "${SCRIPT_SDK_DIR}/include")
set(SCRIPT_SDK_LIB_DIR "${SCRIPT_SDK_DIR}/lib")

file(GLOB_RECURSE ALL_SCRIPTING_FILES
        "${SCRIPTING_DIR}/*.cpp"
        "${SCRIPTING_DIR}/*.h"
        "${SCRIPTING_DIR}/*.hpp"
        "${SCRIPTING_DIR}/*.inl"
)

file(GLOB_RECURSE ALL_SCRIPTING_SDK_FILES
        "${CMAKE_SOURCE_DIR}/engine/include/scripting/*"
        "${CMAKE_SOURCE_DIR}/engine/include/gameobject/*"
        "${CMAKE_SOURCE_DIR}/engine/include/physics/*"
        "${CMAKE_SOURCE_DIR}/engine/include/window/*"
        "${CMAKE_SOURCE_DIR}/engine/include/debug/*"
        "${CMAKE_SOURCE_DIR}/engine/include/sound/*"
        "${CMAKE_SOURCE_DIR}/engine/include/emath/*"
        "${CMAKE_SOURCE_DIR}/engine/include/network/*"
        "${CMAKE_SOURCE_DIR}/engine/include/components/icomponent.h"
        "${CMAKE_SOURCE_DIR}/engine/include/utilities/macro.h"
        "${CMAKE_BINARY_DIR}/_deps/rttr-src/src/rttr/*"
        "${CMAKE_BINARY_DIR}/_deps/rttr-build/src/rttr/detail/base/version.h"
        "${CMAKE_BINARY_DIR}/_deps/joltphysics-src/Jolt/*"
        "${CMAKE_BINARY_DIR}/_deps/glfw-src/include/GLFW/*"
        "${CMAKE_BINARY_DIR}/_deps/enet-src/include/enet/*"
)

add_custom_target(UpdateScripts ALL
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${SCRIPTING_DIR}"
        "${BIN_OUTPUT_DIR}/scripting"
        SOURCES ${ALL_SCRIPTING_FILES}
        COMMENT "Copy scripting project to build"
        VERBATIM
)

add_custom_target(UpdateScriptSDK ALL
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${SCRIPT_SDK_INCLUDE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E rm -rf "${SCRIPT_SDK_LIB_DIR}"

        COMMAND ${CMAKE_COMMAND} -E make_directory "${SCRIPT_SDK_INCLUDE_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${SCRIPT_SDK_LIB_DIR}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${BIN_OUTPUT_DIR}/scripting/config"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/engine/rhi/enigma_rhi/context/include/"
        "${SCRIPT_SDK_INCLUDE_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/engine/include/"
        "${SCRIPT_SDK_INCLUDE_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/assimp-src/include/assimp/"
        "${SCRIPT_SDK_INCLUDE_DIR}/assimp"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/assimp-build/include/assimp/"
        "${SCRIPT_SDK_INCLUDE_DIR}/assimp"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/freetype-src/include"
        "${SCRIPT_SDK_INCLUDE_DIR}"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/rttr-src/src/rttr"
        "${SCRIPT_SDK_INCLUDE_DIR}/rttr"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/_deps/rttr-build/src/rttr/detail/base/version.h"
        "${SCRIPT_SDK_INCLUDE_DIR}/rttr/detail/base/version.h"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/joltphysics-src/Jolt"
        "${SCRIPT_SDK_INCLUDE_DIR}/Jolt"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/glfw-src/include/GLFW"
        "${SCRIPT_SDK_INCLUDE_DIR}/GLFW"

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_BINARY_DIR}/_deps/enet-src/include/enet"
        "${SCRIPT_SDK_INCLUDE_DIR}/enet"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_LINKER_FILE:enet>"
        "${SCRIPT_SDK_LIB_DIR}/$<TARGET_LINKER_FILE_NAME:enet>"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/_deps/miniaudio-src/miniaudio.h"
        "${SCRIPT_SDK_INCLUDE_DIR}/miniaudio.h"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_LINKER_FILE:Engine>"
        "${SCRIPT_SDK_LIB_DIR}/Engine.lib"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_LINKER_FILE:Jolt>"
        "${SCRIPT_SDK_LIB_DIR}/Jolt.lib"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_LINKER_FILE:rttr_core>"
        "${SCRIPT_SDK_LIB_DIR}/librttr_core.lib"

        SOURCES ${ALL_SCRIPTING_SDK_FILES}
        COMMENT "Copy scripting SDK to build"
        VERBATIM
)

function(configure_enigma_target TGT_NAME MODE_DEFINE)
    
    add_executable(${TGT_NAME} ${EDITOR_SOURCE} ${EDITOR_HEADER})
    
    target_compile_definitions(${TGT_NAME} PRIVATE ${MODE_DEFINE})
    
    target_link_libraries(${TGT_NAME} PRIVATE Engine)
    target_include_directories(${TGT_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/editor/include)

    target_sources(${TGT_NAME} PRIVATE Enigma.rc)

    add_dependencies(${TGT_NAME}
        UpdateResources
        UpdateScripts
        UpdateScriptSDK
    )

    add_custom_command(TARGET ${TGT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${TGT_NAME}>/assets
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/editor/config $<TARGET_FILE_DIR:${TGT_NAME}>/config
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/editor/internal_assets $<TARGET_FILE_DIR:${TGT_NAME}>/internal_assets
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Engine> $<TARGET_FILE_DIR:${TGT_NAME}>/
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:rttr_core> $<TARGET_FILE_DIR:${TGT_NAME}>/
    )

    if(MSVC)
        set_target_properties(${TGT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${TGT_NAME}>")
    endif()

endfunction()

configure_enigma_target(EditorEngine EDITOR_MODE)
configure_enigma_target(Game BUILD_MODE)