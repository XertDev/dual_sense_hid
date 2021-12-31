#ifndef DUAL_SENSE_ENUMS_HPP
#define DUAL_SENSE_ENUMS_HPP

#include <cstdint>


namespace dual_sense
{
    enum class ConnectionType: uint8_t
    {
        USB,
        BLUETOOTH
    };
}

#endif //DUAL_SENSE_ENUMS_HPP
