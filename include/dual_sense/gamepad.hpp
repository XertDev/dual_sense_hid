#ifndef DUAL_SENSE_GAMEPAD_HPP
#define DUAL_SENSE_GAMEPAD_HPP

#include <vector>
#include <string>

#include "device_info.hpp"
#include "state.hpp"
#include "enums.hpp"


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
		 */
		explicit Gamepad(const DeviceInfo& device_info);

		/**
		 * @brief Poll state of gamepad from report queue
		 * @return A state of gamepad associated with current object
		 */
		[[nodiscard]] State poll() const;
	private:
		hid_device* device_;
		ConnectionType connection_type_;
	};
}

#endif //DUAL_SENSE_GAMEPAD_HPP
