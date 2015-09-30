ADD_EXECUTABLE(test-ncv2
main.cpp
test-types.cpp
test-train.cpp
test-records.cpp
test-phifn.cpp
test-cpwfn.cpp
test-intg.cpp
test-box.cpp
)

TARGET_LINK_LIBRARIES(test-ncv2 ncv2)
TARGET_LINK_YOCTO(test-ncv2)

ENABLE_TESTING()
ADD_TEST(Types   test-ncv2 types)
ADD_TEST(Train   test-ncv2 train)
ADD_TEST(Records test-ncv2 records)
ADD_TEST(CPWFn   test-ncv2 cpwfn)
ADD_TEST(Intg    test-ncv2 intg)
ADD_TEST(PhiFn   test-ncv2 phifn)
ADD_TEST(Box     test-ncv2 box)


IF(R_FOUND)
ADD_TEST(NAME test_nc_R WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMMAND ${R_EXE} --no-save --slave -f test-nc.R)
ENDIF()


ADD_CUSTOM_TARGET(check-ncv2 COMMAND ${CMAKE_CTEST_COMMAND} -V DEPENDS test-ncv2 )
