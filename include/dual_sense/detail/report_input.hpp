#ifndef DUAL_SENSE_REPORT_INPUT_HPP
#define DUAL_SENSE_REPORT_INPUT_HPP

#include <cstdint>

namespace dual_sense::detail
{
#pragma pack(push, 1)

	struct ReportButtons
	{
		uint8_t dpad: 4;

		uint8_t square: 1;
		uint8_t cross: 1;
		uint8_t circle: 1;
		uint8_t triangle: 1;

		uint8_t l1: 1;
		uint8_t r1: 1;

		uint8_t l2: 1;
		uint8_t r2: 1;

		uint8_t create: 1;
		uint8_t menu: 1;

		uint8_t l3: 1;
		uint8_t r3: 1;

		uint8_t home: 1;
		uint8_t touchpad: 1;
		uint8_t mute: 1;

		uint8_t gap: 5;
	};

	struct ReportCommon
	{
		uint8_t left_pad_x;
		uint8_t left_pad_y;

		uint8_t right_pad_x;
		uint8_t right_pad_y;

		uint8_t left_trigger;
		uint8_t right_trigger;

		uint8_t gap_0; // 1 for BT

		ReportButtons buttons;

		uint8_t gap_1;

		uint8_t timestamp_1[4];

		uint16_t gyro_pitch;
		uint16_t gyro_yaw;
		uint16_t gyro_roll;

		uint16_t acceleration_x;
		uint16_t acceleration_y;
		uint16_t acceleration_z;

		uint8_t gap_2[4];
		uint8_t temperature;

		uint8_t touch_data_0[4];
		uint8_t touch_data_1[4];

		uint8_t touch_timestamp;

		uint8_t right_trigger_feedback: 4;
		uint8_t right_trigger_unknown: 4;
		uint8_t left_trigger_feedback: 4;
		uint8_t left_trigger_unknown: 4;

		uint8_t timestamp_2[4];

		uint8_t gap_3;

		uint8_t timestamp_3[4];

		uint8_t battery_level: 4;
		uint8_t power_status: 4;

		uint8_t headphones: 1;
		uint8_t microphone: 1;
		uint8_t muted: 1;

		uint8_t gap_4: 5;

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

	//Feature reports

	struct PairingInfoReport
	{
		uint8_t report_id;

		uint8_t client_mac[6];
		uint8_t gap[3];
		uint8_t host_mac[6];

		uint8_t crc[4];
	};

	struct CalibrationReport
	{
		uint8_t report_id;

		uint16_t gyro_pitch_bias;
		uint16_t gyro_yaw_bias;
		uint16_t gyro_roll_bias;

		uint16_t gyro_pitch_plus;
		uint16_t gyro_pitch_minus;

		uint16_t gyro_yaw_plus;
		uint16_t gyro_yaw_minus;

		uint16_t gyro_roll_plus;
		uint16_t gyro_roll_minus;

		uint16_t gyro_speed_plus;
		uint16_t gyro_speed_minus;

		uint16_t accel_x_plus;
		uint16_t accel_x_minus;

		uint16_t accel_y_plus;
		uint16_t accel_y_minus;

		uint16_t accel_z_plus;
		uint16_t accel_z_minus;

		uint8_t gap[2];
	};

#pragma pack(pop)

	static_assert(sizeof(ReportUSB) == 64);
	static_assert(sizeof(ReportBT) == 78);

	static_assert(sizeof(PairingInfoReport) == 20);
	static_assert(sizeof(CalibrationReport) == 37);
}

#endif //DUAL_SENSE_REPORT_INPUT_HPP
