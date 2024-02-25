#include <gtest/gtest.h>

#include <dual_sense_hid/detail/crc32.hpp>

TEST(crc32, validation)
{
	{
		const std::array<uint8_t, 1> data = {1};
		EXPECT_EQ(2867854338, dual_sense_hid::detail::crc32(data.data(), data.size()));
	}

	{
		const std::array<uint8_t, 1> data = {2};
		EXPECT_EQ(870755768, dual_sense_hid::detail::crc32(data.data(), data.size()));
	}

	{
		const std::array<uint8_t, 1> data = {231};
		EXPECT_EQ(3817257807, dual_sense_hid::detail::crc32(data.data(), data.size()));
	}

	{
		const std::array<uint8_t, 16> data = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 1};
		EXPECT_EQ(112948030, dual_sense_hid::detail::crc32(data.data(), data.size()));
	}

	{
		const std::array<uint8_t, 16> data = {15, 25, 35, 45, 55, 65, 75, 85, 95, 105, 115, 125, 135, 14, 15, 1};
		EXPECT_EQ(3454721196, dual_sense_hid::detail::crc32(data.data(), data.size()));
	}

	{
		const std::array<uint8_t, 32> data = {
				15, 25, 35, 45, 55, 65, 75, 85, 95, 105, 115, 125, 135, 14, 15, 1,
				15, 25, 35, 45, 55, 65, 75, 85, 95, 105, 115, 125, 135, 14,15, 1
		};
		EXPECT_EQ(1190435119, dual_sense_hid::detail::crc32(data.data(), data.size()));
	}

	{
		const std::array<uint8_t, 45> data = {
				15, 25, 35, 45, 55, 65, 75, 85, 95, 105, 115, 125, 135, 14, 15, 1,
				15, 25, 35, 45, 55, 65, 75, 85, 95, 105, 115, 125, 135, 14, 15, 1,
				15, 25, 35, 45, 55, 65, 75, 85, 95, 105, 115, 125, 135
		};
		EXPECT_EQ(621908005, dual_sense_hid::detail::crc32(data.data(), data.size()));
	}
}
