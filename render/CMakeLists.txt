# RENDER

cmake_minimum_required(VERSION 3.16)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/sources)

# Vulkan
find_package(Vulkan REQUIRED)

# glfw
set(GLFW_DIR ${PROJECT_SOURCE_DIR}/vendor/glfw)
set(GLFW_INCLUDE ${GLFW_DIR}/include)

set(GLFW_DEBUG ${GLFW_DIR}/lib/Debug)
set(GLFW_DEBUG_LIB ${GLFW_DEBUG}/glfw3dll.lib)
set(GLFW_DEBUG_DLL ${GLFW_DEBUG}/glfw3.dll)

set(GLFW_RELEASE ${GLFW_DIR}/lib/Release)
set(GLFW_RELEASE_LIB ${GLFW_RELEASE}/glfw3dll.lib)
set(GLFW_RELEASE_DLL ${GLFW_RELEASE}/glfw3.dll)

set(GLFW_LIBRARY 	optimized 	${GLFW_RELEASE_LIB}
					debug		${GLFW_DEBUG_LIB})

# imgui
set(IMGUI_SOURCES ${PROJECT_SOURCE_DIR}/vendor/imgui)
file(GLOB IMGUI_FILES "${IMGUI_SOURCES}/*.cpp" "${IMGUI_SOURCES}/*.h")
set(IMGUI_INCLUDE ${PROJECT_SOURCE_DIR}/vendor)

# VMA
set(VMA_SOURCES ${PROJECT_SOURCE_DIR}/vendor/VulkanMemoryAllocator)
file(GLOB VMA_FILES "${VMA_SOURCES}/*.cpp" "${VMA_SOURCES}/*.h")
set(VMA_INCLUDE ${PROJECT_SOURCE_DIR}/vendor)

# Fetch project source and header files
include(${CMAKE_CURRENT_SOURCE_DIR}/CMake/sources.cmake)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})

# Create project
add_library(${TARGET_NAME} #SHARED
			${SOURCE_FILES}
			${IMGUI_FILES}
			${VMA_FILES})
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
							${Vulkan_INCLUDE_DIR}
							${GLFW_INCLUDE}
							${IMGUI_INCLUDE}
							${VMA_INCLUDE})
							
target_link_libraries(${TARGET_NAME}
						PUBLIC
						${CORE_LIBRARY}
						${MODEL_LIBRARY}
						PRIVATE
						${Vulkan_LIBRARY}
						${GLFW_LIBRARY})

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
    copy_if_different ${GLFW_RELEASE_DLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
	
add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E
    $<${NOT_DEBUG}:echo> $<${NOT_DEBUG}:"copy omitted for non-DEBUG build">
    copy_if_different ${GLFW_DEBUG_DLL} $<TARGET_FILE_DIR:${TARGET_NAME}>)
	
set(RELEASE_DLL ${RELEASE_DLL} ${GLFW_RELEASE_DLL} PARENT_SCOPE)
set(DEBUG_DLL ${DEBUG_DLL} ${GLFW_DEBUG_DLL} PARENT_SCOPE)

