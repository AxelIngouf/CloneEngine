#!/bin/sh

SOURCES_VAR=SOURCE_FILES

#------------------------------------------------------------------------------

cd "../core"
mkdir CMake

# Using sed to escape filenames with spaces in them.
BASE_SOURCES=$( find sources/ -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp" -or -name "*.inl" -or -name "*.txt" | sed 's/ /\\ /g')

echo "set($SOURCES_VAR
$BASE_SOURCES )" > CMake/sources.cmake

#------------------------------------------------------------------------------

cd "../editor"
mkdir CMake

# Using sed to escape filenames with spaces in them.
BASE_SOURCES=$( find sources/ -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp" -or -name "*.inl" -or -name "*.txt" | sed 's/ /\\ /g')

echo "set($SOURCES_VAR
$BASE_SOURCES
$IMGUI_SOURCES )" > CMake/sources.cmake

#------------------------------------------------------------------------------

cd "../model"
mkdir CMake

# Using sed to escape filenames with spaces in them.
BASE_SOURCES=$( find sources/ -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp" -or -name "*.inl" -or -name "*.txt" | sed 's/ /\\ /g')

echo "set($SOURCES_VAR
$BASE_SOURCES )" > CMake/sources.cmake

#------------------------------------------------------------------------------

cd "../physic"
mkdir CMake

# Using sed to escape filenames with spaces in them.
BASE_SOURCES=$( find sources/ -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp" -or -name "*.inl" -or -name "*.txt" | sed 's/ /\\ /g')

echo "set($SOURCES_VAR
$BASE_SOURCES )" > CMake/sources.cmake

#------------------------------------------------------------------------------

cd "../render"
mkdir CMake

# Using sed to escape filenames with spaces in them.
BASE_SOURCES=$( find sources/ -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp" -or -name "*.inl" -or -name "*.txt" -or -name "*.frag" -or -name "*.vert" | sed 's/ /\\ /g')

echo "set($SOURCES_VAR
$BASE_SOURCES )" > CMake/sources.cmake

#------------------------------------------------------------------------------

cd "../script"
mkdir CMake

# Using sed to escape filenames with spaces in them.
BASE_SOURCES=$( find sources/ -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp" -or -name "*.inl" -or -name "*.txt" | sed 's/ /\\ /g')

echo "set($SOURCES_VAR
$BASE_SOURCES )" > CMake/sources.cmake

#------------------------------------------------------------------------------

cd "../sound"
mkdir CMake

# Using sed to escape filenames with spaces in them.
BASE_SOURCES=$( find sources/ -type f -name "*.cpp" -or -name "*.h" -or -name "*.hpp" -or -name "*.inl" -or -name "*.txt" | sed 's/ /\\ /g')

echo "set($SOURCES_VAR
$BASE_SOURCES )" > CMake/sources.cmake

#------------------------------------------------------------------------------
