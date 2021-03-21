#include <algorithm>
#include <catch.hpp>

namespace WLib
{
  enum class byte_order_t
  {
    native,
    reversed,
    little_endian = native,
    big_endian    = reversed,
  };

  namespace Internal
  {
    void serialize_native(std::byte*(&pos), std::byte const* ptr, std::size_t const& len)
    {
      for (std::size_t idx = 0; idx < len;)
      {
        *pos++ = ptr[idx++];
      }
    }

    void serialize_reversed(std::byte*(&pos), std::byte const* ptr, std::size_t const& len)
    {
      for (std::size_t idx = len; idx > 0;)
      {
        *pos++ = ptr[--idx];
      }
    }
  }    // namespace Internal

  template <typename T> struct serializer_traits
  {
    static constexpr std::size_t serialized_size = sizeof(T);

    static constexpr std::size_t serialize(std::byte*(&pos),
                                           std::size_t const&  max_len,
                                           T const&            val,
                                           std::size_t const&  obj_off,
                                           byte_order_t const& byte_order);

    static constexpr std::size_t deserialize(std::byte const*(&pos),
                                             std::size_t const&  max_len,
                                             T&                  val,
                                             std::size_t const&  obj_off,
                                             byte_order_t const& byte_order);
  };

  template <typename T>
  constexpr std::size_t serialized_size = serializer_traits<T>::serialized_size;

  template <typename T>
  std::size_t serialize(std::byte*(&pos),
                        std::size_t const&  max_len,
                        T const&            obj,
                        std::size_t const&  byte_offset,
                        byte_order_t const& byte_order)
  {
    return serializer_traits<T>::serialize(pos, max_len, obj, byte_offset, byte_order);
  }

  template <typename T>
  std::size_t serialize(std::byte*(&pos),
                        std::byte const* const& end,
                        T const&                obj,
                        std::size_t const&      byte_offset,
                        byte_order_t const&     byte_order)
  {
    return serializer_traits<T>::serialize(pos, end - pos, obj, byte_offset, byte_order);
  }

  template <typename T>
  std::size_t deserialize(std::byte const*(&pos),
                          std::size_t const&  max_len,
                          T&                  obj,
                          std::size_t const&  byte_offset,
                          byte_order_t const& byte_order)
  {
    return serializer_traits<T>::deserialize(pos, max_len, obj, byte_offset, byte_order);
  }

  template <typename T>
  std::size_t deserialize(std::byte const*(&pos),
                          std::byte const* const& end,
                          T&                      obj,
                          std::size_t const&      byte_offset,
                          byte_order_t const&     byte_order)
  {
    return serializer_traits<T>::deserialize(pos, end - pos, obj, byte_offset, byte_order);
  }
}    // namespace WLib

#include <cstdint>

TEST_CASE("tst_serializer check serialized_size")
{
  REQUIRE(WLib::serialized_size<std::byte> == 1);
  REQUIRE(WLib::serialized_size<char> == 1);
  REQUIRE(WLib::serialized_size<signed char> == 1);
  REQUIRE(WLib::serialized_size<unsigned char> == 1);

  REQUIRE(WLib::serialized_size<char16_t> == 2);
  REQUIRE(WLib::serialized_size<char32_t> == 4);

  REQUIRE(WLib::serialized_size<short> == 2);
  REQUIRE(WLib::serialized_size<unsigned short> == 2);

  REQUIRE(WLib::serialized_size<int> == 4);
  REQUIRE(WLib::serialized_size<unsigned int> == 4);

  REQUIRE(WLib::serialized_size<long> == 4);
  REQUIRE(WLib::serialized_size<unsigned long> == 4);

  REQUIRE(WLib::serialized_size<long long> == 8);
  REQUIRE(WLib::serialized_size<unsigned long long> == 8);

  REQUIRE(WLib::serialized_size<float> == 4);
  REQUIRE(WLib::serialized_size<double> == 8);
  REQUIRE(WLib::serialized_size<long double> == 8);
}

TEST_CASE("tst_serializer check a")
{
  std::byte        buffer[25] = {};
  std::byte* const pos        = buffer;


}