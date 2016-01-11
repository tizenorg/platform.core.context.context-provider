CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_SUBDIRECTORY(device)
GET_DIRECTORY_PROPERTY(target DIRECTORY device DEFINITION target)
GET_DIRECTORY_PROPERTY(deps DIRECTORY device DEFINITION deps)
SET(target_all "${target_all} -l${target}")
SET(deps_list "${deps_list} ${deps}")

SEPARATE_ARGUMENTS(deps_list)
LIST(REMOVE_DUPLICATES deps_list)
FOREACH(item IN LISTS deps_list)
	SET(deps_all "${deps_all} ${item}")
ENDFOREACH(item)

STRING(STRIP "${target_all}" target_all)
STRING(STRIP "${deps_all}" deps_all)
