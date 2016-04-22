SET(target "${target_prefix}-headphone")

SET(DEPS ${DEPS}
	capi-system-runtime-info
	capi-network-bluetooth
)

FILE(GLOB SRCS *.cpp)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_HEADPHONE REQUIRED ${DEPS})

FOREACH(flag ${PKG_HEADPHONE_CFLAGS})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${target} ${DEPS_LDFLAGS} ${target_shared})

INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR}/context)
