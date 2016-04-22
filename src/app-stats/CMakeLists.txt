SET(target "${target_prefix}-app-stats")

SET(DEPS ${DEPS}
	capi-appfw-package-manager
	capi-appfw-application
	capi-appfw-app-manager
	pkgmgr
	pkgmgr-info
)

FILE(GLOB SRCS *.cpp)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_APP_STATS REQUIRED ${DEPS})

FOREACH(flag ${PKG_APP_STATS_CFLAGS})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${target} ${PKG_APP_LDFLAGS} ${target_shared})

INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR}/context)
