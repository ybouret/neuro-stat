ADD_EXECUTABLE(test-ncv2
main.cpp
test-types.cpp
test-train.cpp
test-neuron.cpp
)

TARGET_LINK_YOCTO(test-ncv2)
TARGET_LINK_LIBRARIES(test-ncv2 ncv2)

ENABLE_TESTING()
ADD_TEST(Types  test-ncv2 types)
ADD_TEST(Train  test-ncv2 train)
ADD_TEST(Neuron test-ncv2 neuron)

