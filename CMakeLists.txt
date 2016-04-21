CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(context-provider)
INCLUDE(GNUInstallDirs)
SET(target "context-provider")
SET(compile_defs "LOG_TAG=\"CONTEXT\"")

# Common Options
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src/shared
)
ADD_DEFINITIONS(-O2 -Wall -fPIC -fdata-sections -ffunction-sections -fvisibility=hidden)
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIC -Wl,--as-needed -Wl,--gc-section -Wl,--print-gc-section")

# Profiles
IF("${PROFILE}" STREQUAL "mobile")
	ADD_DEFINITIONS("-D_MOBILE_")
ENDIF("${PROFILE}" STREQUAL "mobile")

IF("${PROFILE}" STREQUAL "wearable")
	ADD_DEFINITIONS("-D_WEARABLE_")
ENDIF("${PROFILE}" STREQUAL "wearable")


# Dependency & Source
SET(DEPS
	libcontext-server
	vconf
	capi-system-info
	capi-system-device
	capi-system-runtime-info
	capi-appfw-package-manager
	capi-appfw-application
	capi-appfw-app-manager
	pkgmgr
	pkgmgr-info
	capi-media-sound-manager
	capi-network-bluetooth
	capi-network-wifi
	motion
)

FILE(GLOB SRCS
	src/shared/*.cpp
	src/time/*.cpp
	src/activity/*.cpp
	src/app-stats/*.cpp
	src/headphone/*.cpp
	src/system/*.cpp
	src/wifi/*.cpp
)

IF("${PROFILE}" STREQUAL "mobile")
SET(DEPS ${DEPS}
	msg-service
	contacts-service2
	tapi
	capi-telephony
	capi-messaging-email
	capi-content-media-content
	capi-location-manager
	capi-geofence-manager
)

FILE(GLOB SRCS ${SRCS}
	src/call/*.cpp
	src/contacts/*.cpp
	src/email/*.cpp
	src/geofence/*.cpp
	src/media-stats/*.cpp
	src/message/*.cpp
	src/my-place/*.cpp
	src/my-place/user_places/*.cpp
	src/social-stats/*.cpp
)
ENDIF("${PROFILE}" STREQUAL "mobile")


# Build
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(DEPS REQUIRED ${DEPS})

FOREACH(flag ${DEPS_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} STATIC ${SRCS})
TARGET_LINK_LIBRARIES(${target} ${DEPS_LDFLAGS})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS ${EXTRA_CFLAGS})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_DEFINITIONS "${compile_defs}")
SET_TARGET_PROPERTIES(${target} PROPERTIES SOVERSION ${MAJORVER})
SET_TARGET_PROPERTIES(${target} PROPERTIES VERSION ${FULLVER})

# Package Config
FOREACH(item IN LISTS DEPS)
	SET(deps_str "${deps_str} ${item}")
ENDFOREACH(item)

SET(VERSION ${FULLVER})
SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(PC_NAME ${PROJECT_NAME})
SET(PC_DESCRIPTION "Tizen Context Framework Context Provider")
SET(PC_INCLUDE "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR}/context-service")
SET(PC_LIBDIR "${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}")
SET(PC_CFLAGS "-I\${includedir}/context-service")
SET(PC_LDFLAGS "-l${target}")
SET(PC_REQUIRED "${deps_str}")

CONFIGURE_FILE(
	${PROJECT_NAME}.pc.in
	${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc
	@ONLY
)

# Installing
INSTALL(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/context-service/internal
	FILES_MATCHING PATTERN "*.h"
)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR})
