SET(c_srcs
types.hpp
types.cpp
)

ADD_LIBRARY(ncv2 STATIC ${c_srcs})

ADD_SUBDIRECTORY(tests)

