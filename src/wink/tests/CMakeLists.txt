ADD_EXECUTABLE(t_coincidence coincidence.cpp)
TARGET_LINK_LIBRARIES(t_coincidence wink)

ADD_EXECUTABLE(t_lookup lookup.cpp)
TARGET_LINK_LIBRARIES(t_lookup wink)

ADD_EXECUTABLE(t_windows windows.cpp)
TARGET_LINK_LIBRARIES(t_windows wink)

ADD_EXECUTABLE(t_permutation permutation.cpp)
TARGET_LINK_LIBRARIES(t_permutation wink)


ENABLE_TESTING()
ADD_TEST(Coincidences t_coincidence)
ADD_TEST(LookUp       t_lookup)
ADD_TEST(Windows      t_windows)
ADD_TEST(Permutations t_permutation) 

ADD_CUSTOM_TARGET(check1 ALL 
	COMMAND ${CMAKE_CTEST_COMMAND} 
	DEPENDS t_coincidence t_lookup t_windows t_permutation)


