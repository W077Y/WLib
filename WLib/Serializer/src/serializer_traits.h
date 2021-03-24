#pragma once

#include <byte_order.h>
#include <algorithm>
#include <cstddef>
#include <type_traits>

namespace WLib
{
  namespace Internal
  {
    struct not_seriailizeable_type
    {
      static constexpr bool is_serializable = false;
    };

    template <typename T> struct native_serializeable_type
    {
      using type_t                                 = std::remove_cv_t<T>;
      static constexpr bool        is_serializable = true;
      static constexpr std::size_t serialized_size = sizeof(type_t);

      static constexpr std::byte const& get_byte_ref(type_t const& obj, std::size_t const& idx)
      {
        return reinterpret_cast<std::byte const*>(&obj)[idx];
      }
      
      static constexpr std::byte& get_byte_ref(type_t& obj, std::size_t const& idx)
      {
        return reinterpret_cast<std::byte*>(&obj)[idx];
      }
    };

    template <typename T> constexpr bool is_native_serializable_v = false;

    template <> constexpr bool is_native_serializable_v<std::byte> = true;
    template <> constexpr bool is_native_serializable_v<char>      = true;
    template <> constexpr bool is_native_serializable_v<char16_t>  = true;
    template <> constexpr bool is_native_serializable_v<char32_t>  = true;

    template <> constexpr bool is_native_serializable_v<signed char>      = true;
    template <> constexpr bool is_native_serializable_v<signed short>     = true;
    template <> constexpr bool is_native_serializable_v<signed int>       = true;
    template <> constexpr bool is_native_serializable_v<signed long>      = true;
    template <> constexpr bool is_native_serializable_v<signed long long> = true;

    template <> constexpr bool is_native_serializable_v<unsigned char>      = true;
    template <> constexpr bool is_native_serializable_v<unsigned short>     = true;
    template <> constexpr bool is_native_serializable_v<unsigned int>       = true;
    template <> constexpr bool is_native_serializable_v<unsigned long>      = true;
    template <> constexpr bool is_native_serializable_v<unsigned long long> = true;

    template <> constexpr bool is_native_serializable_v<float>       = true;
    template <> constexpr bool is_native_serializable_v<double>      = true;
    template <> constexpr bool is_native_serializable_v<long double> = true;

    constexpr std::size_t sum(std::initializer_list<std::size_t> const V)
    {
      std::size_t ret = 0;
      for (std::size_t item : V)
        ret += item;
      return ret;
    }

  }    // namespace Internal

  template <typename T>
  struct serializer_traits
      : public std::conditional_t<Internal::is_native_serializable_v<T>,
                                  Internal::native_serializeable_type<T>,
                                  Internal::not_seriailizeable_type>
  {
  };

  template <typename T> constexpr bool is_serializeable_v = serializer_traits<T>::is_serializable;

  template <typename... Ts>
  constexpr std::size_t
      serialized_size_v = Internal::sum({ serializer_traits<Ts>::serialized_size... });
}    // namespace WLib
