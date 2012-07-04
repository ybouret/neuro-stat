ADD_LIBRARY(coincidence STATIC coincidence.hpp coincidence.cpp)

ADD_EXECUTABLE(test_coincidence test_coincidence.cpp)
TARGET_LINK_LIBRARIES(test_coincidence coincidence)

ADD_EXECUTABLE(test_windows test_windows.cpp)
TARGET_LINK_LIBRARIES(test_windows coincidence)

ENABLE_TESTING()
ADD_TEST(Coincidences test_coincidence)
ADD_TEST(Windows      test_windows)

