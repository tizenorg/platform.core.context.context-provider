CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

FUNCTION(add_provider prvd_name)
	ADD_SUBDIRECTORY(${prvd_name})
	GET_DIRECTORY_PROPERTY(target_sub DIRECTORY ${prvd_name} DEFINITION target)
	GET_DIRECTORY_PROPERTY(deps_sub DIRECTORY ${prvd_name} DEFINITION deps)
	SET(target_all "${target_all} -l${target_sub}" PARENT_SCOPE)
	SET(deps_list "${deps_list} ${deps_sub}" PARENT_SCOPE)
ENDFUNCTION(add_provider)

add_provider(device)
add_provider(statistics)
add_provider(place)

SEPARATE_ARGUMENTS(deps_list)
LIST(REMOVE_DUPLICATES deps_list)
FOREACH(item IN LISTS deps_list)
	SET(deps_all "${deps_all} ${item}")
ENDFOREACH(item)

STRING(STRIP "${target_all}" target_all)
STRING(STRIP "${deps_all}" deps_all)
