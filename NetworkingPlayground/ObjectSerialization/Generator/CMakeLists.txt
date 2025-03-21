
cmake_minimum_required (VERSION 3.12)

include("Utilities.cmake")

#Set paths
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH ROOT_DIR)

cmake_path(GET ROOT_DIR FILENAME ProjectId)
string(REPLACE " " "_" ProjectId ${ProjectId})
set(MAIN_PROJECT_NAME ${ProjectId})

set(SOURCE_DIR ${ROOT_DIR}/Source)
set(CODE_DIR ${SOURCE_DIR}/Code)


#Generate Project
project(${MAIN_PROJECT_NAME})
AddLibraryToCurrentProject(${ROOT_DIR} "${CODE_DIR}")
SetLibraryCppStandard(${MAIN_PROJECT_NAME} 20)


#Set options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	SetCompileOptionsMSVC(${MAIN_PROJECT_NAME})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	SetCompileOptionsGNU(${MAIN_PROJECT_NAME})
endif()
