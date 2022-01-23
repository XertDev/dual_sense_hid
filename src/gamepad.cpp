#include "dual_sense/gamepad.hpp"

#include <locale>
#include <cstring>

#include <hidapi.h>

#include "dual_sense/detail/helper.hpp"
#include "dual_sense/detail/report.hpp"

static constexpr uint8_t CALIBRATION_REPORT_ID = 0x05;


namespace dual_sense
{
	namespace
	{
		inline dual_sense::State::TouchPoint extract_touch_point(const uint8_t touch_data[4])
		{
			const auto x = static_cast<uint16_t>(((touch_data[2] & 0x0f) << 8) | touch_data[1]);
			const auto y = static_cast<uint16_t>((touch_data[3] << 4) | (touch_data[2] & 0xf0 >> 4));

			return
					{
							(touch_data[0] & 0x80) == 0,
							x, y,
							static_cast<uint8_t>(touch_data[0] & 0x7f)
					};
		}
	}

	std::vector<DeviceInfo> enumerate()
	{
		using namespace detail;

		const auto enumerated_list = hid_enumerate(VENDOR_ID, PRODUCT_ID);

		std::vector<DeviceInfo> devices;

		auto current = enumerated_list;
		while (current)
		{
			const std::string path = current->path;
			const auto serial = wstring_to_string(current->serial_number);
			const auto manufacturer_string = wstring_to_string(current->manufacturer_string);
			const auto product_string = wstring_to_string(current->product_string);

			const auto connection_type = current->release_number == 0 ? ConnectionType::BLUETOOTH : ConnectionType::USB;

			devices.emplace_back(path, serial, manufacturer_string, product_string, connection_type);

			current = current->next;
		}

		return devices;
	}

	Gamepad::Gamepad(const DeviceInfo &device_info) : connection_type_(device_info.connection_type)
	{
		const auto &path = device_info.path;
		device_ = hid_open_path(path.c_str());
		if (device_ == nullptr)
		{
			throw std::runtime_error("Failed to open device path");
		}
	}

	State Gamepad::poll() const
	{
		using namespace detail;

		uint8_t report[78] = {};
		const size_t to_read =
				connection_type_ == ConnectionType::USB ? sizeof(detail::ReportUSB) : sizeof(detail::ReportBT);
		hid_read(device_, reinterpret_cast<unsigned char *>(&report), to_read);

		const auto &common =
				connection_type_ == ConnectionType::USB ? reinterpret_cast<detail::ReportUSB *>(report)->common
				                                        : reinterpret_cast<detail::ReportBT *>(report)->common;

		return
			{
					{common.left_pad_x, common.left_pad_y},
					{common.right_pad_x, common.right_pad_y},
					{
						common.left_trigger,
						common.left_trigger_feedback,
					},
					{
						common.right_trigger,
						common.right_trigger_feedback,
					},
					static_cast<State::DPadDirection>(common.dpad),
					{
						static_cast<bool>(common.triangle),
						static_cast<bool>(common.circle),
						static_cast<bool>(common.cross),
						static_cast<bool>(common.square)
					},
					{
							static_cast<bool>(common.buttons.l1),
							static_cast<bool>(common.buttons.r1),
							static_cast<bool>(common.buttons.l2),
							static_cast<bool>(common.buttons.r2),
							static_cast<bool>(common.buttons.create),
							static_cast<bool>(common.buttons.menu),
							static_cast<bool>(common.buttons.l3),
							static_cast<bool>(common.buttons.r3),
							static_cast<bool>(common.buttons.home),
							static_cast<bool>(common.buttons.touchpad),
							static_cast<bool>(common.buttons.mute)
					},
					{
							std::bit_cast<int16_t>(le_to_native(common.gyro_pitch)),
							std::bit_cast<int16_t>(le_to_native(common.gyro_yaw)),
							std::bit_cast<int16_t>(le_to_native(common.gyro_roll))
					},
					{
							std::bit_cast<int16_t>(le_to_native(common.acceleration_x)),
							std::bit_cast<int16_t>(le_to_native(common.acceleration_y)),
							std::bit_cast<int16_t>(le_to_native(common.acceleration_z))
					},
					common.temperature,
					extract_touch_point(common.touch_data_0),
					extract_touch_point(common.touch_data_1),
					{
						common.battery_level,
						static_cast<State::PowerStatus>(common.power_status)
					},
					{
						static_cast<bool>(common.muted),
						static_cast<bool>(common.headphones),
						static_cast<bool>(common.microphone)
					}
			};
	}

