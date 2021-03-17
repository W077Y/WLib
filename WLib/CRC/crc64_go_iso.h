#pragma once

#include <cstddef>
#include <cstdint>

namespace WLib::CRC
{
  uint64_t crc64_go_iso(std::byte const* beg, std::byte const* end);
  uint64_t crc64_go_iso(std::byte const* beg, std::size_t const& len);

  uint64_t crc64_go_iso(uint64_t const& init_value, std ::byte const* beg, std::byte const* end);
  uint64_t crc64_go_iso(uint64_t const& init_value, std::byte const* beg, std::size_t const& len);
}    // namespace WLib::CRC
