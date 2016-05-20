CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(context-provider-test)
SET(EXEC context-provider-test)

# Source List
FILE(GLOB_RECURSE TC_SRCS src/*.cpp ../src/my-place/*.cpp ../src/shared/WifiWrapper.cpp)
MESSAGE("TC Sources: ${TC_SRCS}")

INCLUDE_DIRECTORIES(
	/usr/include
	/usr/include/glib-2.0
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/src
)

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	dlog
	glib-2.0
	capi-base-common
	libcontext-server
	libcontext-shared
	capi-network-wifi
	capi-location-manager
)

SET(EXTRA_CFLAGS -fPIE)
FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

# for Emulator
IF("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DTARGET")
ELSE("${ARCH}" STREQUAL "arm")
	ADD_DEFINITIONS("-DEMULATOR")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_EXECUTABLE(${EXEC} ${TC_SRCS})
TARGET_LINK_LIBRARIES(${EXEC} ${pkgs_LDFLAGS} ${target} -pie)
SET_TARGET_PROPERTIES(${EXEC} PROPERTIES COMPILE_FLAGS ${EXTRA_CFLAGS})
SET_TARGET_PROPERTIES(${EXEC} PROPERTIES COMPILE_DEFINITIONS "LOG_TAG=\"CONTEXT-TEST\"")

INSTALL(TARGETS ${EXEC} DESTINATION ${CMAKE_INSTALL_BINDIR})