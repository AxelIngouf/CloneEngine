# MATH

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git") # if git is on this machine and ${PROJECT_SOURCE_DIR} is a git repository
# Update submodules as needed
    option(GIT_SUBMODULE "Check submodules during build" ON)
    if(GIT_SUBMODULE)
        message(STATUS "Submodule update")
        execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive --remote
                        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                        RESULT_VARIABLE GIT_SUBMOD_RESULT)
        if(NOT GIT_SUBMOD_RESULT EQUAL "0")
            message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
        endif()
    endif()
endif()

set(MATH_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../extern/math/libMaths)

if(NOT EXISTS ${MATH_DIR}/CMakeLists.txt)
    message(FATAL_ERROR "The submodules were not downloaded! GIT_SUBMODULE was turned off or failed. Please update submodules and try again.")
else ()
	set(LIB_MATHS ${MATH_LIBRARY})
	add_subdirectory(${MATH_DIR} ../build)
endif()