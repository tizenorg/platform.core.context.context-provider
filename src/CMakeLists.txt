CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FUNCTION(add_provider prvd_name)
	ADD_SUBDIRECTORY(${prvd_name})
	GET_DIRECTORY_PROPERTY(srcs_subdir DIRECTORY ${prvd_name} DEFINITION prvd_srcs)
	GET_DIRECTORY_PROPERTY(deps_subdir DIRECTORY ${prvd_name} DEFINITION prvd_deps)
	GET_DIRECTORY_PROPERTY(cdef_subdir DIRECTORY ${prvd_name} DEFINITION prvd_cdef)
	SET(srcs_collected ${srcs_collected} ${srcs_subdir} PARENT_SCOPE)
	SET(deps_collected ${deps_collected} ${deps_subdir} PARENT_SCOPE)
	SET(cdef_collected ${cdef_collected} ${cdef_subdir} PARENT_SCOPE)
ENDFUNCTION(add_provider)


#add_provider(device)
#add_provider(statistics)
add_provider(place)
#add_provider(custom)


LIST(REMOVE_DUPLICATES deps_collected)

SET(sources ${sources} ${srcs_collected} PARENT_SCOPE)
SET(dependencies ${dependencies} ${deps_collected} PARENT_SCOPE)
SET(compile_defs ${compile_defs} ${cdef_collected} PARENT_SCOPE)
