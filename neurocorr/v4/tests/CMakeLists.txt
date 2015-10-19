ADD_EXECUTABLE(test-ncv4
main.cpp)

TARGET_LINK_LIBRARIES(test-ncv4 ncv4)
TARGET_LINK_YOCTO(test-ncv4 mk)

