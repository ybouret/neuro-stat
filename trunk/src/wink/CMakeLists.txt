ADD_LIBRARY(wink STATIC wink.hpp wink.cpp)

ADD_EXECUTABLE(test_coincidence test_coincidence.cpp)
TARGET_LINK_LIBRARIES(test_coincidence wink)

ADD_EXECUTABLE(test_lookup test_lookup.cpp)
TARGET_LINK_LIBRARIES(test_lookup wink)

ADD_EXECUTABLE(test_windows test_windows.cpp)
TARGET_LINK_LIBRARIES(test_windows wink)

ENABLE_TESTING()
ADD_TEST(Coincidences test_coincidence)
ADD_TEST(LookUp       test_lookup)
ADD_TEST(Windows      test_windows)

ADD_CUSTOM_TARGET(check1 ALL COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS test_coincidence test_lookup test_windows)


