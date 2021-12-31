#ifndef DUAL_SENSE_STATE_HPP
#define DUAL_SENSE_STATE_HPP

#include <cstdint>


namespace dual_sense
{
	struct State
	{
		enum class DpadDirection: uint8_t
		{
			UP          = 0,
			UP_RIGHT    = 1,
			RIGHT       = 2,
			DOWN_RIGHT  = 3,
			DOWN        = 4,
			DOWN_LEFT   = 5,
			LEFT        = 6,
			UP_LEFT     = 7,
			NONE        = 8
		};

		struct Buttons
		{
			bool l1;
			bool r1;
			bool l2;
			bool r2;
			bool create;
			bool menu;
			bool l3;
			bool r3;
			bool ps;
			bool touchpad;
			bool mute;
		};

		struct ButtonPad
		{
			bool triangle;
			bool circle;
			bool cross;
			bool square;
		};

		struct TouchPoint
		{
			bool active;
			uint16_t x;
			uint16_t y;
			uint8_t id;
		};

		struct Battery
		{
			bool charging;
			bool charged;
			uint8_t level;
		};

		struct Audio
		{
			bool muted;
			bool headphones_connected;
			bool microphone_connected;
		};

		struct Gyro
		{
			int16_t pitch;
			int16_t yaw;
			int16_t roll;
		};

		struct Acceleration
		{
			int16_t x;
			int16_t y;
			int16_t z;
		};

		struct AnalogPad
		{
			uint8_t x;
			uint8_t y;
		};

		struct Trigger
		{
			uint8_t value;
			uint8_t feedback;
		};

		AnalogPad left_pad;
		AnalogPad right_pad;

		Trigger left_trigger;
		Trigger right_trigger;

		DpadDirection dpad_direction;
		ButtonPad button_pad;
		Buttons buttons;

		Gyro gyro;
		Acceleration acceleration;

		TouchPoint touch_point_0;
		TouchPoint touch_point_1;

		Battery battery;

		Audio audio;
	};
}

#endif //DUAL_SENSE_STATE_HPP
