ADD_EXECUTABLE(test-ncv4
main.cpp
test-types.cpp
)

TARGET_LINK_LIBRARIES(test-ncv4 ncv4)
TARGET_LINK_YOCTO(test-ncv4 mk)

ENABLE_TESTING()
ADD_TEST(Types test-ncv4 types)


