#ROOT

cmake_minimum_required(VERSION 3.16)

# set a variable name for the project
set(PROJECT_NAME "RaceEngine")

# Create solution with a name and language CXX = C++ and C = C
project(${PROJECT_NAME} C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/CMake/)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if (MSVC)
    # remove default warning level from CMAKE_CXX_FLAGS_INIT
    string (REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS_INIT "${CMAKE_CXX_FLAGS_INIT}")
endif()

set(DEBUG_DLL)
set(RELEASE_DLL)
set(SHIPPING_DLL)


# Set projects name
set(CORE_LIBRARY core)
set(EDITOR_EXECUTABLE editor)
set(MATH_LIBRARY math)
set(MODEL_LIBRARY model)
set(PHYSIC_LIBRARY physic)
set(RENDER_LIBRARY render)
set(SOUND_LIBRARY sound)
set(SCRIPT_LIBRARY script)

# Call a sub CMakeLists to add projects
set(TARGET_NAME ${MATH_LIBRARY})
add_subdirectory(${PROJECT_SOURCE_DIR}/math)
set(TARGET_NAME ${CORE_LIBRARY})
add_subdirectory(${PROJECT_SOURCE_DIR}/core) # is included by all and use math

set(TARGET_NAME ${MODEL_LIBRARY})
add_subdirectory(${PROJECT_SOURCE_DIR}/model)
set(TARGET_NAME ${PHYSIC_LIBRARY})
add_subdirectory(${PROJECT_SOURCE_DIR}/physic)
set(TARGET_NAME ${RENDER_LIBRARY})
add_subdirectory(${PROJECT_SOURCE_DIR}/render)
set(TARGET_NAME ${SCRIPT_LIBRARY})
add_subdirectory(${PROJECT_SOURCE_DIR}/script)
set(TARGET_NAME ${SOUND_LIBRARY})
add_subdirectory(${PROJECT_SOURCE_DIR}/sound)


set(TARGET_NAME ${EDITOR_EXECUTABLE})
add_subdirectory(${PROJECT_SOURCE_DIR}/editor) # include all other

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${EDITOR_EXECUTABLE})

# Add shipping build configuration

SET(CMAKE_CXX_FLAGS_SHIPPING
    "-Wall -D_SHIPPING -DNDEBUG"
    CACHE STRING "Flags used by the C++ compiler during Shipping builds."
    FORCE )
SET(CMAKE_C_FLAGS_SHIPPING
    "-Wall"
    CACHE STRING "Flags used by the C compiler during Shipping builds."
    FORCE )
SET(CMAKE_EXE_LINKER_FLAGS_SHIPPING
    ""
    CACHE STRING "Flags used for linking binaries during Shipping builds."
    FORCE )
SET(CMAKE_SHARED_LINKER_FLAGS_SHIPPING
    ""
    CACHE STRING "Flags used by the shared libraries linker during Shipping builds."
    FORCE )
MARK_AS_ADVANCED(
    CMAKE_CXX_FLAGS_SHIPPING
    CMAKE_C_FLAGS_SHIPPING
    CMAKE_EXE_LINKER_FLAGS_SHIPPING
    CMAKE_SHARED_LINKER_FLAGS_SHIPPING )
	
set(CMAKE_CONFIGURATION_TYPES Debug Release Shipping CACHE STRING  
INTERNAL FORCE )
