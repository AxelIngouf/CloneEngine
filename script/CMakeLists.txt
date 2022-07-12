# SCRIPT

cmake_minimum_required(VERSION 3.16)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sources)

# glfw
set(LUA_DIR ${PROJECT_SOURCE_DIR}/vendor/lua)
set(LUA_INCLUDE ${LUA_DIR}/include)

set(LUA_LIB ${LUA_DIR}/lib/lua54.lib)
set(LUA_DLL ${LUA_DIR}/lib/lua54.dll)

# Fetch project list of source and header files
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/sources.cmake)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Create project
add_library(${TARGET_NAME} #SHARED
			${SOURCE_FILES})
set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_property(TARGET ${TARGET_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
if (MSVC)
	target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
endif()

include (GenerateExportHeader)
GENERATE_EXPORT_HEADER(${TARGET_NAME})

target_include_directories(${TARGET_NAME}
							PUBLIC
							${INCLUDE_DIR}
							${CMAKE_CURRENT_BINARY_DIR} #include import/export generated file
							PRIVATE
							${LUA_INCLUDE})

target_link_libraries(${TARGET_NAME}
						PUBLIC
						${CORE_LIBRARY}
						PRIVATE
						${LUA_LIB})

# copy dll into executable directory
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    copy_if_different ${LUA_DLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)

set(RELEASE_DLL ${RELEASE_DLL} ${LUA_DLL} PARENT_SCOPE)
set(DEBUG_DLL ${DEBUG_DLL} ${LUA_DLL} PARENT_SCOPE)

