#ifndef DUAL_SENSE_DEVICE_INFO_HPP
#define DUAL_SENSE_DEVICE_INFO_HPP

#include <string>

#include "enums.hpp"


namespace dual_sense
{
	/**
	 * @brief Enumerated info of connected gamepad
	 */
	struct DeviceInfo
	{
		std::string path; /*!< OS device path */

		std::string serial; /*!< Serial number of gamepad */
		std::string manufacturer_string; /*!< Manufacturer name */
		std::string product_string; /*!< Product name of gamepad */

		/**
		 * @brief Type of connection
		 */
		ConnectionType connection_type;
	};
}

#endif //DUAL_SENSE_DEVICE_INFO_HPP
