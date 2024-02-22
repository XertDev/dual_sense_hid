#include <string>
#include <chrono>
#include <thread>

#include <tabulate/table.hpp>

#include "dual_sense_hid/gamepad.hpp"


using namespace tabulate;
using namespace dual_sense;
using namespace std::chrono_literals;
using Row_t = Table::Row_t;


inline std::string to_string(ConnectionType connection_type)
{
	return connection_type == ConnectionType::USB ? "USB" : "BT";
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

using RGB = std::tuple<uint8_t, uint8_t, uint8_t>;

RGB interpolate(RGB src, RGB dst, float scale)
{
	const float normalized_scale = std::clamp(scale, 0.0f, 1.0f);

	const auto red = static_cast<uint8_t>((1.0f - normalized_scale) * static_cast<float>(std::get<0>(src)) + normalized_scale * static_cast<float>(std::get<0>(dst)));
	const auto green = static_cast<uint8_t>((1.0f - normalized_scale) * static_cast<float>(std::get<1>(src)) + normalized_scale * static_cast<float>(std::get<1>(dst)));
	const auto blue = static_cast<uint8_t>((1.0f - normalized_scale) * static_cast<float>(std::get<2>(src)) + normalized_scale * static_cast<float>(std::get<2>(dst)));

	return {red, green, blue};
}

constexpr size_t STEP_COUNT = 200;
constexpr auto STEP_DELAY = 50ms;

constexpr RGB RED = {255, 0, 0};
constexpr RGB GREEN = {0, 255, 0};
constexpr RGB BLUE = {0, 0, 255};
constexpr RGB PINK = {243, 75, 125};

void gradient_touchpad_backlight_demo(Gamepad& gamepad)
{
	auto& lights = gamepad.lights();

	const auto set_color = [&lights](uint8_t red, uint8_t green, uint8_t blue)
	{
		lights.set_touchpad_light_color(red, green, blue);
	};

	std::cout << "Gradient from RED to BLUE - max light" << std::endl;

	lights.set_player_indicator_brightness(dual_sense::Gamepad::Lights::PlayerIndicatorBrightness::MAX);
	gamepad.push_state();

	std::apply(set_color, RED);
	gamepad.push_state();

	for (size_t i = 0; i < STEP_COUNT; ++i)
	{
		std::this_thread::sleep_for(STEP_DELAY);

		float step = (1.0f / STEP_COUNT) * static_cast<float>(i);

		const auto gradient_color = interpolate(RED, BLUE, step);
		std::apply(set_color, gradient_color);
		gamepad.push_state();
	}

	std::cout << "Gradient from BLUE to GREEN - mid light" << std::endl;

	lights.set_player_indicator_brightness(dual_sense::Gamepad::Lights::PlayerIndicatorBrightness::MEDIUM);

	for (size_t i = 0; i < STEP_COUNT; ++i)
	{
		std::this_thread::sleep_for(STEP_DELAY);

		float step = (1.0f / STEP_COUNT) * static_cast<float>(i);

		const auto gradient_color = interpolate(BLUE, GREEN, step);
		std::apply(set_color, gradient_color);
		gamepad.push_state();
	}

	std::this_thread::sleep_for(1s);
}

void player_indicator_demo(Gamepad& gamepad)
{
	auto& lights = gamepad.lights();

	std::this_thread::sleep_for(1s);

	std::cout << "5..";
	std::flush(std::cout);

	gamepad.lights().set_player_indicator(dual_sense::Gamepad::Lights::PlayerIndicator::PLAYER_FIVE);
	gamepad.push_state();

	std::this_thread::sleep_for(1s);

	std::cout << "4..";
	std::flush(std::cout);

	gamepad.lights().set_player_indicator(dual_sense::Gamepad::Lights::PlayerIndicator::PLAYER_FOUR);
	gamepad.push_state();

	std::this_thread::sleep_for(1s);

	std::cout << "3..";
	std::flush(std::cout);

	gamepad.lights().set_player_indicator(dual_sense::Gamepad::Lights::PlayerIndicator::PLAYER_THREE);
	gamepad.push_state();

	std::this_thread::sleep_for(2s);

	std::cout << "2..";
	std::flush(std::cout);

	gamepad.lights().set_player_indicator(dual_sense::Gamepad::Lights::PlayerIndicator::PLAYER_TWO);
	gamepad.push_state();

	std::this_thread::sleep_for(2s);

	std::cout << "1..";
	std::flush(std::cout);

	gamepad.lights().set_player_indicator(dual_sense::Gamepad::Lights::PlayerIndicator::PLAYER_ONE);
	gamepad.push_state();

	std::this_thread::sleep_for(2s);

	std::cout << "0" << std::endl;

	gamepad.lights().set_player_indicator(dual_sense::Gamepad::Lights::PlayerIndicator::DISABLED);
	gamepad.push_state();
}

void mute_mic_indicator_demo(Gamepad& gamepad)
{
	auto& lights = gamepad.lights();

	{
		std::this_thread::sleep_for(2s);

		std::cout << "Muted" << std::endl;

		gamepad.lights().set_mute_light_mode(dual_sense::Gamepad::Lights::MuteLightMode::ON);
		gamepad.push_state();
	}

	{
		std::this_thread::sleep_for(2s);

		std::cout << "Unmuted" << std::endl;

		gamepad.lights().set_mute_light_mode(dual_sense::Gamepad::Lights::MuteLightMode::OFF);
		gamepad.push_state();
	}

	{
		std::this_thread::sleep_for(1s);

		std::cout << "Breathing" << std::endl;

		gamepad.lights().set_mute_light_mode(dual_sense::Gamepad::Lights::MuteLightMode::BREATHING);
		gamepad.push_state();

		std::this_thread::sleep_for(4s);
	}
	gamepad.lights().set_mute_light_mode(dual_sense::Gamepad::Lights::MuteLightMode::OFF);
}

int main(int argc, char **argv)
{
	const auto enumerated = enumerate();
	print_enumeration(enumerated);

	if(enumerated.empty())
	{
		std::cout << "No valid device found.\n";
		std::cout << "Stopping demo...\n";
		std::cout << "Bye!" << std::endl;

		return 0;
	}

	std::cout << "Selecting first found device...\n";
	std::cout << "Selected: " << enumerated.front().serial << std::endl;


	auto demo_device = Gamepad(enumerated.front());

	gradient_touchpad_backlight_demo(demo_device);

	player_indicator_demo(demo_device);

	mute_mic_indicator_demo(demo_device);

	return 0;
}