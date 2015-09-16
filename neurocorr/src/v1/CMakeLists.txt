
ADD_LIBRARY(neurocorr STATIC
	record.hpp
	record.cpp
	neuron.hpp
	neuron.cpp
	trial.hpp
	trial.cpp
	neurodata.hpp
	neurodata.cpp
	experiment.hpp
	experiment.cpp)

ADD_EXECUTABLE(test-nc
	test-main.cpp
	test-experiment.cpp
	test-records.cpp
)

TARGET_LINK_LIBRARIES(test-nc neurocorr)
TARGET_LINK_YOCTO(test-nc mk)

ENABLE_TESTING()

ADD_TEST(Records test-nc records)


ADD_CUSTOM_TARGET(check-nc COMMAND ${CMAKE_CTEST_COMMAND} -V DEPENDS test-nc )
