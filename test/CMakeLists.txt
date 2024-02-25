find_package(GTest REQUIRED)

add_executable(dual_sense_hid_test "")
target_link_libraries(
		dual_sense_hid_test
		PRIVATE
		dual_sense_hid

		GTest::gtest GTest::gmock GTest::gtest_main
		options_target
		warnings_target
)

target_sources(
		dual_sense_hid_test
		PRIVATE
		crc32_test.cpp
)

include(GoogleTest)
gtest_discover_tests(dual_sense_hid_test)