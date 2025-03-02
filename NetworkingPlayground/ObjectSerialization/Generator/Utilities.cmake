cmake_minimum_required (VERSION 3.12)

function(GetDefaultCppHeaderFileExtensions out_extensions)
	set(${out_extensions} "*.h" "*.hpp" "*.inl" PARENT_SCOPE)
endfunction()

function(GetDefaultCppSourceFileExtensions out_extensions)
	set(${out_extensions} "*.c" "*.cpp" PARENT_SCOPE)
endfunction()

function(AppendCodeFilesRecursively out_header_files out_source_files top_dir relative_dirs)
	GetDefaultCppHeaderFileExtensions(AppendCodeFilesRecursively_HEADER_EXTENSIONS)
	GetDefaultCppSourceFileExtensions(AppendCodeFilesRecursively_SOURCE_EXTENSIONS)

	set(AppendCodeFilesRecursively_${out_header_files} ${${out_header_files}})
	set(AppendCodeFilesRecursively_${out_source_files} ${${out_source_files}})

	AppendFilesRecursively(AppendCodeFilesRecursively_${out_header_files} ${top_dir} "${relative_dirs}" "${AppendCodeFilesRecursively_HEADER_EXTENSIONS}")
	AppendFilesRecursively(AppendCodeFilesRecursively_${out_source_files} ${top_dir} "${relative_dirs}" "${AppendCodeFilesRecursively_SOURCE_EXTENSIONS}")

	set(${out_header_files} ${AppendCodeFilesRecursively_${out_header_files}} PARENT_SCOPE)
	set(${out_source_files} ${AppendCodeFilesRecursively_${out_source_files}} PARENT_SCOPE)
endfunction()

function(AppendFilesRecursively out_files top_dir relative_dirs extensions)
	foreach (relative_dir ${relative_dirs})
		set(AppendFilesRecursively_EXTENSIONS ${extensions})
		list(TRANSFORM AppendFilesRecursively_EXTENSIONS PREPEND ${relative_dir}/)

		file(GLOB_RECURSE AppendFilesRecursively_FILES RELATIVE ${top_dir} ${AppendFilesRecursively_EXTENSIONS})
		set(AppendFilesRecursively_${out_files} ${AppendFilesRecursively_${out_files}} ${AppendFilesRecursively_FILES})
    endforeach ()
	
	set(AppendFilesRecursively_${out_files} ${${out_files}} ${AppendFilesRecursively_${out_files}})

	set(${out_files} ${AppendFilesRecursively_${out_files}} PARENT_SCOPE)
endfunction()

function(AddIncludeDirectoriesToCurrentProject scope include_dirs)
	foreach (include_dir ${include_dirs})
		target_include_directories(${PROJECT_NAME} ${scope} ${include_dir})
    endforeach ()
endfunction()

function(AddLibraryToCurrentProject root_dir source_dir)
	AppendCodeFilesRecursively(AddLibraryToCurrentProject_CODE_FILES AddLibraryToCurrentProject_CODE_FILES ${PROJECT_SOURCE_DIR} "${source_dir}")
	AddExecutableToCurrentProject(${root_dir} "${AddLibraryToCurrentProject_CODE_FILES}" ${source_dir})
endfunction()

function(AddExecutableToCurrentProject root_dir code_files public_include_dirs)
	source_group(TREE ${root_dir} FILES ${code_files})

	add_executable(${PROJECT_NAME} ${code_files})

	AddIncludeDirectoriesToCurrentProject(PUBLIC "${public_include_dirs}")
endfunction()

function(SetLibraryCppStandard target_name cpp_standard)
	set_target_properties(${target_name} PROPERTIES
		CXX_STANDARD ${cpp_standard}
		CXX_STANDARD_REQUIRED ON)
endfunction()

function(SetCompileOptionsMSVC project_name)
	target_compile_options(${project_name} PUBLIC $<$<CXX_COMPILER_ID:MSVC>:/bigobj>)
	target_compile_options(${project_name} PRIVATE /WX)	#Enable Warnings as errors
endfunction()

function(SetCompileOptionsGNU project_name)
	set_target_properties(${MAIN_PROJECT_NAME} PROPERTIES CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER ON)
	add_compile_options(-Werror)	#Enable Warnings as errors
endfunction()
