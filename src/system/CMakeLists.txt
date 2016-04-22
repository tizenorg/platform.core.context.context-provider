SET(target "${target_prefix}-system")

SET(DEPS ${DEPS}
	vconf
	capi-system-info
	capi-system-device
	capi-system-runtime-info
)

FILE(GLOB SRCS *.cpp)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_SYSTEM REQUIRED ${DEPS})

FOREACH(flag ${PKG_SYSTEM_CFLAGS})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${target} ${DEPS_LDFLAGS} ${target_shared})

INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR}/context)
