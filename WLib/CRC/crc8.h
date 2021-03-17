#pragma once

#include <CRC/crc_interface.h>
#include <cstddef>
#include <cstdint>

namespace WLib::CRC
{
  class CRC_8 final: CRC_Interface<uint8_t>
  {
  public:
    virtual void    reset() noexcept override;
    virtual uint8_t operator()(std::byte const* beg, std::byte const* end) noexcept override;
    virtual uint8_t operator()(std::byte const* beg, std::size_t const& len) noexcept override;
    virtual uint8_t get() const noexcept override;

  private:
    static constexpr uint8_t init_value = 0x00;
    uint8_t                  m_crc      = init_value;
  };
}    // namespace WLib::CRC
