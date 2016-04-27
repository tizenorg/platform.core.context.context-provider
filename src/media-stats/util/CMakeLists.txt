SET(target "${target_prefix}-media-stats-util")

SET(DEPS ${DEPS}
	capi-content-media-content
)

FILE(GLOB SRCS *.cpp)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_MEDIA_STATS_UTIL REQUIRED ${DEPS})

FOREACH(flag ${PKG_MEDIA_STATS_UTIL_CFLAGS})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${target} ${PKG_MEDIA_STATS_UTIL_LDFLAGS} ${target_shared})

INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR}/${target_dir})
