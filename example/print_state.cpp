#include <string>
#include <chrono>
#include <thread>

#include <tabulate/table.hpp>
#include <fmt/format.h>

#include "dual_sense/gamepad.hpp"


using namespace tabulate;
using namespace dual_sense;
using Row_t = Table::Row_t;

namespace
{
	inline std::string to_string(ConnectionType connection_type)
	{
		return connection_type == ConnectionType::USB ? "USB" : "BT";
	}

	inline std::string to_string(State::DPadDirection direction)
	{
		switch(direction)
		{
			case State::DPadDirection::UP:
				return "UP";
			case State::DPadDirection::UP_RIGHT:
				return "UP_RIGHT";
			case State::DPadDirection::RIGHT:
				return "RIGHT";
			case State::DPadDirection::DOWN_RIGHT:
				return "DOWN_RIGHT";
			case State::DPadDirection::DOWN:
				return "DOWN";
			case State::DPadDirection::DOWN_LEFT:
				return "DOWN_LEFT";
			case State::DPadDirection::LEFT:
				return "LEFT";
			case State::DPadDirection::UP_LEFT:
				return "UP_LEFT";
			case State::DPadDirection::NONE:
				return "NONE";
			default:
				return "UNKNOWN";
		}
	}
}

void print_enumeration(const std::vector<DeviceInfo>& devices)
{
	Table enumerated_devices;
	enumerated_devices.add_row({
			                           "Enumerated devices:",
			                           "Serial",
			                           "Manufacturer",
			                           "Product",
			                           "Connection type"
	                           });

	size_t counter = 1u;
	for(const auto& device: devices )
	{
		enumerated_devices.add_row({
				                           std::to_string(counter),
				                           device.serial,
				                           device.manufacturer_string,
				                           device.product_string,
				                           to_string(device.connection_type)
		                           });
	}

	for(auto& cell: enumerated_devices[0])
	{
		cell.format()
				.font_color(Color::yellow)
				.font_align(FontAlign::center)
				.font_style({FontStyle::bold});
	}

	std::cout << enumerated_devices << std::endl;
}

Table analog_to_table(const State::AnalogPad& left, const State::AnalogPad& right)
{
	Table pads_table;
	pads_table.add_row(Row_t{"Analog pads"});
	pads_table[0].format().font_color(Color::yellow).font_align(FontAlign::center);

	const auto left_x = std::to_string(left.x);
	const auto left_y = std::to_string(left.y);

	const auto right_x = std::to_string(right.x);
	const auto right_y = std::to_string(right.y);

	Table pads_value_table;
	pads_value_table.add_row(Row_t{"Left x", left_x, "Right x", right_x});
	pads_value_table.add_row(Row_t{"Left y", left_y, "Right y", right_y});

	pads_value_table.column(1).format().border_left(":");
	pads_value_table.column(3).format().border_left(":");

	pads_table.add_row(Row_t{pads_value_table});

	return pads_table;
}

Table trigger_to_table(const State::Trigger& left, const State::Trigger& right)
{
	Table trigger_table;
	trigger_table.add_row(Row_t{"Triggers"});
	trigger_table[0].format().font_color(Color::yellow).font_align(FontAlign::center);

	const auto left_value = std::to_string(left.value);
	const auto right_value = std::to_string(right.value);

	const auto left_stop = std::to_string(left.stop_location);
	const auto right_stop = std::to_string(right.stop_location);

	Table trigger_value_table;
	trigger_value_table.add_row(Row_t{"Left", left_value, "Right", right_value});
	trigger_value_table.add_row(Row_t{"Left stop location", left_stop, "Right stop location", right_stop});

	trigger_value_table.column(1).format().border_left(":");
	trigger_value_table.column(3).format().border_left(":");

	trigger_table.add_row(Row_t{trigger_value_table});

	return trigger_table;
}

