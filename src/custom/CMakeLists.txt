CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(target "ctx-custom")

# Common Profile
FILE(GLOB srcs ./*.cpp)

MESSAGE("Custom Provider Sources: ${srcs}")

# Build
INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(custom_pkgs REQUIRED ${deps_common})

FOREACH(flag ${custom_pkgs_CFLAGS})
	SET(extra_cflags "${extra_cflags} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${srcs})
TARGET_LINK_LIBRARIES(${target} ${custom_pkgs_LDFLAGS})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_FLAGS ${extra_cflags})
SET_TARGET_PROPERTIES(${target} PROPERTIES COMPILE_DEFINITIONS "LOG_TAG=\"CONTEXT-CUSTOM\"")
SET_TARGET_PROPERTIES(${target} PROPERTIES SOVERSION ${MAJORVER})
SET_TARGET_PROPERTIES(${target} PROPERTIES VERSION ${FULLVER})

# Install
INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT RuntimeLibraries)
