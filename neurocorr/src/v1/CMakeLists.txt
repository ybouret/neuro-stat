ADD_LIBRARY(neurocorr STATIC
	experiment.hpp
	experiment.cpp)

ADD_EXECUTABLE(test-nc
	test-main.cpp
	test-experiment.cpp)


