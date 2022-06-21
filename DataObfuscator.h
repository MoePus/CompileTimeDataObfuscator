#pragma once
#include <iostream>
#include <memory>
namespace MoePus {
	using BlockType = size_t;

	constexpr BlockType inline GenKey() {
		const unsigned long long FNV_64_PRIME = 0x100000001b3ULL;
		unsigned long long hash = 14695981039346656037u;
		for (int i = 0; i < sizeof(__TIME__); i++)
		{
			hash ^= __TIME__[i];
			hash *= FNV_64_PRIME;
		}
		return static_cast<BlockType>(hash);
	}

	template<typename T, int N, BlockType KEY = MoePus::GenKey()>
	class ObData {
	public:
		constexpr static int block_count = N / sizeof(BlockType) + (N % sizeof(BlockType) ? 1 : 0);
		constexpr static BlockType key = KEY;
		constexpr static BlockType counts = N;
		constexpr ObData() {
		}
		template <class U> constexpr ObData(const U* _data) {
			for (int i = 0; i < N / sizeof(U); i++)
			{
				data[i * sizeof(U) / sizeof(BlockType)] |= 
					(BlockType(_data[i]) << ((i % (sizeof(BlockType) / sizeof(T))) * sizeof(U) * 8));
			}
			enc();
		}
		constexpr void enc()
		{
			for (int i = 0; i < block_count; i++)
			{
				data[i] ^= key;
			}
		}
		const T *dec()
		{
			BlockType randInt = rand();
			BlockType k = key + key * (((BlockType)randInt * ((BlockType)randInt + 1)) & 1);
			int i;
			for (i = 0; i < block_count - (N % sizeof(BlockType) ? 1 : 0); i++)
			{
				data[i] ^= k;
			}
			for (int j = 0; j < N % sizeof(BlockType); j++)
			{
				((char *)&data[i])[j] ^= (k >> (j * 8)) & 0xff;
			}
			return (T *)&data[0];
		}
		operator const T*() const
		{
			return (T*)get_data();
		};
		inline constexpr const BlockType* get_data() const { return data; }
	private:
		BlockType data[block_count]{};
	};

	template<typename T, size_t N>
	constexpr bool IsStringLiteral(T const(&)[N]) { return true; }

	template<typename T, size_t N>
	constexpr bool IsStringLiteral(T (&)[N]) { return false; }

	template<typename T>
	constexpr bool IsStringLiteral(T) { return false; }
}

#define MP_OB(x) \
[](){ \
static_assert(MoePus::IsStringLiteral(x), "MP_OB supports string literal only.");\
using data_type = std::remove_const_t<std::remove_pointer_t<std::decay_t<decltype(x)>>>; \
constexpr static auto sec = MoePus::ObData<const data_type, sizeof(x) - sizeof(data_type)>(x); \
constexpr int length = sizeof(x) / sizeof(data_type) - 1; \
std::basic_string<data_type, std::char_traits<data_type>, std::allocator<data_type>> p((data_type*)&sec, length); \
((std::remove_const_t<decltype(sec)>*)p.data())->dec(); \
return p; \
}()

#define MP_OBD(x) \
[](){ \
using data_type = std::remove_const_t<std::decay_t<decltype(x[0])>>; \
constexpr static auto sec = MoePus::ObData<const data_type, sizeof(x)>(x); \
auto d = std::make_unique<std::remove_const_t<decltype(sec)>>(sec); \
d->dec(); \
return d; \
}();
