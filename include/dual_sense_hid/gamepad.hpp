#ifndef DUAL_SENSE_HID_GAMEPAD_HPP
#define DUAL_SENSE_HID_GAMEPAD_HPP

#include <vector>
#include <string>

#include "device_info.hpp"
#include "state.hpp"
#include "enums.hpp"
#include "calibration.hpp"


struct hid_device_;
typedef struct hid_device_ hid_device;

/**
 * @namespace dual_sense
 * @brief Dual Sense support library
 */
namespace dual_sense_hid
{
	static constexpr unsigned long VENDOR_ID = 1356ul; /*!< Vendor ID of DualSense gamepad */
	static constexpr unsigned long PRODUCT_ID = 3302ul; /*!< Product ID of DualSense gamepad */

	/**
	 * @brief Enumerate connected gamepads
	 * \return A vector of info for connected devices
	 */
	std::vector<DeviceInfo> enumerate();

	/**
	 * @brief Gamepad handle used to poll state from device
	 */
	class Gamepad
	{
	public:
		/**
		 * @brief Proxy object for calls which mutate state of gamepad's lights
		 */
		class Lights
		{
		public:
			/**
			 * @enum PlayerIndicatorBrightness
			 * @brief Player Indicator Brightness level
			 */
			enum class PlayerIndicatorBrightness: uint8_t
			{
				MAX         = 0, /*!< Maximum level */
				MEDIUM      = 1, /*!< Medium level */
				LOW         = 2  /*!< Low level */
			};

			/**
			 * @enum PlayerIndicator
			 * @brief Player Indicator state
			 */
			enum class PlayerIndicator: uint8_t
			{
				PLAYER_ONE      = 0, /*!< Player one (1 LED) */
				PLAYER_TWO      = 1, /*!< Player two (2 LEDs) */
				PLAYER_THREE    = 2, /*!< Player three (3 LEDs) */
				PLAYER_FOUR     = 3, /*!< Player four (4 LEDs) */
				PLAYER_FIVE     = 4, /*!< Player five (5 LEDs) */
				DISABLED        = 5  /*!< LEDs disabled */
			};

			/**
			 * @enum MuteLightMode
			 * @brief Mute light mode
			 */
			enum class MuteLightMode: uint8_t
			{
				OFF,        /*!< Off */
				ON,         /*!< On */
				BREATHING   /*!< Pulsating */
			};

			/**
			 * @brief Set light mode of mute button
			 * @param mute_light_mode mute light mode to be set on gamepad after state push
			 * @see MuteLightMode
			 */
			void set_mute_light_mode(MuteLightMode mute_light_mode);

			/**
			 * @brief Set indicator which represents player number
			 * @param indicator player number (1-5) or disabled
			 * @see PlayerIndicator
			 */
			void set_player_indicator(PlayerIndicator indicator);
			/**
			 * @brief Set brightness level of player indicator
			 * @param brightness brightness level
			 * @see PlayerIndicatorBrightness
			 */
			void set_player_indicator_brightness(PlayerIndicatorBrightness brightness);

			/**
			 * @brief Set color of touchpad backlight
			 * @param red brightness level
			 * @param green brightness level
			 * @param blue brightness level
			 */
			void set_touchpad_light_color(uint8_t red, uint8_t green, uint8_t blue);

		private:
			MuteLightMode mute_light_mode_;

			PlayerIndicator player_indicator_;
			PlayerIndicatorBrightness player_indicator_brightness_;

			uint8_t touchpad_light_red_;
			uint8_t touchpad_light_green_;
			uint8_t touchpad_light_blue_;

			bool changed_ = true;

			friend class Gamepad;
		};

		/**
		 * @brief Constructor
		 * @param device_info	Device info to create gamepad instance for
		 * @param fetch_calibration_data Prefetch calibration data while initializing (default: true)
		 */
		explicit Gamepad(const DeviceInfo& device_info, bool fetch_calibration_data=true);

		/**
		 * @brief Poll state of gamepad from report queue
		 * @param use_calibration_data Apply accelerometer & gyroscope calibration data to readings
		 * @return A state of gamepad associated with current object
		 */
		[[nodiscard]] State poll(bool use_calibration_data=true) const;

		/**
		 * @brief Push internal gamepad state to real device
		 * @param full_update When set to false push only changed sections, otherwise push everything
		 */
		void push_state(bool full_update = false);

		/**
		 * @brief Get calibration data (cached)
		 * @return Calibration data from gamepad
		 */
		const Calibration& get_calibration_data() const;

		/**
		 * @brief Get gamepad's lights proxy object
		 * @return Gamepad's lights proxy object
		 */
		[[nodiscard]] Lights& lights();

	private:
		hid_device* device_;
		ConnectionType connection_type_;

		mutable bool calibration_data_loaded_ = false;
		mutable Calibration calibration_data_;

		Lights lights_;

		void take_lights_control();
	};
}

#endif //DUAL_SENSE_HID_GAMEPAD_HPP
