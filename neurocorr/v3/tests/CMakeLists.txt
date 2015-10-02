ADD_EXECUTABLE(test-ncv3
main.cpp
test-types.cpp)

TARGET_LINK_YOCTO(test-ncv3)
TARGET_LINK_LIBRARIES(test-ncv3 ncv3)


