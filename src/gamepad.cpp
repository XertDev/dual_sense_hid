#include "dual_sense_hid/gamepad.hpp"

#include <locale>

#include <hidapi.h>

#include "dual_sense_hid/detail/helper.hpp"
#include "dual_sense_hid/detail/report_input.hpp"
#include "dual_sense_hid/detail/report_output.hpp"

static constexpr uint8_t CALIBRATION_REPORT_ID = 0x05;


namespace dual_sense_hid
{
	namespace
	{
		inline State::TouchPoint extract_touch_point(const uint8_t touch_data[4])
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

		template<typename T>
		requires std::integral<T>
		inline T mult_frac(T value, T numerator, T denominator)
		{
			return (value / denominator) * numerator + ((value % denominator) * numerator) / denominator;
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

	void Gamepad::Lights::set_player_indicator(Gamepad::Lights::PlayerIndicator indicator)
	{
		changed_ |= indicator != player_indicator_;

		player_indicator_ = indicator;
	}

	void Gamepad::Lights::set_touchpad_light_color(uint8_t red, uint8_t green, uint8_t blue)
	{
		changed_ |= (red != touchpad_light_red_) || (green != touchpad_light_green_) || (blue != touchpad_light_blue_);

		touchpad_light_red_ = red;
		touchpad_light_green_ = green;
		touchpad_light_blue_ = blue;
	}

	void Gamepad::Lights::set_player_indicator_brightness(Gamepad::Lights::PlayerIndicatorBrightness brightness)
	{
		changed_ |= player_indicator_brightness_ != brightness;

		player_indicator_brightness_ = brightness;
	}

	void Gamepad::Lights::set_mute_light_mode(MuteLightMode mute_light_mode)
	{
		changed_ |= mute_light_mode_ == mute_light_mode;

		mute_light_mode_ = mute_light_mode;
	}

	Gamepad::Gamepad(const DeviceInfo &device_info, bool fetch_calibration_data, bool auto_push_settings)
		:connection_type_(device_info.connection_type), auto_push_settings_(auto_push_settings)
	{
		const auto &path = device_info.path;
		device_ = hid_open_path(path.c_str());
		if (device_ == nullptr)
		{
			throw std::runtime_error("Failed to open device path");
		}

		if(fetch_calibration_data)
		{
			get_calibration_data();
		}

		take_lights_control();
	}

	State Gamepad::poll(bool use_calibration_data) const
	{
		using namespace detail;

		uint8_t report[78] = {};
		const size_t to_read =
				connection_type_ == ConnectionType::USB ? sizeof(detail::ReportUSB) : sizeof(detail::ReportBT);
		hid_read(device_, reinterpret_cast<unsigned char *>(&report), to_read);

		const auto &common =
				connection_type_ == ConnectionType::USB ? reinterpret_cast<detail::ReportUSB *>(report)->common
				                                        : reinterpret_cast<detail::ReportBT *>(report)->common;

		auto gyro_pitch = static_cast<int32_t>(std::bit_cast<int16_t>(le_to_native(common.gyro_pitch)));
		auto gyro_yaw = static_cast<int32_t>(std::bit_cast<int16_t>(le_to_native(common.gyro_yaw)));
		auto gyro_roll = static_cast<int32_t>(std::bit_cast<int16_t>(le_to_native(common.gyro_roll)));

		auto accel_x = static_cast<int32_t>(std::bit_cast<int16_t>(le_to_native(common.acceleration_x)));
		auto accel_y = static_cast<int32_t>(std::bit_cast<int16_t>(le_to_native(common.acceleration_y)));
		auto accel_z = static_cast<int32_t>(std::bit_cast<int16_t>(le_to_native(common.acceleration_z)));

		if(use_calibration_data)
		{
			if(!calibration_data_loaded_)
			{
				get_calibration_data();
			}

			const auto& gyro_calib = calibration_data_.gyroscope;

			gyro_pitch = mult_frac(gyro_pitch - gyro_calib.pitch_offset, gyro_calib.factor_numerator, gyro_calib.pitch_factor_denominator);
			gyro_yaw = mult_frac(gyro_yaw - gyro_calib.yaw_offset, gyro_calib.factor_numerator, gyro_calib.yaw_factor_denominator);
			gyro_roll = mult_frac(gyro_roll - gyro_calib.roll_offset, gyro_calib.factor_numerator, gyro_calib.roll_factor_denominator);

			const auto& accel_calib = calibration_data_.accelerometer;

			accel_x = mult_frac(accel_x - accel_calib.x_offset, accel_calib.factor_numerator, accel_calib.x_factor_denominator);
			accel_y = mult_frac(accel_y - accel_calib.y_offset, accel_calib.factor_numerator, accel_calib.y_factor_denominator);
			accel_z = mult_frac(accel_z - accel_calib.z_offset, accel_calib.factor_numerator, accel_calib.z_factor_denominator);
		}

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
					static_cast<State::DPadDirection>(common.buttons.dpad),
					{
						static_cast<bool>(common.buttons.triangle),
						static_cast<bool>(common.buttons.circle),
						static_cast<bool>(common.buttons.cross),
						static_cast<bool>(common.buttons.square)
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
							gyro_pitch,
							gyro_yaw,
							gyro_roll
					},
					{
							accel_x,
							accel_y,
							accel_z
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
	
	void Gamepad::push_state(bool full_update)
	{
		detail::SetStateReportCommon common_report{};

		if(full_update || lights_.changed_)
		{
			common_report.enable_led_color_section = true;
			common_report.enable_player_indicators_section = true;
			common_report.enable_light_brightness_section = true;
			common_report.enable_color_light_fade_section = true;
			common_report.enable_mute_light_section = true;

			common_report.power_save_mute.mute_light_mode = static_cast<uint8_t>(lights_.mute_light_mode_);

			common_report.player_led.brightness = static_cast<uint8_t>(lights_.player_indicator_brightness_);

			common_report.player_led.led_1 = false;
			common_report.player_led.led_2 = false;
			common_report.player_led.led_3 = false;
			common_report.player_led.led_4 = false;
			common_report.player_led.led_5 = false;

			if (
					lights_.player_indicator_ == Lights::PlayerIndicator::PLAYER_ONE
					|| lights_.player_indicator_ == Lights::PlayerIndicator::PLAYER_THREE
					|| lights_.player_indicator_ == Lights::PlayerIndicator::PLAYER_FIVE
				)
			{
				common_report.player_led.led_3 = true;
			}

			if (
					lights_.player_indicator_ == Lights::PlayerIndicator::PLAYER_TWO
					|| lights_.player_indicator_ == Lights::PlayerIndicator::PLAYER_FOUR
					|| lights_.player_indicator_ == Lights::PlayerIndicator::PLAYER_FIVE
			)
			{
				common_report.player_led.led_2 = true;
				common_report.player_led.led_4 = true;
			}

			if (
					lights_.player_indicator_ == Lights::PlayerIndicator::PLAYER_THREE
					|| lights_.player_indicator_ == Lights::PlayerIndicator::PLAYER_FOUR
					|| lights_.player_indicator_ == Lights::PlayerIndicator::PLAYER_FIVE
			)
			{
				common_report.player_led.led_1 = true;
				common_report.player_led.led_5 = true;
			}

			common_report.touchpad_led_color.red_led = lights_.touchpad_light_red_;
			common_report.touchpad_led_color.green_led = lights_.touchpad_light_green_;
			common_report.touchpad_led_color.blue_led = lights_.touchpad_light_blue_;
		}


		if (connection_type_ == ConnectionType::USB)
		{
			detail::SetStateReportUSB report{};
			report.report_id = 0x02;
			report.common = common_report;

			hid_write(device_, reinterpret_cast<uint8_t*>(&report), sizeof(detail::SetStateReportUSB));
		}
	}

	Gamepad::Lights& Gamepad::lights()
	{
		return lights_;
	}

	void Gamepad::take_lights_control()
	{
		detail::SetStateReportCommon common_report{};

		if(!lights_reset_)
		{
			common_report.reset_lights = true;
			lights_reset_ = true;
		}

		if (connection_type_ == ConnectionType::USB)
		{
			detail::SetStateReportUSB report{};
			report.report_id = 0x02;
			report.common = common_report;

			hid_write(device_, reinterpret_cast<uint8_t*>(&report), sizeof(detail::SetStateReportUSB));
		}
	}


}