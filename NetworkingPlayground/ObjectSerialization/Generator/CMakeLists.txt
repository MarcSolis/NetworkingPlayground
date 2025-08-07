
cmake_minimum_required (VERSION 3.12)

include("Utilities.cmake")

#Set paths
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH ROOT_DIR)

cmake_path(GET ROOT_DIR FILENAME ProjectId)
string(REPLACE " " "_" ProjectId ${ProjectId})

set(SOURCE_DIR ${ROOT_DIR}/Source)
set(CODE_DIR ${SOURCE_DIR}/Code)
set(TEST_DIR ${SOURCE_DIR}/Code)
set(BUILDING_PROJECT_NAME ${ProjectId})


#Optional params
option(TEST "Generate test project" OFF)

if(TEST)
	set(BUILDING_PROJECT_NAME "${BUILDING_PROJECT_NAME}_Test")
	message(Building " ${BUILDING_PROJECT_NAME}")

	GetRepoBasePath("${ROOT_DIR}" REPO_DIR)
	set(GOOGLETEST_SOURCE_DIR "${REPO_DIR}/External/GoogleTest")
	set(GOOGLETEST_BINARY_DIR "${CMAKE_BINARY_DIR}/external/GoogleTest-build")

	if (MSVC)
	  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")  # /MD
	  # Use MultiThreadedDebugDLL for Debug config
	endif()

	#Generate GoogleTest Project
	project(${BUILDING_PROJECT_NAME})
	set(gtest_force_shared_crt on)
	add_subdirectory(${GOOGLETEST_SOURCE_DIR} ${GOOGLETEST_BINARY_DIR})
	AddLibraryToCurrentTestProject(${ROOT_DIR} "${SOURCE_DIR}")
	SetLibraryCppStandard(${BUILDING_PROJECT_NAME} 20)
else()
	message(Building " ${BUILDING_PROJECT_NAME}")
	#Generate Main Project
	project(${BUILDING_PROJECT_NAME})
	AddLibraryToCurrentProject(${ROOT_DIR} "${CODE_DIR}")
	SetLibraryCppStandard(${BUILDING_PROJECT_NAME} 20)
endif()


#Set options
if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	SetCompileOptionsMSVC(${BUILDING_PROJECT_NAME})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	SetCompileOptionsGNU(${BUILDING_PROJECT_NAME})
endif()
