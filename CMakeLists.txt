cmake_minimum_required(VERSION 3.25)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project(EnigmaEngine)

set(CMAKE_CXX_STANDARD 20)

set(ENGINE_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/engine/shaders" CACHE PATH "Path to engine shaders")
set(SCRIPTING_DIR "${CMAKE_CURRENT_SOURCE_DIR}/scripting" CACHE PATH "Path to scripting")

add_subdirectory(engine)
add_subdirectory(editor)
add_subdirectory(scripting)

message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")