#ifndef DUAL_SENSE_REPORT_HPP
#define DUAL_SENSE_REPORT_HPP

#include <cstdint>

namespace dual_sense::detail
{
#pragma pack(push, 1)
	struct ReportCommon
	{
		uint8_t left_pad_x;
		uint8_t left_pad_y;

		uint8_t right_pad_x;
		uint8_t right_pad_y;

		uint8_t left_trigger;
		uint8_t right_trigger;

		uint8_t frame_counter; //frame counter only USB, 1 for BT

		uint8_t dpad;

		uint8_t buttons[2];

		uint8_t gap_0;

		uint8_t timestamp_1[4];

		uint16_t gyro_pitch;
		uint16_t gyro_yaw;
		uint16_t gyro_roll;

		uint16_t acceleration_x;
		uint16_t acceleration_y;
		uint16_t acceleration_z;

		uint8_t gap_1[5];

		uint8_t touch_data_0[4];
		uint8_t touch_data_1[4];

		uint8_t gap_2;

		uint8_t left_trigger_feedback;
		uint8_t right_trigger_feedback;

		uint8_t gap_3[5];

		uint8_t timestamp_2[4];

		uint8_t battery_level;

		uint8_t battery_audio_state;

		uint8_t gap_5[9];
	};

	struct ReportUSB
	{
		uint8_t report_id;

		ReportCommon common;
	};

	struct ReportBT
	{
		uint8_t report_id;

		uint8_t gap1_;

		ReportCommon common;

		uint8_t rest[13];
	};
#pragma pack(pop)

	static_assert(sizeof(ReportUSB) == 64);
	static_assert(sizeof(ReportBT) == 78);
}

#endif //DUAL_SENSE_REPORT_HPP
