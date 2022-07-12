# SOUND

cmake_minimum_required(VERSION 3.16)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sources)

# FMOD
set(FMOD_DIR ${PROJECT_SOURCE_DIR}/vendor/FMOD)
set(FMOD_INCLUDE ${FMOD_DIR}/inc)

set(FMOD_DEBUG ${FMOD_DIR}/lib)
set(FMOD_DEBUG_LIB ${FMOD_DEBUG}/fmodL_vc.lib)
set(FMOD_DEBUG_DLL ${FMOD_DEBUG}/fmodL.dll)

set(FMOD_RELEASE ${FMOD_DIR}/lib)
set(FMOD_RELEASE_LIB ${FMOD_RELEASE}/fmod_vc.lib)
set(FMOD_RELEASE_DLL ${FMOD_RELEASE}/fmod.dll)

set(FMOD_LIBRARY 	optimized 	${FMOD_RELEASE_LIB}
					debug		${FMOD_DEBUG_LIB})

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
							${FMOD_INCLUDE})

target_link_libraries(${TARGET_NAME}
						PUBLIC
						${CORE_LIBRARY}
						PRIVATE
						${FMOD_LIBRARY})

# copy dll into executable directory
set(NOT_RELEASE $<NOT:$<CONFIG:Release>>) # abbreviation
set(NOT_DEBUG $<NOT:$<CONFIG:Debug>>) # abbreviation
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E
    # do nothing for non-Release build
    $<${NOT_RELEASE}:echo>
    # output text to make clear that the copy command is not actually issued
    $<${NOT_RELEASE}:"copy omitted for non-release build">
    # the actual copy command, which is overridden by the "echo" above
    # in the case of a non-release build
    copy_if_different ${FMOD_RELEASE_DLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
	
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_DEBUG}:echo> $<${NOT_DEBUG}:"copy omitted for non-DEBUG build">
    copy_if_different ${FMOD_DEBUG_DLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
	
set(RELEASE_DLL ${RELEASE_DLL} ${FMOD_RELEASE_DLL} PARENT_SCOPE)
set(DEBUG_DLL ${DEBUG_DLL} ${FMOD_DEBUG_DLL} PARENT_SCOPE)

