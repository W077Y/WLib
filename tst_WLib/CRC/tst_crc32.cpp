#include <CRC/crc32.h>
#include <catch.hpp>

TEST_CASE("tst_crc32")
{
  char tst_str[] = {
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
  };

  uint32_t crc = WLib::CRC::crc32(reinterpret_cast<std::byte const*>(tst_str), std::size(tst_str));
  REQUIRE((crc) == 0xCBF43926);

  crc = WLib::CRC::crc32(reinterpret_cast<std::byte const*>(tst_str) + 0, 1);
  crc = WLib::CRC::crc32(crc, reinterpret_cast<std::byte const*>(tst_str) + 1, 2);
  crc = WLib::CRC::crc32(crc, reinterpret_cast<std::byte const*>(tst_str) + 3, 6);
  REQUIRE((crc) == 0xCBF43926);
}


TEST_CASE("tst_crc32 string")
{
  char tst_str[] = "This is a test string";

  uint32_t crc = WLib::CRC::crc32(reinterpret_cast<std::byte const*>(tst_str), std::size(tst_str)-1);
  REQUIRE((crc) == 0x6B4EF36D);
}
