ADD_LIBRARY(coincidence STATIC coincidence.hpp coincidence.cpp)

ADD_EXECUTABLE(test_coincidence test_coincidence.cpp)
TARGET_LINK_LIBRARIES(test_coincidence coincidence)

