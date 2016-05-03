SET(target "${target_prefix}-call")

SET(DEPS ${DEPS}
	tapi
	capi-telephony
)

FILE(GLOB SRCS *.cpp)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_CALL REQUIRED ${DEPS})

FOREACH(flag ${PKG_CALL_CFLAGS})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${target} ${PKG_CALL_LDFLAGS} ${target_shared})

INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR}/${target_dir})
