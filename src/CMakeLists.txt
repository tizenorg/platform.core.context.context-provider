CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

ADD_SUBDIRECTORY(device)
GET_DIRECTORY_PROPERTY(target_sub DIRECTORY device DEFINITION target)
GET_DIRECTORY_PROPERTY(deps_sub DIRECTORY device DEFINITION deps)
SET(target_all "${target_all} -l${target_sub}")
SET(deps_list "${deps_list} ${deps_sub}")

ADD_SUBDIRECTORY(statistics)
GET_DIRECTORY_PROPERTY(target_sub DIRECTORY statistics DEFINITION target)
GET_DIRECTORY_PROPERTY(deps_sub DIRECTORY statistics DEFINITION deps)
SET(target_all "${target_all} -l${target_sub}")
SET(deps_list "${deps_list} ${deps_sub}")

SEPARATE_ARGUMENTS(deps_list)
LIST(REMOVE_DUPLICATES deps_list)
FOREACH(item IN LISTS deps_list)
	SET(deps_all "${deps_all} ${item}")
ENDFOREACH(item)

STRING(STRIP "${target_all}" target_all)
STRING(STRIP "${deps_all}" deps_all)
