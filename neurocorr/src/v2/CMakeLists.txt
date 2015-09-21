SET(c_srcs
types.hpp
types.cpp
train.hpp
train.cpp
neuron.cpp
neuron.hpp
)

ADD_LIBRARY(ncv2 STATIC ${c_srcs})

ADD_SUBDIRECTORY(tests)

