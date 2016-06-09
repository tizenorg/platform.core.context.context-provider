SET(target "${target_prefix}-my-place-visit-categer")

FILE(GLOB SRCS *.cpp)

ADD_LIBRARY(${target} SHARED ${SRCS})

INSTALL(TARGETS ${target} DESTINATION ${CMAKE_INSTALL_LIBDIR}/${target_dir})