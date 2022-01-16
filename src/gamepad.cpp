#include "dual_sense/gamepad.hpp"

#include <locale>
#include <cstring>

#include <hidapi.h>

#include "dual_sense/detail/helper.hpp"
#include "dual_sense/detail/report.hpp"


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
							static_cast<int16_t>(le_to_native(common.gyro_pitch)),
							static_cast<int16_t>(le_to_native(common.gyro_yaw)),
							static_cast<int16_t>(le_to_native(common.gyro_roll))
					},
					{
							static_cast<int16_t>(le_to_native(common.acceleration_x)),
							static_cast<int16_t>(le_to_native(common.acceleration_y)),
							static_cast<int16_t>(le_to_native(common.acceleration_z))
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
}