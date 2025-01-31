#pragma once
#ifndef WLIB_MEMORY_INTERFACE_HPP_INCLUDED
#define WLIB_MEMORY_INTERFACE_HPP_INCLUDED

#include <cstdint>
#include <span>

namespace wlib
{
  namespace memory
  {
    class Non_Volatile_Memory_Interface
    {
    public:
      virtual ~Non_Volatile_Memory_Interface() = default;

      virtual void write(std::size_t add, std::span<std::byte const> data) = 0;
      virtual void flush()                                                 = 0;
      virtual void read(std::size_t add, std::span<std::byte> data)        = 0;
    };
  }    // namespace memory

  namespace storage
  {
    template <typename T> class Non_Volatile_Storage_Interface
    {
    public:
      using value_type = std::remove_cvref_t<T>;

      virtual ~Non_Volatile_Storage_Interface() = default;

      virtual value_type load() const            = 0;
      virtual void       save(value_type const&) = 0;
    };
  }    // namespace storage
}    // namespace wlib

#endif    // WLIB_MEMORY_INTERFACE_HPP_INCLUDED
