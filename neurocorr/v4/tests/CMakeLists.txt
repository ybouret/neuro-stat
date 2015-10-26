ADD_EXECUTABLE(test-ncv4
main.cpp
test-types.cpp
test-records.cpp
test-locate.cpp
test-cpw.cpp
test-phi.cpp
test-moments.cpp
test-abs.cpp
test-builders.cpp
test-intg.cpp
test-indx.cpp
)

TARGET_LINK_LIBRARIES(test-ncv4 ncv4)
TARGET_LINK_YOCTO(test-ncv4 mk)

ENABLE_TESTING()
ADD_TEST(Types    test-ncv4 types)
ADD_TEST(Records  test-ncv4 records)
ADD_TEST(CPW      test-ncv4 cpw)
ADD_TEST(PHI      test-ncv4 phi)
ADD_TEST(Locate   test-ncv4 locate)
ADD_TEST(Moments  test-ncv4 moments)
ADD_TEST(ABS      test-ncv4 abs)
ADD_TEST(INTG     test-ncv4 intg)
ADD_TEST(Builders test-ncv4 builders)









