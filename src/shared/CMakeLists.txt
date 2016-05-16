SET(DEPS ${DEPS}
	capi-system-info
	capi-system-runtime-info
	capi-media-sound-manager
	capi-network-wifi
)

FILE(GLOB SRCS *.cpp)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_SHARED REQUIRED ${DEPS})

FOREACH(flag ${PKG_SHARED_CFLAGS})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target_shared} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${target_shared} ${PKG_SHARED_LDFLAGS})

INSTALL(TARGETS ${target_shared} DESTINATION ${CMAKE_INSTALL_LIBDIR})
