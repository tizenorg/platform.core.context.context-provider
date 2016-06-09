SET(target "${target_prefix}-custom")

#SET(DEPS ${DEPS}
#)

FILE(GLOB SRCS *.cpp)
#FILE(GLOB SRCS CreateProvider.cpp CustomManager.cpp CustomProvider.cpp)

INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PKG_CUSTOM REQUIRED ${DEPS})

FOREACH(flag ${PKG_CUSTOM_CFLAGS})
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
ENDFOREACH(flag)

ADD_LIBRARY(${target} SHARED ${SRCS})
TARGET_LINK_LIBRARIES(${target} ${PKG_CUSTOM_LDFLAGS} ${target_shared})

INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR}/${target_dir})
