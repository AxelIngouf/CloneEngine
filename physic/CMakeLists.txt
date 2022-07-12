# PHYSIC

cmake_minimum_required(VERSION 3.16)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sources)

# PhysX
set(PHYSX_DIR ${PROJECT_SOURCE_DIR}/vendor/PhysX)
set(PHYSX_INCLUDE ${PHYSX_DIR}/include)

set(PHYSX_DEBUG ${PHYSX_DIR}/lib/debug)
set(PHYSX_RELEASE ${PHYSX_DIR}/lib/release)

set(PHYSX_LIBRARY
				optimized ${PHYSX_RELEASE}/PhysX_64.lib
				optimized ${PHYSX_RELEASE}/PhysXCommon_64.lib
				optimized ${PHYSX_RELEASE}/PhysXCooking_64.lib
				optimized ${PHYSX_RELEASE}/PhysXFoundation_64.lib
				optimized ${PHYSX_RELEASE}/PhysXPvdSDK_static_64.lib
				optimized ${PHYSX_RELEASE}/PhysXVehicle_static_64.lib
				optimized ${PHYSX_RELEASE}/PhysXExtensions_static_64.lib
				debug ${PHYSX_DEBUG}/PhysX_64.lib
				debug ${PHYSX_DEBUG}/PhysXCommon_64.lib
				debug ${PHYSX_DEBUG}/PhysXCooking_64.lib
				debug ${PHYSX_DEBUG}/PhysXFoundation_64.lib
				debug ${PHYSX_DEBUG}/PhysXPvdSDK_static_64.lib
				debug ${PHYSX_DEBUG}/PhysXVehicle_static_64.lib
				debug ${PHYSX_DEBUG}/PhysXExtensions_static_64.lib)

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
							${PHYSX_INCLUDE})

target_link_libraries(${TARGET_NAME}
						PUBLIC
						${CORE_LIBRARY}
						${MODEL_LIBRARY}
						PRIVATE
						${PHYSX_LIBRARY})

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
    copy_if_different ${PHYSX_RELEASE}/PhysX_64.dll $<TARGET_FILE_DIR:${TARGET_NAME}>)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_RELEASE}:echo> $<${NOT_RELEASE}:"copy omitted for non-release build">
    copy_if_different ${PHYSX_RELEASE}/PhysXCommon_64.dll $<TARGET_FILE_DIR:${TARGET_NAME}>)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_RELEASE}:echo> $<${NOT_RELEASE}:"copy omitted for non-release build">
    copy_if_different ${PHYSX_RELEASE}/PhysXCooking_64.dll $<TARGET_FILE_DIR:${TARGET_NAME}>)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_RELEASE}:echo> $<${NOT_RELEASE}:"copy omitted for non-release build">
    copy_if_different ${PHYSX_RELEASE}/PhysXFoundation_64.dll $<TARGET_FILE_DIR:${TARGET_NAME}>)

add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_DEBUG}:echo> $<${NOT_DEBUG}:"copy omitted for non-DEBUG build">
    copy_if_different ${PHYSX_DEBUG}/PhysX_64.dll $<TARGET_FILE_DIR:${TARGET_NAME}>)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_DEBUG}:echo> $<${NOT_DEBUG}:"copy omitted for non-DEBUG build">
    copy_if_different ${PHYSX_DEBUG}/PhysXCommon_64.dll $<TARGET_FILE_DIR:${TARGET_NAME}>)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_DEBUG}:echo> $<${NOT_DEBUG}:"copy omitted for non-DEBUG build">
    copy_if_different ${PHYSX_DEBUG}/PhysXCooking_64.dll $<TARGET_FILE_DIR:${TARGET_NAME}>)
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_DEBUG}:echo> $<${NOT_DEBUG}:"copy omitted for non-DEBUG build">
    copy_if_different ${PHYSX_DEBUG}/PhysXFoundation_64.dll $<TARGET_FILE_DIR:${TARGET_NAME}>)
	
set(RELEASE_DLL ${RELEASE_DLL}
				${PHYSX_RELEASE}/PhysX_64.dll
				${PHYSX_RELEASE}/PhysXCommon_64.dll
				${PHYSX_RELEASE}/PhysXCooking_64.dll
				${PHYSX_RELEASE}/PhysXFoundation_64.dll
	PARENT_SCOPE)
set(DEBUG_DLL ${DEBUG_DLL}
				${PHYSX_DEBUG}/PhysX_64.dll
				${PHYSX_DEBUG}/PhysXCommon_64.dll
				${PHYSX_DEBUG}/PhysXCooking_64.dll
				${PHYSX_DEBUG}/PhysXFoundation_64.dll
	PARENT_SCOPE)
	
	