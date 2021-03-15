#pragma once

#include <cstddef>
#include <cstdint>

namespace WLib::CRC
{
  uint16_t crc16_ccitt(std::byte const* beg, std::byte const* end);
  uint16_t crc16_ccitt(std::byte const* beg, std::size_t const& len);

  uint16_t crc16_ccitt(uint16_t const& init_value, std ::byte const* beg, std::byte const* end);
  uint16_t crc16_ccitt(uint16_t const& init_value, std ::byte const* beg, std::size_t const& len);
}    // namespace WLib::CRC
