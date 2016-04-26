CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(context-provider)
INCLUDE(GNUInstallDirs)
SET(target_prefix "ctx-prvd")

# Common Options
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src/shared
)
ADD_DEFINITIONS(-O2 -Wall -fPIC -fdata-sections -ffunction-sections)
ADD_DEFINITIONS(-fvisibility=hidden)
ADD_DEFINITIONS(-DLOG_TAG="CONTEXT")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fPIC -Wl,--as-needed -Wl,--gc-section -Wl,--print-gc-section")

# Profiles
IF("${PROFILE}" STREQUAL "mobile")
	ADD_DEFINITIONS("-D_MOBILE_")
ENDIF("${PROFILE}" STREQUAL "mobile")

IF("${PROFILE}" STREQUAL "wearable")
	ADD_DEFINITIONS("-D_WEARABLE_")
ENDIF("${PROFILE}" STREQUAL "wearable")

INSTALL(
	DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
	DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/context-service/internal
	FILES_MATCHING PATTERN "*.h"
)

ADD_SUBDIRECTORY(src)
