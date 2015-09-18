
ADD_EXECUTABLE(test-nc
	test-main.cpp
	test-experiment.cpp
	test-records.cpp
    	test-neurodata.cpp
	test-step.cpp
)

TARGET_LINK_LIBRARIES(test-nc neurocorr)
TARGET_LINK_YOCTO(test-nc)

ENABLE_TESTING()

ADD_TEST(Records      test-nc records)
ADD_TEST(NeuroData    test-nc neurodata)
ADD_TEST(StepFunction test-nc step)


IF(R_FOUND)
ADD_TEST(NAME test_nc_R WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMAND ${R_EXE} --no-save --slave -f test-nc.R)
ENDIF()


ADD_CUSTOM_TARGET(check-nc COMMAND ${CMAKE_CTEST_COMMAND} -V DEPENDS test-nc )
