enable_testing()

add_executable(derivative-test derivative-test.cpp)
target_link_libraries(derivative-test GTest::gtest_main hugegrad)

include(GoogleTest)
gtest_discover_tests(derivative-test)
