#ifndef DUAL_SENSE_HID_ENUMS_HPP
#define DUAL_SENSE_HID_ENUMS_HPP

#include <cstdint>


namespace dual_sense_hid
{
	/**
 	 * @enum ConnectionType
	 * @brief method of connection
 	 */
	enum class ConnectionType: uint8_t
	{
		USB, /*!< USB connection */
		BLUETOOTH /*!< Bluetooth connection */
	};
}

#endif //DUAL_SENSE_HID_ENUMS_HPP
