#ifndef DUAL_SENSE_STATE_HPP
#define DUAL_SENSE_STATE_HPP

#include <cstdint>


namespace dual_sense
{
	/**
	 * @brief State of analogs, buttons, etc. of gamepad
	 */
	struct State
	{
		/**
		 * @enum DPadDirection
		 * @brief Direction of pressed DigitalPad buttons
		 */
		enum class DPadDirection: uint8_t
		{
			UP          = 0, /*!< North button pressed */
			UP_RIGHT    = 1, /*!< North and east buttons pressed */
			RIGHT       = 2, /*!< East button pressed */
			DOWN_RIGHT  = 3, /*!< South and east buttons pressed */
			DOWN        = 4, /*!< South button pressed */
			DOWN_LEFT   = 5, /*!< South and west buttons pressed */
			LEFT        = 6, /*!< West button pressed */
			UP_LEFT     = 7, /*!< North and west buttons pressed */
			NONE        = 8  /*!< Default state */
		};

		/**
		 * @brief State of buttons
		 *
		 * @note State of each button is represented by boolean value:
		 * @note true - button down
		 * @note false - button up
		 */
		struct Buttons
		{
			bool l1; /*!< Left bumper state */
			bool r1; /*!< Right bumper state */
			bool l2; /*!< Left trigger as button state */
			bool r2; /*!< Right trigger as button state */
			bool create; /*!< Create button state */
			bool menu; /*!< Menu button state */
			bool l3; /*!< Left analog button state */
			bool r3; /*!< Right analog button state */
			bool home; /*!< Home button state */
			bool touchpad; /*!< Touchpad button state */
			bool mute; /*!< Mute button state */
		};

		/**
		 * @brief State of buttons on button pad
		 *
		 * @note State of each button is represented by boolean value:
		 * @note true - button down
		 * @note false - button up
		 */
		struct ButtonPad
		{
			bool triangle; /*!< Triangle button state */
			bool circle; /*!< Circle button state */
			bool cross; /*!< Cross button state */
			bool square; /*!< Square button state */
		};

		/**
		 * @brief Data of single touch point registered by touchpad
		 * @note Point (0, 0) is located on top left corner of touchpad
		 */
		struct TouchPoint
		{
			bool active; /*!< True if structure holds data of active touch point */
			uint16_t x; /*!< X coordinate (0-1920) */
			uint16_t y; /*!< X coordinate (0-1080) */
			uint8_t id; /*!< ID of touch point (7-bit). Increased after each new touch point detected */
		};

		/**
		 * @enum PowerStatus
		 * @brief Status of power
		 */
		enum class PowerStatus: uint8_t
		{
			DISCHARGING     = 0x00, /*!< Battery is discharging */
			CHARGING        = 0x01, /*!< Battery is charging */
			CHARGED         = 0x02, /*!< Battery is charged */
			VOLTAGE_ERROR   = 0x0A, /*!< Voltage error */
			TEMP_ERROR      = 0x0B, /*!< Temperature error */
			CHARGING_ERROR  = 0x0F  /*!< Unknown error */
		};

		/**
		 * @brief State of gamepad's battery
		 */
		struct Battery
		{
			uint8_t level; /*!< Current battery level */
			PowerStatus power_status; /*!< Power status */
		};

		/**
		 * @brief Information about audio capabilities
		 */
		struct Audio
		{
			bool muted; /*!< true if internal microphone is muted. Not related to state of led indicator */
			bool headphones_connected; /*!< Headphones connected via headphone jack socket */
			bool microphone_connected; /*!< Headphones with microphone built-in */
		};

		/**
		 * @brief Report of angular velocity
		 */
		struct Gyro
		{
			int32_t pitch; /*!< Pitch velocity */
			int32_t yaw; /*!< Yaw velocity */
			int32_t roll; /*!< Roll velocity */
		};

		/**
		 * @brief Report of linear acceleration
		 */
		struct Acceleration
		{
			int32_t x; /*!< X axis acceleration */
			int32_t y; /*!< Y axis acceleration */
			int32_t z; /*!< Z axis acceleration */
		};

		/**
		 * @brief State of analog pad
		 */
		struct AnalogPad
		{
			uint8_t x; /*!< Stick X position */
			uint8_t y; /*!< Stick Y position */
		};

		/**
		 * @brief State of trigger
		 */
		struct Trigger
		{
			uint8_t value; /*!< Trigger position */
			uint8_t stop_location; /*!< Trigger stop location */
		};

		/**
		 * @name Analog pads state
		 */
		///@{
		/** Left stick state */
		AnalogPad left_pad;
		/** Right stick state */
		AnalogPad right_pad;
		///@}

		/**
		 * @name Triggers state
		 */
		///@{
		/** Left trigger state */
		Trigger left_trigger;
		/** Right trigger state */
		Trigger right_trigger;
		///@}

		DPadDirection dpad_direction; /*!< Direction created from state of pushed buttons on arrow button pad */
		ButtonPad button_pad; /*!< State of right button pad (circle, square, triangle, cross) */
		Buttons buttons; /*!< Other buttons */

		Gyro gyro; /*!< Gyroscope state */
		Acceleration acceleration; /*!< Accelerometer state */

		uint8_t temperature; /*!< Gamepad temperature */

		/**
		 * @name Touchpad state
		 * @note Touchpad support multi touch (2 touch points)
		 */
		///@{
		/** First touch point */
		TouchPoint touch_point_0;
		/** Second touch point */
		TouchPoint touch_point_1;
		///@}

		Battery battery; /*!< Battery state (level, charging, charged) */

		Audio audio; /*!< State of connected headphones and internal mic */
	};
}

#endif //DUAL_SENSE_STATE_HPP
