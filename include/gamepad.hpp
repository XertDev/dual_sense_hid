#ifndef DUAL_SENSE_GAMEPAD_HPP
#define DUAL_SENSE_GAMEPAD_HPP

#include <vector>
#include <string>

#include "device_info.hpp"
#include "state.hpp"
#include "enums.hpp"


struct hid_device_;
typedef struct hid_device_ hid_device;

namespace dual_sense
{
	static constexpr unsigned long VENDOR_ID = 1356ul;
	static constexpr unsigned long PRODUCT_ID = 3302ul;

	std::vector<DeviceInfo> enumerate();

	class Gamepad
	{
	public:
		explicit Gamepad(const DeviceInfo& device_info);
		[[nodiscard]] State poll() const;
	private:
		hid_device* device_;
		ConnectionType connection_type_;
	};
}

#endif //DUAL_SENSE_GAMEPAD_HPP
