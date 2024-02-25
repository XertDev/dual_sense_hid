#ifndef DUAL_SENSE_CRC32_HPP
#define DUAL_SENSE_CRC32_HPP

#include <cstdint>


namespace dual_sense::detail
{
	uint32_t crc32(const unsigned char* buffer, std::size_t size);
}

#endif //DUAL_SENSE_CRC32_HPP
