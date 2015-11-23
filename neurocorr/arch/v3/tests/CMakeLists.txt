ADD_EXECUTABLE(test-ncv3
main.cpp
test-types.cpp
test-records.cpp
test-cpw.cpp
test-intg.cpp
test-phi.cpp
test-box.cpp
)

TARGET_LINK_LIBRARIES(test-ncv3 ncv3)
TARGET_LINK_YOCTO(test-ncv3)

INSTALL(TARGETS test-ncv3 RUNTIME DESTINATION bin)


ENABLE_TESTING()
ADD_TEST(Types   test-ncv3 types)
ADD_TEST(Records test-ncv3 records)
ADD_TEST(CPW     test-ncv3 cpw)
ADD_TEST(Intg    test-ncv3 intg)
ADD_TEST(PHI     test-ncv3 phi)
ADD_TEST(Box     test-ncv3 box)



