ADD_EXECUTABLE(coincidence coincidence.cpp)
TARGET_LINK_LIBRARIES(coincidence wink)

ADD_EXECUTABLE(lookup lookup.cpp)
TARGET_LINK_LIBRARIES(lookup wink)

ADD_EXECUTABLE(windows windows.cpp)
TARGET_LINK_LIBRARIES(windows wink)

ENABLE_TESTING()
ADD_TEST(Coincidences coincidence)
ADD_TEST(LookUp       lookup)
ADD_TEST(Windows      windows)

ADD_CUSTOM_TARGET(check1 ALL 
	COMMAND ${CMAKE_CTEST_COMMAND} 
	DEPENDS coincidence lookup windows)


