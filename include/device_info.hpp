#ifndef DUAL_SENSE_DEVICE_INFO_HPP
#define DUAL_SENSE_DEVICE_INFO_HPP

#include <string>

#include "enums.hpp"


namespace dual_sense
{
	struct DeviceInfo
	{
		std::string path;

		std::string serial;
		std::string manufacturer_string;
		std::string product_string;

		ConnectionType connection_type;
	};
}

#endif //DUAL_SENSE_DEVICE_INFO_HPP