Table dpad_button_pad_to_table(const State::DPadDirection& dpad_direction, const State::ButtonPad& button_pad)
{
	Table buttons_table;
	buttons_table.add_row(Row_t{"DPad & Button pad"});
	buttons_table[0].format().font_color(Color::yellow).font_align(FontAlign::center);

	Table button_state_table;

	button_state_table.add_row({"dpad", to_string(dpad_direction)});

	button_state_table.add_row({"triangle", std::to_string(button_pad.triangle)});
	button_state_table.add_row({"circle", std::to_string(button_pad.circle)});
	button_state_table.add_row({"cross", std::to_string(button_pad.cross)});
	button_state_table.add_row({"square", std::to_string(button_pad.square)});

	button_state_table.column(1).format().border_left(":");

	buttons_table.add_row(Row_t{button_state_table});

	return buttons_table;
}

Table other_buttons_to_table(const State::Buttons& buttons)
{
	Table buttons_table;
	buttons_table.add_row(Row_t{"Other Buttons"});
	buttons_table[0].format().font_color(Color::yellow).font_align(FontAlign::center);

	Table button_state_table;

	button_state_table.add_row({"l1", std::to_string(buttons.l1), "r1", std::to_string(buttons.r1)});
	button_state_table.add_row({"l2", std::to_string(buttons.l2), "r2", std::to_string(buttons.r2)});
	button_state_table.add_row({"l3", std::to_string(buttons.l3), "r3", std::to_string(buttons.r3)});

	button_state_table.add_row({"create", std::to_string(buttons.create), "menu", std::to_string(buttons.menu)});
	button_state_table.add_row({"home", std::to_string(buttons.home), "touchpad", std::to_string(buttons.touchpad)});
	button_state_table.add_row({"mute", std::to_string(buttons.mute)});

	button_state_table.column(1).format().border_left(":");
	button_state_table.column(3).format().border_left(":");

	buttons_table.add_row(Row_t{button_state_table});

	return buttons_table;
}

Table gyro_to_table(const State::Gyro& gyro)
{
	Table gyro_table;
	gyro_table.add_row(Row_t{"Gyroscope"});
	gyro_table[0].format().font_color(Color::yellow).font_align(FontAlign::center);

	const auto pitch = std::to_string(gyro.pitch);
	const auto yaw = std::to_string(gyro.yaw);
	const auto roll = std::to_string(gyro.roll);

	Table gyro_value_table;
	gyro_value_table.add_row(Row_t{"Pitch", pitch});
	gyro_value_table.add_row(Row_t{"Yaw", yaw});
	gyro_value_table.add_row(Row_t{"Roll", roll});

	gyro_value_table.column(1).format().border_left(":");

	gyro_table.add_row(Row_t{gyro_value_table});

	return gyro_table;
}

Table acceleration_to_table(const State::Acceleration& acceleration)
{
	Table acceleration_table;
	acceleration_table.add_row(Row_t{"Acceleration"});
	acceleration_table[0].format().font_color(Color::yellow).font_align(FontAlign::center);

	const auto x = std::to_string(acceleration.x);
	const auto y = std::to_string(acceleration.y);
	const auto z = std::to_string(acceleration.z);

	Table acceleration_value_table;
	acceleration_value_table.add_row(Row_t{"X", x});
	acceleration_value_table.add_row(Row_t{"Y", y});
	acceleration_value_table.add_row(Row_t{"Z", z});

	acceleration_value_table.column(1).format().border_left(":");

	acceleration_table.add_row(Row_t{acceleration_value_table});

	return acceleration_table;
}

Table touch_point_to_table(size_t index, const State::TouchPoint& point)
{
	Table touch_point_table;
	touch_point_table.add_row(Row_t{fmt::format("Touch point {}", index)});
	touch_point_table[0].format().font_color(Color::yellow).font_align(FontAlign::center);

	const auto x = std::to_string(point.x);
	const auto y = std::to_string(point.y);
	const auto id = std::to_string(point.id);
	const auto active = std::to_string(point.active);

	Table touch_point_value_table;
	touch_point_value_table.add_row(Row_t{"X", x});
	touch_point_value_table.add_row(Row_t{"Y", y});
	touch_point_value_table.add_row(Row_t{"id", id});
	touch_point_value_table.add_row(Row_t{"active", active});

	touch_point_value_table.column(1).format().border_left(":");

	touch_point_table.add_row(Row_t{touch_point_value_table});

	return touch_point_table;
}

