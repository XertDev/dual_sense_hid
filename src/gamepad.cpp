#include "gamepad.hpp"

#include <locale>
#include <cstring>

#include <hidapi.h>

#include "helper.hpp"
#include "report.hpp"


namespace dual_sense
{
	namespace
	{
		inline dual_sense::State::ButtonPad extract_button_pad(uint8_t state)
		{
			uint8_t button_pad_values = state >> 4;

			return
					{
							static_cast<bool>(button_pad_values & 0b1000),
							static_cast<bool>(button_pad_values & 0b0100),
							static_cast<bool>(button_pad_values & 0b0010),
							static_cast<bool>(button_pad_values & 0b0001)
					};
		}

		inline dual_sense::State::Buttons extract_buttons(const uint8_t state[2])
		{
			return
					{
							static_cast<bool>(state[0] & 0x01), static_cast<bool>(state[0] & 0x02),
							static_cast<bool>(state[0] & 0x04), static_cast<bool>(state[0] & 0x08),
							static_cast<bool>(state[0] & 0x10),
							static_cast<bool>(state[0] & 0x20),
							static_cast<bool>(state[0] & 0x40), static_cast<bool>(state[0] & 0x80),
							static_cast<bool>(state[1] & 0x01),
							static_cast<bool>(state[1] & 0x02),
							static_cast<bool>(state[1] & 0x04)
					};
		}

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

		inline dual_sense::State::Battery extract_battery_state(uint8_t charging_state_data, uint8_t charging_level_data)
		{
			return
					{
							static_cast<bool>(charging_state_data & 0x10),
							static_cast<bool>(charging_level_data & 0x20),
							static_cast<uint8_t>(charging_level_data & 0x0f)
					};
		}

		inline dual_sense::State::Audio extract_audio_state(uint8_t audio_state_data)
		{
			return
					{
							static_cast<bool>(audio_state_data & 4),
							static_cast<bool>(audio_state_data & 1),
							static_cast<bool>(audio_state_data & 2)
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
						{common.left_trigger, common.left_trigger_feedback},
						{common.right_trigger, common.right_trigger_feedback},
						static_cast<State::DpadDirection>(common.dpad & 0b1111),
						extract_button_pad(common.dpad),
						extract_buttons(common.buttons),
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
						extract_touch_point(common.touch_data_0),
						extract_touch_point(common.touch_data_1),
						extract_battery_state(common.battery_audio_state, common.battery_level),
						extract_audio_state(common.battery_audio_state)
				};
	}
}