CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
# prvd_cdef, prvd_deps, and prvd_srcs need to be set properly

# Build flag for the prediction engine.
# Set this to "yes" to enable the engine.
SET(enable_prediction_engine "no")

# Common Profile
FILE(GLOB prvd_srcs ./*.cpp)
FILE(GLOB prvd_srcs ${prvd_srcs} shared/*.cpp)
FILE(GLOB prvd_srcs ${prvd_srcs} app/*.cpp)
IF("${enable_prediction_engine}" STREQUAL "yes")
	FILE(GLOB prvd_srcs ${prvd_srcs} prediction/*.cpp)
ELSE("${enable_prediction_engine}" STREQUAL "yes")
	SET(prvd_cdef ${prvd_cdef} _DISABLE_PREDICTION_ENGINE_)
ENDIF("${enable_prediction_engine}" STREQUAL "yes")

SET(prvd_deps capi-system-runtime-info pkgmgr pkgmgr-info capi-appfw-package-manager)
SET(prvd_deps ${prvd_deps} capi-appfw-application capi-appfw-app-manager)
SET(prvd_deps ${prvd_deps} capi-media-sound-manager)

# Mobile Profile
IF("${PROFILE}" STREQUAL "mobile")
	FILE(GLOB prvd_srcs ${prvd_srcs} media/*.cpp)
	FILE(GLOB prvd_srcs ${prvd_srcs} social/*.cpp)
	SET(prvd_deps ${prvd_deps} contacts-service2 capi-content-media-content)
ENDIF("${PROFILE}" STREQUAL "mobile")

# Wearable Profile
#IF("${PROFILE}" STREQUAL "wearable")
#ENDIF("${PROFILE}" STREQUAL "wearable")

# TV Profile
IF("${PROFILE}" STREQUAL "tv")
	FILE(GLOB prvd_srcs ${prvd_srcs} media/*.cpp)
	SET(prvd_deps ${prvd_deps} capi-content-media-content)
ENDIF("${PROFILE}" STREQUAL "tv")