	const Calibration& Gamepad::get_calibration_data() const
	{
		using namespace detail;
		if(!calibration_data_loaded_)
		{
			uint8_t report_raw[37];
			report_raw[0] = CALIBRATION_REPORT_ID;

			hid_get_feature_report(device_, report_raw, sizeof(report_raw));

			const auto data_raw = reinterpret_cast<detail::CalibrationReport*>(report_raw);

			auto& gyro_calibration = calibration_data_.gyroscope;
			auto& accel_calibration = calibration_data_.accelerometer;

			gyro_calibration.pitch_offset = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_pitch_bias));
			gyro_calibration.yaw_offset = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_yaw_bias));
			gyro_calibration.roll_offset = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_roll_bias));

			{
				const auto gyro_speed_plus = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_speed_plus));
				const auto gyro_speed_minus = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_speed_minus));

				const auto gyro_speed = gyro_speed_plus + gyro_speed_minus;

				gyro_calibration.factor_numerator = gyro_speed * Calibration::GYROSCOPE_RESOLUTION;
			}
			{
				const auto gyro_pitch_plus = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_pitch_plus));
				const auto gyro_pitch_minus = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_pitch_minus));

				gyro_calibration.pitch_factor_denominator = gyro_pitch_plus - gyro_pitch_minus;
			}
			{
				const auto gyro_yaw_plus = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_yaw_plus));
				const auto gyro_yaw_minus = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_yaw_minus));

				gyro_calibration.yaw_factor_denominator = gyro_yaw_plus - gyro_yaw_minus;
			}
			{
				const auto gyro_roll_plus = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_roll_plus));
				const auto gyro_roll_minus = std::bit_cast<int16_t>(le_to_native(data_raw->gyro_roll_minus));

				gyro_calibration.roll_factor_denominator = gyro_roll_plus - gyro_roll_minus;
			}

			accel_calibration.factor_numerator = 2 * Calibration::ACCELEROMETER_RESOLUTION;
			{
				const auto accel_x_plus = std::bit_cast<int16_t>(le_to_native(data_raw->accel_x_plus));
				const auto accel_x_minus = std::bit_cast<int16_t>(le_to_native(data_raw->accel_x_minus));

				const auto accel_range = accel_x_plus - accel_x_minus;

				accel_calibration.x_offset = accel_x_plus - accel_range/2;
				accel_calibration.x_factor_denominator = accel_range;
			}
			{
				const auto accel_y_plus = std::bit_cast<int16_t>(le_to_native(data_raw->accel_y_plus));
				const auto accel_y_minus = std::bit_cast<int16_t>(le_to_native(data_raw->accel_y_minus));

				const auto accel_range = accel_y_plus - accel_y_minus;

				accel_calibration.y_offset = accel_y_plus - accel_range/2;
				accel_calibration.y_factor_denominator = accel_range;
			}
			{
				const auto accel_z_plus = std::bit_cast<int16_t>(le_to_native(data_raw->accel_z_plus));
				const auto accel_z_minus = std::bit_cast<int16_t>(le_to_native(data_raw->accel_z_minus));

				const auto accel_range = accel_z_plus - accel_z_minus;

				accel_calibration.z_offset = accel_z_plus - accel_range/2;
				accel_calibration.z_factor_denominator = accel_range;
			}
		}

		return calibration_data_;
	}
}