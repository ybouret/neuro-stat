ADD_EXECUTABLE(t_coincidence coincidence.cpp)
TARGET_LINK_LIBRARIES(t_coincidence wink)

ADD_EXECUTABLE(t_lookup lookup.cpp)
TARGET_LINK_LIBRARIES(t_lookup wink)

ADD_EXECUTABLE(t_windows windows.cpp)
TARGET_LINK_LIBRARIES(t_windows wink)

ADD_EXECUTABLE(t_permutation permutation.cpp)
TARGET_LINK_LIBRARIES(t_permutation wink)

ADD_EXECUTABLE(t_neuro_trials neuro-trials.cpp)
TARGET_LINK_LIBRARIES(t_neuro_trials wink)

ADD_EXECUTABLE(t_rand32 rand32.cpp)
TARGET_LINK_LIBRARIES(t_rand32 wink)

ENABLE_TESTING()
ADD_TEST(Coincidences t_coincidence)
ADD_TEST(LookUp       t_lookup)
ADD_TEST(Windows      t_windows)
ADD_TEST(Permutations t_permutation) 
ADD_TEST(NeuroTrials  t_neuro_trials)
ADD_TEST(Rand32       t_rand32)

ADD_CUSTOM_TARGET(check1 ALL 
	COMMAND ${CMAKE_CTEST_COMMAND} 
	DEPENDS t_coincidence 
		t_lookup 
		t_windows 
		t_permutation 
		t_neuro_trials
		t_rand32)