Table battery_to_table(const State::Battery& battery)
{
	Table battery_table;
	battery_table.add_row(Row_t{"Battery"});
	battery_table[0].format().font_color(Color::yellow).font_align(FontAlign::center);

	const auto charging = std::to_string(battery.power_status == State::PowerStatus::CHARGING);
	const auto charged = std::to_string(battery.power_status == State::PowerStatus::CHARGED);
	const auto level = std::to_string(battery.level);

	Table battery_value_table;
	battery_value_table.add_row(Row_t{"Charging", charging});
	battery_value_table.add_row(Row_t{"Charged", charged});
	battery_value_table.add_row(Row_t{"Level", level});

	battery_value_table.column(1).format().border_left(":");

	battery_table.add_row(Row_t{battery_value_table});

	return battery_table;
}

Table audio_to_table(const State::Audio& audio)
{
	Table audio_table;
	audio_table.add_row(Row_t{"Audio"});
	audio_table[0].format().font_color(Color::yellow).font_align(FontAlign::center);

	const auto muted = std::to_string(audio.muted);
	const auto headphones = std::to_string(audio.headphones_connected);
	const auto mic = std::to_string(audio.microphone_connected);

	Table audio_value_table;
	audio_value_table.add_row(Row_t{"Muted", muted});
	audio_value_table.add_row(Row_t{"Headphones", headphones});
	audio_value_table.add_row(Row_t{"Microphone", mic});

	audio_value_table.column(1).format().border_left(":");

	audio_table.add_row(Row_t{audio_value_table});

	return audio_table;
}

int main(int argc, char **argv)
{
	const auto enumerated = enumerate();
	print_enumeration(enumerated);

	for(size_t i = 0; i < enumerated.size(); ++i)
	{
		const auto& enumerated_pad = enumerated[i];
		const Gamepad gamepad(enumerated_pad);
		const auto state = gamepad.poll();

		Table gamepad_table;
		gamepad_table.add_row(Row_t{fmt::format("Gamepad {}", i)});
		gamepad_table[0]
			.format()
			.font_color(Color::yellow)
			.hide_border_bottom()
			.font_align(FontAlign::center);
		gamepad_table.add_row(Row_t{fmt::format("Connection: {}", to_string(enumerated_pad.connection_type))});
		gamepad_table[1]
				.format()
				.font_color(Color::green)
				.hide_border_top()
				.hide_border_bottom()
				.font_align(FontAlign::center);
		gamepad_table.add_row(Row_t{enumerated_pad.serial});
		gamepad_table[2]
				.format()
				.font_color(Color::blue)
				.hide_border_top()
				.hide_border_bottom()
				.font_align(FontAlign::center);

		Table gamepad_state_table;
		gamepad_state_table.format().hide_border();
		gamepad_state_table.add_row(Row_t{
			analog_to_table(state.left_pad, state.right_pad),
			trigger_to_table(state.left_trigger, state.right_trigger)
		});

		gamepad_state_table.add_row(Row_t{dpad_button_pad_to_table(state.dpad_direction, state.button_pad)});
		gamepad_state_table.add_row(Row_t{other_buttons_to_table(state.buttons)});
		gamepad_state_table.add_row(Row_t{gyro_to_table(state.gyro), acceleration_to_table(state.acceleration)});
		gamepad_state_table.add_row(Row_t{touch_point_to_table(0, state.touch_point_0), touch_point_to_table(1, state.touch_point_1)});
		gamepad_state_table.add_row(Row_t{battery_to_table(state.battery), audio_to_table(state.audio)});

		gamepad_table.add_row(Row_t{gamepad_state_table});
		gamepad_table[3]
				.format()
				.hide_border_top();
		std::cout << gamepad_table;
	}

    return 0;
}