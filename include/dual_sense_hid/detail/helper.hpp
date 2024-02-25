#ifndef DUAL_SENSE_HELPER_HPP
#define DUAL_SENSE_HELPER_HPP

#include <cstdint>
#include <bit>


namespace dual_sense::detail
{
#if defined(__cpp_lib_byteswap)
	uint16_t dual_sense_bswap16(uint16_t val)
	{
		return std::byteswap(val);
	}
#elif defined(_MSC_VER)
using dual_sense_bswap16 = _byteswap_ushort;
#elif defined(__has_builtin)
	#if __has_builtin(__builtin_bswap16)
		uint16_t dual_sense_bswap16(uint16_t val)
		{
			return __builtin_bswap16(val);
		}
	#endif
#elif !defined(dual_sense_bswap16)
	inline uint16_t dual_sense_bswap16(uint16_t val)
	{
		return  ((val << 8)  & 0xff00u) |
		        ((val >> 8)  & 0x00ffu);
	}
#endif

inline uint16_t le_to_native(uint16_t le)
{
	if constexpr (std::endian::native == std::endian::little)
	{
		return le;
	}
	else
	{
		return dual_sense_bswap16(le);
	}
}

	std::string wstring_to_string(const std::wstring &input)
	{
		auto &facet = std::use_facet<std::codecvt<wchar_t, char, std::mbstate_t>>(std::locale());
		std::mbstate_t mb{};

		std::string output(input.size() * static_cast<size_t>(facet.max_length()), '\0');

		const wchar_t *from_next;
		char *to_next;

		facet.out(mb, &input[0], &input[input.size()], from_next, &output[0], &output[output.size()], to_next);

		output.resize(static_cast<size_t>(to_next - &output[0]));

		return output;
	}
}

#endif //DUAL_SENSE_HELPER_HPP
