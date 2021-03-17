#pragma once

#include <cstddef>
#include <cstdint>

namespace WLib::CRC
{
  template <typename T> class CRC_Interface
  {
  public:
    virtual ~CRC_Interface()                                                       = default;
    virtual void reset() noexcept                                                  = 0;
    virtual T    operator()(std::byte const* beg, std::byte const* end) noexcept   = 0;
    virtual T    operator()(std::byte const* beg, std::size_t const& len) noexcept = 0;
    virtual T    get() const noexcept                                              = 0;
  };
}    // namespace WLib::CRC
