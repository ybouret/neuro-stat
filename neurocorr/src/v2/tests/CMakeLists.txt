ADD_EXECUTABLE(test-ncv2
main.cpp
test-types.cpp)

TARGET_LINK_YOCTO(test-ncv2)
TARGET_LINK_LIBRARIES(test-ncv2 ncv2)

ENABLE_TESTING()
ADD_TEST(Types test-ncv2 types)

