enable_testing()

add_executable(
  gtester
  gtest/serialize_test.cpp
  gtest/custom_types.cpp
)
target_link_libraries(
  gtester
  GTest::gtest_main
)

target_include_directories(
  gtester
  PRIVATE
  serialize/)

include(GoogleTest)
gtest_discover_tests(gtester)
