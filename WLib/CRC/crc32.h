#pragma once

#include <cstddef>
#include <cstdint>

namespace WLib::CRC
{
  uint32_t crc32(std::byte const* beg, std::byte const* end);
  uint32_t crc32(std::byte const* beg, std::size_t const& len);

  uint32_t crc32(uint32_t const& init_value, std ::byte const* beg, std::byte const* end);
  uint32_t crc32(uint32_t const& init_value, std::byte const* beg, std::size_t const& len);
}    // namespace WLib::CRC
