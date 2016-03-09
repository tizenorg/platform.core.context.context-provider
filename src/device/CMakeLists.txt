CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
# prvd_cdef, prvd_deps, and prvd_srcs need to be set properly

# Common Profile
FILE(GLOB prvd_srcs ./*.cpp system/alarm.cpp system/time.cpp)
SET(prvd_deps vconf capi-system-info capi-system-device capi-system-runtime-info)

# Common Profile
SET(srcs ${srcs} system/alarm.cpp system/time.cpp)

# Mobile Profile
IF("${PROFILE}" STREQUAL "mobile")
	SET(prvd_deps ${prvd_deps} capi-network-bluetooth capi-network-wifi)
	SET(prvd_deps ${prvd_deps} capi-telephony tapi msg-service capi-messaging-email motion)
	FILE(GLOB_RECURSE prvd_srcs ${prvd_srcs} activity/*.cpp)
	FILE(GLOB_RECURSE prvd_srcs ${prvd_srcs} system/*.cpp)
	FILE(GLOB_RECURSE prvd_srcs ${prvd_srcs} social/*.cpp)
ENDIF("${PROFILE}" STREQUAL "mobile")

# Wearable Profile
IF("${PROFILE}" STREQUAL "wearable")
	SET(prvd_deps ${prvd_deps} capi-network-bluetooth capi-network-wifi)
	SET(prvd_deps ${prvd_deps} capi-telephony tapi msg-service motion)
	FILE(GLOB_RECURSE prvd_srcs ${prvd_srcs} activity/*.cpp)
	FILE(GLOB_RECURSE prvd_srcs ${prvd_srcs} system/*.cpp)
	FILE(GLOB prvd_srcs ${prvd_srcs} social/call.cpp social/message.cpp)
ENDIF("${PROFILE}" STREQUAL "wearable")

# TV Profile
IF("${PROFILE}" STREQUAL "tv")
	SET(prvd_deps ${prvd_deps} capi-network-bluetooth capi-network-wifi)
	FILE(GLOB prvd_srcs ${prvd_srcs} system/headphone.cpp system/wifi.cpp)
ENDIF("${PROFILE}" STREQUAL "tv")
