SET(target "${target_prefix}-my-place")

SET(DEPS ${DEPS}
	gmodule-2.0
	capi-location-manager
	capi-network-wifi
)

FILE(GLOB SRCS *.cpp facade/*.cpp place/*.cpp utils/*.cpp visit-detector/*.cpp)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_MYPLACE REQUIRED ${DEPS})

FOREACH(flag ${PKG_MYPLACE_CFLAGS})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${target} ${PKG_MYPLACE_LDFLAGS} ${target_shared})

INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR}/${target_dir})

ADD_SUBDIRECTORY(visit-categer)