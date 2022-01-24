#ifndef DUAL_SENSE_GAMEPAD_HPP
#define DUAL_SENSE_GAMEPAD_HPP

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
namespace dual_sense
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
		 * @brief Get calibration data (cached)
		 * @return Calibration data from gamepad
		 */
		const Calibration& get_calibration_data() const;
	private:
		hid_device* device_;
		ConnectionType connection_type_;

		mutable bool calibration_data_loaded_ = false;
		mutable Calibration calibration_data_;
	};
}

#endif //DUAL_SENSE_GAMEPAD_HPP
