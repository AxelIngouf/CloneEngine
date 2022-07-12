# MODEL

cmake_minimum_required(VERSION 3.16)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sources)

# assimp
set(ASSIMP_DIR ${PROJECT_SOURCE_DIR}/vendor/assimp)
set(ASSIMP_INCLUDE ${ASSIMP_DIR}/include)

set(ASSIMP_DEBUG ${ASSIMP_DIR}/lib/assimp-vc142-mtd)
set(ASSIMP_DEBUG_LIB ${ASSIMP_DEBUG}.lib)
set(ASSIMP_DEBUG_DLL ${ASSIMP_DEBUG}.dll)

set(ASSIMP_RELEASE ${ASSIMP_DIR}/lib/assimp-vc142-mt)
set(ASSIMP_RELEASE_LIB ${ASSIMP_RELEASE}.lib)
set(ASSIMP_RELEASE_DLL ${ASSIMP_RELEASE}.dll)

set(ASSIMP_LIBRARY 	optimized 	${ASSIMP_RELEASE_LIB}
					debug		${ASSIMP_DEBUG_LIB})

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
							${ASSIMP_INCLUDE})

target_link_libraries(${TARGET_NAME}
						PUBLIC
						${CORE_LIBRARY}
						PRIVATE
						${ASSIMP_LIBRARY})

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
    copy_if_different ${ASSIMP_RELEASE_DLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
	
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_DEBUG}:echo> $<${NOT_DEBUG}:"copy omitted for non-DEBUG build">
    copy_if_different ${ASSIMP_DEBUG_DLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
	
set(RELEASE_DLL ${RELEASE_DLL} ${ASSIMP_RELEASE_DLL} PARENT_SCOPE)
set(DEBUG_DLL ${DEBUG_DLL} ${ASSIMP_DEBUG_DLL} PARENT_SCOPE)

