# EDITOR

cmake_minimum_required(VERSION 3.16)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sources)

# Fetch project list of source and header files
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/sources.cmake)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Create project
add_executable(${TARGET_NAME}
				${SOURCE_FILES})
set_property(TARGET ${TARGET_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
set_property(TARGET ${TARGET_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
if (MSVC)
	target_compile_options(${TARGET_NAME} PRIVATE /W4 /WX)
endif()

target_include_directories(${TARGET_NAME}
							PRIVATE
							${INCLUDE_DIR})
							
target_link_libraries(${TARGET_NAME}
						PRIVATE
						${MODEL_LIBRARY}
						${PHYSIC_LIBRARY}
						${RENDER_LIBRARY}
						${SOUND_LIBRARY}
						${SCRIPT_LIBRARY}
						${WINDOW_LIBRARY})
						
# copy dll into executable directory
set(NOT_RELEASE $<NOT:$<CONFIG:Release>>) # abbreviation
set(NOT_DEBUG $<NOT:$<CONFIG:Debug>>) # abbreviation
					
foreach(DLL IN LISTS RELEASE_DLL)
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
		$<${NOT_RELEASE}:echo> $<${NOT_RELEASE}:"copy omitted for non-release build">
		copy_if_different ${DLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
endforeach()
					
foreach(DLL IN LISTS DEBUG_DLL)
	add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
		$<${NOT_DEBUG}:echo> $<${NOT_DEBUG}:"copy omitted for non-DEBUG build">
		copy_if_different ${DLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
endforeach()

