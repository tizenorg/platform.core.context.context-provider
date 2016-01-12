CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(target "ctx-place")

# Build flag for the place auto detection engine.
# Set this to "yes" to enable the engine.
SET(enable_recog_engine "no")

# Common Profile
FILE(GLOB srcs *.cpp)

# Mobile Profile
IF("${PROFILE}" STREQUAL "mobile")
	FILE(GLOB_RECURSE srcs ${srcs} geofence/*.cpp)
	SET(deps "${deps} capi-geofence-manager")
	IF("${enable_recog_engine}" STREQUAL "yes")
		FILE(GLOB_RECURSE srcs ${srcs} recognition/*.cpp)
		SET(deps "${deps} capi-location-manager")
		SET(deps "${deps} capi-network-wifi")
	ENDIF("${enable_recog_engine}" STREQUAL "yes")
ENDIF("${PROFILE}" STREQUAL "mobile")

MESSAGE("Place Provider Sources: ${srcs}")

# Build
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(place_pkgs REQUIRED ${deps_common} ${deps})

FOREACH(flag ${place_pkgs_CFLAGS})
	SET(extra_cflags "${extra_cflags} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${srcs})
TARGET_LINK_LIBRARIES(${target} ${place_pkgs_LDFLAGS})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS ${extra_cflags})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_DEFINITIONS "LOG_TAG=\"CONTEXT-PLACE\"")
SET_TARGET_PROPERTIES(${target} PROPERTIES SOVERSION ${MAJORVER})
SET_TARGET_PROPERTIES(${target} PROPERTIES VERSION ${FULLVER})

IF(NOT "${enable_recog_engine}" STREQUAL "yes")
	SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_DEFINITIONS "_DISABLE_RECOG_ENGINE_")
ENDIF(NOT "${enable_recog_engine}" STREQUAL "yes")

# Install
INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RuntimeLibraries)
