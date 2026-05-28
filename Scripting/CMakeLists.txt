cmake_minimum_required(VERSION 3.25)
project(UserScripts)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(VCPKG_APPLOCAL_DEPS OFF CACHE BOOL "Disable vcpkg app-local deployment for standalone UserScripts builds" FORCE)
endif()

file(GLOB SCRIPTING_LIB_SOURCE CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

set(SCRIPTING_CONFIG_DIR "${CMAKE_CURRENT_SOURCE_DIR}/config")
file(MAKE_DIRECTORY "${SCRIPTING_CONFIG_DIR}")

if (TARGET Engine)
    set(GENERATED_REFLECTION_CPP "${CMAKE_BINARY_DIR}/editor/scripting/config/generated_reflection.cpp")
    set(USER_SCRIPTS_OUTPUT_DIR "${CMAKE_BINARY_DIR}/editor/scripting/config")
    set(USER_SCRIPTS_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    set(USER_SCRIPTS_LINK_LIBS
        Engine
    )
else()
    set(SCRIPT_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/sdk")
    set(GENERATED_REFLECTION_CPP "${SCRIPTING_CONFIG_DIR}/generated_reflection.cpp")
    set(USER_SCRIPTS_OUTPUT_DIR "${SCRIPTING_CONFIG_DIR}")

    add_library(Engine SHARED IMPORTED GLOBAL)
    set_target_properties(Engine PROPERTIES
        IMPORTED_IMPLIB "${SCRIPT_SDK_DIR}/lib/Engine.lib"
        IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/../Engine.dll"
    )

    set(USER_SCRIPTS_INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${SCRIPT_SDK_DIR}/include"
    )
    set(USER_SCRIPTS_LINK_LIBS
        Engine
        "${SCRIPT_SDK_DIR}/lib/Jolt.lib"
        "${SCRIPT_SDK_DIR}/lib/librttr_core.lib"
    )
    set(USER_SCRIPTS_COMPILE_DEFINITIONS
        RTTR_DLL
    )
endif()

set_source_files_properties(${GENERATED_REFLECTION_CPP} PROPERTIES GENERATED TRUE)

add_library(UserScripts SHARED
    ${SCRIPTING_LIB_SOURCE}
    ${GENERATED_REFLECTION_CPP}
)

target_link_libraries(UserScripts PUBLIC
    ${USER_SCRIPTS_LINK_LIBS}
)

target_include_directories(UserScripts PUBLIC
    ${USER_SCRIPTS_INCLUDE_DIRS}
)

if (USER_SCRIPTS_COMPILE_DEFINITIONS)
    target_compile_definitions(UserScripts PRIVATE ${USER_SCRIPTS_COMPILE_DEFINITIONS})
endif()

#Hot-reload support: Append a unique ID to the output name to allow multiple versions of the DLL to coexist
if(NOT DEFINED HOTRELOAD_ID)
    set(HOTRELOAD_ID 0)
endif()

set(USER_SCRIPT_OUTPUT_NAME "UserScripts_${HOTRELOAD_ID}")

set_target_properties(UserScripts PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${USER_SCRIPTS_OUTPUT_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${USER_SCRIPTS_OUTPUT_DIR}"
    PDB_OUTPUT_DIRECTORY "${USER_SCRIPTS_OUTPUT_DIR}"
    OUTPUT_NAME "${USER_SCRIPT_OUTPUT_NAME}"
)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set_target_properties(UserScripts PROPERTIES
        VS_GLOBAL_VcpkgEnabled "false"
        VS_GLOBAL_VcpkgApplocalDeps "false"
    )
endif()

foreach(CONFIG DEBUG RELEASE MINSIZEREL RELWITHDEBINFO)
    set_target_properties(UserScripts PROPERTIES
        "RUNTIME_OUTPUT_DIRECTORY_${CONFIG}" "${USER_SCRIPTS_OUTPUT_DIR}"
        "LIBRARY_OUTPUT_DIRECTORY_${CONFIG}" "${USER_SCRIPTS_OUTPUT_DIR}"
        "PDB_OUTPUT_DIRECTORY_${CONFIG}" "${USER_SCRIPTS_OUTPUT_DIR}"
    )
endforeach()

if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET UserScripts PROPERTY CXX_STANDARD 20)
endif()

if(MSVC)
    target_compile_options(UserScripts PRIVATE /W4)
    set_target_properties(UserScripts PROPERTIES
        APPEND_OV_PDB_NAME_STR "RELOAD"
        COMPILE_FLAGS "/Zi"
        LINK_FLAGS "/INCREMENTAL:NO"
    )
else()
    target_compile_options(UserScripts PRIVATE)
endif()
