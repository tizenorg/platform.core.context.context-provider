CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(target "ctx-device")
SET(definitions LOG_TAG=\"CONTEXT-DEVICE\")

# Common Profile
FILE(GLOB srcs ./*.cpp)
SET(deps "vconf capi-system-info capi-system-device capi-system-runtime-info")

# Common Profile
SET(srcs ${srcs} system/alarm.cpp system/time.cpp)

# Mobile Profile
IF("${PROFILE}" STREQUAL "mobile")
	SET(deps "${deps} capi-network-bluetooth capi-network-wifi")
	SET(deps "${deps} capi-telephony tapi msg-service capi-messaging-email motion")
	FILE(GLOB_RECURSE srcs ${srcs} activity/*.cpp)
	FILE(GLOB_RECURSE srcs ${srcs} system/*.cpp)
	FILE(GLOB_RECURSE srcs ${srcs} social/*.cpp)
ENDIF("${PROFILE}" STREQUAL "mobile")

# Wearable Profile
IF("${PROFILE}" STREQUAL "wearable")
	SET(deps "${deps} capi-network-bluetooth capi-network-wifi")
	SET(deps "${deps} capi-telephony tapi msg-service motion")
	FILE(GLOB_RECURSE srcs ${srcs} activity/*.cpp)
	FILE(GLOB_RECURSE srcs ${srcs} system/*.cpp)
	SET(srcs ${srcs} social/call.cpp)
	SET(srcs ${srcs} social/message.cpp)
ENDIF("${PROFILE}" STREQUAL "wearable")

# TV Profile
IF("${PROFILE}" STREQUAL "tv")
	SET(deps "${deps} capi-network-bluetooth capi-network-wifi")
	SET(srcs
		${srcs}
		system/headphone.cpp
		system/wifi.cpp
	)
ENDIF("${PROFILE}" STREQUAL "tv")

MESSAGE("Device Provider Sources: ${srcs}")

# Build
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(device_pkgs REQUIRED ${deps_common} ${deps})

FOREACH(flag ${device_pkgs_CFLAGS})
	SET(extra_cflags "${extra_cflags} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${srcs})
TARGET_LINK_LIBRARIES(${target} ${device_pkgs_LDFLAGS})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS ${extra_cflags})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_DEFINITIONS "${definitions}")
SET_TARGET_PROPERTIES(${target} PROPERTIES SOVERSION ${MAJORVER})
SET_TARGET_PROPERTIES(${target} PROPERTIES VERSION ${FULLVER})

# Install
INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RuntimeLibraries)
