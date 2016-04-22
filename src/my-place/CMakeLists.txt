SET(target "${target_prefix}-my-place")

SET(DEPS ${DEPS}
	capi-location-manager
	capi-network-wifi
)

FILE(GLOB_RECURSE SRCS *.cpp)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_MYPLACE REQUIRED ${DEPS})

FOREACH(flag ${PKG_MYPLACE_CFLAGS})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${target} ${DEPS_LDFLAGS} ${target_shared})

INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR}/context)
