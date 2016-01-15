CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(target "ctx-statistics")
SET(definitions LOG_TAG=\"CONTEXT-STATS\")

# Build flag for the prediction engine.
# Set this to "yes" to enable the engine.
SET(enable_prediction_engine "no")

# Common Options
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/shared
)

# Common Profile
FILE(GLOB srcs ./*.cpp)
FILE(GLOB srcs ${srcs} shared/*.cpp)
FILE(GLOB srcs ${srcs} app/*.cpp)
IF("${enable_prediction_engine}" STREQUAL "yes")
	FILE(GLOB srcs ${srcs} prediction/*.cpp)
ELSE("${enable_prediction_engine}" STREQUAL "yes")
	SET(definitions ${definitions} "_DISABLE_PREDICTION_ENGINE_")
ENDIF("${enable_prediction_engine}" STREQUAL "yes")

SET(deps "capi-system-runtime-info pkgmgr pkgmgr-info capi-appfw-package-manager")
SET(deps "${deps} capi-appfw-application capi-appfw-app-manager")
SET(deps "${deps} capi-media-sound-manager")

# Mobile Profile
IF("${PROFILE}" STREQUAL "mobile")
	FILE(GLOB srcs ${srcs} media/*.cpp)
	FILE(GLOB srcs ${srcs} social/*.cpp)
	SET(deps "${deps} contacts-service2 capi-content-media-content")
ENDIF("${PROFILE}" STREQUAL "mobile")

# Wearable Profile
#IF("${PROFILE}" STREQUAL "wearable")
#ENDIF("${PROFILE}" STREQUAL "wearable")

# TV Profile
IF("${PROFILE}" STREQUAL "tv")
	FILE(GLOB srcs ${srcs} media/*.cpp)
	SET(deps "${deps} capi-content-media-content")
ENDIF("${PROFILE}" STREQUAL "tv")

MESSAGE("Statistics Provider Sources: ${srcs}")

# Build
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(stats_pkgs REQUIRED ${deps_common} ${deps})

FOREACH(flag ${stats_pkgs_CFLAGS})
	SET(extra_cflags "${extra_cflags} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${srcs})
TARGET_LINK_LIBRARIES(${target} ${stats_pkgs_LDFLAGS})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS ${extra_cflags})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_DEFINITIONS "${definitions}")
SET_TARGET_PROPERTIES(${target} PROPERTIES SOVERSION ${MAJORVER})
SET_TARGET_PROPERTIES(${target} PROPERTIES VERSION ${FULLVER})

# Install
INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RuntimeLibraries)
