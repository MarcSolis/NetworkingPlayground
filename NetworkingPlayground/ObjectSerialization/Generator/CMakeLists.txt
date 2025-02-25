
cmake_minimum_required (VERSION 3.12)

include("Utilities.cmake")


set(MAIN_PROJECT_NAME ObjectSerialization)
set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(SOURCE_DIR ${ROOT_DIR}/Source)
set(CODE_DIR ${SOURCE_DIR}/Code)

project(${MAIN_PROJECT_NAME})

AddLibraryToCurrentProject(${ROOT_DIR} "${CODE_DIR}")
set_target_properties(${MAIN_PROJECT_NAME} PROPERTIES COMPILE_WARNING_AS_ERROR ON)
SetLibraryCppStandard(${MAIN_PROJECT_NAME} 20)

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	target_compile_options(${MAIN_PROJECT_NAME} PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/bigobj>)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set_target_properties(${MAIN_PROJECT_NAME} PROPERTIES CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER ON)
endif()
