ADD_EXECUTABLE(test-ncv3
main.cpp
test-types.cpp
test-records.cpp
)

TARGET_LINK_LIBRARIES(test-ncv3 ncv3)
TARGET_LINK_YOCTO(test-ncv3)


ENABLE_TESTING()
ADD_TEST(Types   test-ncv3 types)
ADD_TEST(Records test-ncv3 records)


