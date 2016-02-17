CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
# prvd_cdef, prvd_deps, and prvd_srcs need to be set properly

# Build flag for the place auto detection engine.
# Set this to "yes" to enable the engine.
SET(enable_recog_engine "no")

# Common Profile
FILE(GLOB prvd_srcs *.cpp)

# Mobile Profile
IF("${PROFILE}" STREQUAL "mobile")
	FILE(GLOB_RECURSE prvd_srcs ${prvd_srcs} geofence/*.cpp)
	SET(prvd_deps ${prvd_deps} capi-geofence-manager)
	IF("${enable_recog_engine}" STREQUAL "yes")
		FILE(GLOB_RECURSE prvd_srcs ${prvd_srcs} recognition/*.cpp)
		SET(prvd_deps ${prvd_deps} capi-location-manager)
		SET(prvd_deps ${prvd_deps} capi-network-wifi)
	ELSE("${enable_recog_engine}" STREQUAL "yes")
		SET(prvd_cdef ${prvd_cdef} _DISABLE_RECOG_ENGINE_)
	ENDIF("${enable_recog_engine}" STREQUAL "yes")
ENDIF("${PROFILE}" STREQUAL "mobile")
