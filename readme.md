# About
A constexpr guaranteed compile-time data obfuscator.

Obfuscated data storges in program and decodes in stack/heap.

Tested on MSVC 2019 and Clang-LLVM12.

Requires at least C++14.

# Usage
Include me and use macros `MP_OB` and `MP_OBD` to obfuscate datas.

Supports both multibyte and wide string literal.
``` C++
auto multibyte_string = MP_OB("Hello MultiBytes!\n");
std::cout << multibyte_string; // Hello MultiBytes!
std::cout << typeid(multibyte_string).name() << std::endl; // class std::basic_string<char,struct std::char_traits<char>,class std::allocator<char> >

auto wide_string = MP_OB(L"Hello WideChars!\n");
wprintf_s(wide_string.c_str()); // Hello WideChars!
std::cout << std::flush << typeid(wide_string).name() << std::endl; // class std::basic_string<wchar_t,struct std::char_traits<wchar_t>,class std::allocator<wchar_t> >
```
Also support data types.
``` C++
constexpr static int ints_raw[] = {
    INT_MAX, INT_MIN,
};
auto ints = MP_OBD(ints_raw);
_ASSERT((*ints)[0] == INT_MAX && (*ints)[1] == INT_MIN);
std::cout << std::endl << typeid(*ints).name() << std::endl; // class MoePus::ObData<int const ,8,3086120660>
```

# Result
Compile the code below with MSVC2019 /Ox x64
``` C++
constexpr static uint8_t sbox_raw[] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
};
auto sbox = MP_OBD(sbox_raw);
for (auto i = 0; i < sizeof(sbox_raw); i++)
{
    std::cout << std::hex << static_cast<int>((*sbox)[i]);
} // 637c777bf26b6fc5301672bfed7ab76ca82c97dfa5947f0add4a2af9ca472c0
```
And decompile with IDA
``` C++
int __cdecl main(int argc, const char **argv, const char **envp)
{
  // [COLLAPSED LOCAL DECLARATIONS. PRESS KEYPAD CTRL-"+" TO EXPAND]

  v3 = operator new(0x20ui64);
  *v3 = *&type_info::`vftable'[1];
  *(v3 + 1) = xmmword_140003340;
  v4 = ((v3 * (v3 - 1)) & 1) + 0xC0ABF1977807DE9i64;
  v5 = _mm_unpacklo_epi64(v4, v4);
  v6 = 0;
  for ( i = 0i64; i < 4; i += 2i64 )
    *&v3[8 * i] = _mm_xor_si128(v5, _mm_loadu_si128(&v3[8 * i]));
  v8 = v3;
  do
  {
    v9 = *v8;
    v10 = std::ostream::operator<<(std::cout, std::hex);
    std::ostream::operator<<(v10, v9);
    ++v6;
    ++v8;
  }
  while ( v6 < 0x20 );
  operator delete(v3, 0x20ui64);
  return 0;
}
```
As you can see the code will try to optmize into SIMD.