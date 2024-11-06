enable_testing()

find_package(GTest CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)

add_executable (unitTests "test.cpp")

target_link_libraries(unitTests PRIVATE fmt::fmt GTest::gtest GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unitTests)