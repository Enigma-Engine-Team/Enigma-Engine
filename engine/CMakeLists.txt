cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(ENGINE_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shaders")

set(JSON_BuildTests OFF CACHE INTERNAL "")

set(FT_WITH_ZLIB OFF CACHE BOOL "" FORCE)
set(FT_WITH_BZIP2 OFF CACHE BOOL "" FORCE)
set(FT_WITH_PNG OFF CACHE BOOL "" FORCE)
set(FT_WITH_HARFBUZZ OFF CACHE BOOL "" FORCE)
set(FT_WITH_BROTLI OFF CACHE BOOL "" FORCE)
set(FT_WITH_UNIT_TESTS OFF CACHE BOOL "" FORCE)

set(JPH_BUILD_SHARED_LIBRARY OFF CACHE BOOL "" FORCE)
set(JPH_BUILD_STATIC_LIBRARY ON CACHE BOOL "" FORCE)
set(JPH_BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(JPH_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(JPH_ENABLE_DX12 OFF CACHE BOOL "" FORCE)
set(JPH_ENABLE_VULKAN OFF CACHE BOOL "" FORCE)
set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF CACHE BOOL "" FORCE)

set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC ON CACHE BOOL "" FORCE)
set(BUILD_PACKAGE OFF CACHE BOOL "" FORCE)
set(BUILD_WITH_RTTI ON CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_DOCUMENTATION OFF CACHE BOOL "" FORCE)
set(BUILD_INSTALLER OFF CACHE BOOL "" FORCE)
set(USE_PCH OFF CACHE BOOL "" FORCE)
set(CUSTOM_DOXYGEN_STYLE OFF CACHE BOOL "" FORCE)

set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_SAMPLES OFF CACHE INTERNAL "")
set(ASSIMP_INSTALL OFF CACHE INTERNAL "")
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE INTERNAL "")
set(ASSIMP_NO_EXPORT ON CACHE INTERNAL "")
set(ASSIMP_BUILD_DRACO OFF CACHE INTERNAL "")
set(ASSIMP_BUILD_ASSIMP_VIEW OFF CACHE INTERNAL "")
set(ASSIMP_IGNORE_GIT_HASH ON CACHE INTERNAL "")
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE INTERNAL "")

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG v6.0.5
)

FetchContent_Declare(
    rttr
    GIT_REPOSITORY https://github.com/rttrorg/rttr.git
    GIT_TAG master
)

FetchContent_Declare(
    joltphysics
    GIT_REPOSITORY https://github.com/jrouwe/JoltPhysics.git
    GIT_TAG v5.5.0
)

FetchContent_Declare(
    enet
    GIT_REPOSITORY https://github.com/lsalzman/enet.git
    GIT_TAG v1.3.18
)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.7-docking
)

FetchContent_Declare(
    imgizmo
    GIT_REPOSITORY https://github.com/cedricguillemet/imguizmo
    GIT_TAG 1.9
)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
    GIT_TAG        VER-2-14-3
)

FetchContent_Declare(
    miniaudio
    GIT_REPOSITORY https://github.com/mackron/miniaudio.git
    GIT_TAG 0.11.25
)

FetchContent_MakeAvailable(glfw rttr joltphysics enet json assimp imgui imgizmo freetype miniaudio)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgizmo_SOURCE_DIR}/src/ImGuizmo.cpp
)

add_subdirectory("${joltphysics_SOURCE_DIR}/Build" "${joltphysics_BINARY_DIR}")

file(GLOB_RECURSE ENGINE_SOURCE "src/*.cpp")
file(GLOB_RECURSE ENGINE_HEADER "include/*.h" "include/*.inl")

add_subdirectory(rhi)

add_library(Engine SHARED ${ENGINE_SOURCE} ${ENGINE_HEADER} ${IMGUI_SOURCES})

target_include_directories(Engine PUBLIC
    include
    ${joltphysics_SOURCE_DIR}
    ${enet_SOURCE_DIR}/include
)

target_link_libraries(Engine PUBLIC enet glfw RTTR::Core nlohmann_json::nlohmann_json EnigmaRHI Jolt ws2_32 winmm assimp freetype miniaudio)

target_compile_definitions(Engine PUBLIC
        JPH_USE_SSE4_2
        JPH_DEBUG_RENDERER
)

target_compile_definitions(Engine PRIVATE
        BUILDING_ENIGMA_ENGINE
)

target_compile_definitions(Engine PRIVATE "IMGUI_API=__declspec(dllexport)")
target_compile_definitions(Engine INTERFACE "IMGUI_API=__declspec(dllimport)")
target_compile_definitions(Engine PRIVATE "IMGUIZMO_NAMESPACE=ImGuizmo")

target_include_directories(Engine PUBLIC
        include
        ${joltphysics_SOURCE_DIR}
        ${imgui_SOURCE_DIR}
        ${imgui_SOURCE_DIR}/backends
        ${imgizmo_SOURCE_DIR}
)

add_custom_command(TARGET Engine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:Engine>
        ${CMAKE_BINARY_DIR}/editor
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_FILE:rttr_core>
        ${CMAKE_BINARY_DIR}/editor
)

if(MSVC)
  target_compile_options(Engine PRIVATE /W4)
endif()