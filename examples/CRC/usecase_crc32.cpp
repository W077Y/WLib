#include <CRC/crc32.h>

int main()
{
  char const str_1[] = "Here I have a string";
  char const str_2[] = "Here I have another string";

  // calculate the checksum over the str_1 (excluding the null terminaton)
  uint32_t crc_1 =
      WLib::CRC::CRC_32()(reinterpret_cast<std::byte const*>(str_1), sizeof(str_1) - 1);

  if (crc_1 != 0x67FAA791)
    return -1;

  // calculate the checksum over str_1 and str_2
  // we already had the checksum over str_1 (crc_1) so wie just had to calculate the secound part
  WLib::CRC::CRC_32 crc_obj;
  crc_obj(reinterpret_cast<std::byte const*>(str_1), sizeof(str_1) - 1);    // part 1
  crc_obj(reinterpret_cast<std::byte const*>(str_2), sizeof(str_2) - 1);    // part 2

  // check the crc with an refernce value from
  // "http://www.sunshine2k.de/coding/javascript/crc/crc_js.html"
  if (crc_obj.get() == 0x401FEA6F)
    return 0;

  return -2;
}