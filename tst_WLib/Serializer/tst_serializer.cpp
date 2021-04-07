#include <WLib_Serializer.h>
#include <catch.hpp>

struct not_serializable_type
{
};

TEST_CASE("tst_serializer check serialized_size")
{
  REQUIRE(WLib::is_serializeable_v<std::byte>);
  REQUIRE(WLib::is_serializeable_v<char>);
  REQUIRE(WLib::is_serializeable_v<short>);
  REQUIRE(WLib::is_serializeable_v<int>);
  REQUIRE(WLib::is_serializeable_v<long>);
  REQUIRE(WLib::is_serializeable_v<long long>);

  REQUIRE_FALSE(WLib::is_serializeable_v<not_serializable_type>);
}

TEST_CASE("tst_serializer check a")
{
  REQUIRE(WLib::byte_order_t::native == WLib::byte_order_t::little_endian);

  std::byte          buf[25] = {};
  WLib::serializer_t ser(buf);

  REQUIRE(ser.get_begin() == buf);
  REQUIRE(ser.get_end() == buf + 25);
  REQUIRE(ser.get_position() == buf);
  REQUIRE(ser.get_byte_order() == WLib::byte_order_t::native);

  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_free() == 25);
  REQUIRE(ser.get_used() == 0);

  ser.push_back(static_cast<uint16_t>(0xAABB));

  REQUIRE(ser.get_position() == &buf[2]);
  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_free() == 23);
  REQUIRE(ser.get_used() == 2);

  ser.push_back(static_cast<int16_t>(0xCCDD));
  ser.set_byte_order(WLib::byte_order_t::big_endian);
  ser.push_back(static_cast<int32_t>(0x0102'0304));
  ser.push_back(static_cast<uint32_t>(0x0807'0605));

  REQUIRE(ser.get_position() == &buf[12]);
  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_free() == 13);
  REQUIRE(ser.get_used() == 12);

  REQUIRE(ser.push_back(static_cast<uint64_t>(0x0F0E'0D0C'0B0A'0908)) == 8);
  REQUIRE(ser.push_back(static_cast<int64_t>(0x0706'0504'0302'0100)) == 5);

  constexpr unsigned char ref_1[25] = { 0xBB, 0xAA, 0xDD, 0xCC, 0x01, 0x02, 0x03, 0x04, 0x08,
                                        0x07, 0x06, 0x05, 0x0F, 0x0E, 0x0D, 0x0C, 0x0B, 0x0A,
                                        0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03 };

  for (std::size_t i = 0; i < 25; i++)
    REQUIRE(buf[i] == static_cast<std::byte>(ref_1[i]));

  ser.clear();

  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_free() == 25);
  REQUIRE(ser.get_used() == 0);

  REQUIRE(ser.push_back(static_cast<int64_t>(0x0706'0504'0302'0100), 5) == 3);

  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_free() == 22);
  REQUIRE(ser.get_used() == 3);

  constexpr char ref_2[3] = { 0x02, 0x01, 0x00 };

  for (std::size_t i = 0; i < 3; i++)
    REQUIRE(buf[i] == static_cast<std::byte>(ref_2[i]));
}

TEST_CASE("tst_deserializer check a")
{
  REQUIRE(WLib::byte_order_t::native == WLib::byte_order_t::little_endian);

  constexpr std::byte buf_1[25] = {
    static_cast<std::byte>(0xBB), static_cast<std::byte>(0xAA), static_cast<std::byte>(0xDD),
    static_cast<std::byte>(0xCC), static_cast<std::byte>(0x01), static_cast<std::byte>(0x02),
    static_cast<std::byte>(0x03), static_cast<std::byte>(0x04), static_cast<std::byte>(0x08),
    static_cast<std::byte>(0x07), static_cast<std::byte>(0x06), static_cast<std::byte>(0x05),
    static_cast<std::byte>(0x0F), static_cast<std::byte>(0x0E), static_cast<std::byte>(0x0D),
    static_cast<std::byte>(0x0C), static_cast<std::byte>(0x0B), static_cast<std::byte>(0x0A),
    static_cast<std::byte>(0x09), static_cast<std::byte>(0x08), static_cast<std::byte>(0x07),
    static_cast<std::byte>(0x06), static_cast<std::byte>(0x05), static_cast<std::byte>(0x04),
    static_cast<std::byte>(0x03)
  };
  constexpr std::byte buf_2[3] = { static_cast<std::byte>(0x02), static_cast<std::byte>(0x01),
                                   static_cast<std::byte>(0x00) };

  WLib::deserializer_t ser(buf_1);

  REQUIRE(ser.get_begin() == buf_1);
  REQUIRE(ser.get_end() == buf_1 + 25);
  REQUIRE(ser.get_position() == buf_1);
  REQUIRE(ser.get_byte_order() == WLib::byte_order_t::native);

  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_number_of_bytes_left() == 25);
  REQUIRE(ser.get_number_of_bytes_read() == 0);

  uint16_t val_1 = 0;
  ser(val_1);
  REQUIRE(val_1 == static_cast<uint16_t>(0xAABB));

  REQUIRE(ser.get_position() == &buf_1[2]);
  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_number_of_bytes_left() == 23);
  REQUIRE(ser.get_number_of_bytes_read() == 2);

  int16_t val_2 = 0;
  ser(val_2);
  REQUIRE(val_2 == static_cast<int16_t>(0xCCDD));

  ser.set_byte_order(WLib::byte_order_t::big_endian);

  int32_t val_3 = 0;
  ser(val_3);
  REQUIRE(val_3 == static_cast<int32_t>(0x0102'0304));
  uint32_t val_4 = 0;
  ser(val_4);
  REQUIRE(val_4 == static_cast<int32_t>(0x0807'0605));

  REQUIRE(ser.get_position() == &buf_1[12]);
  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_number_of_bytes_left() == 13);
  REQUIRE(ser.get_number_of_bytes_read() == 12);

  uint64_t val_5 = 0;
  int64_t  val_6 = 0;
  REQUIRE(ser(val_5) == 8);
  REQUIRE(ser(val_6) == 5);

  REQUIRE(static_cast<uint64_t>(0x0F0E'0D0C'0B0A'0908) == val_5);

  ser.back_to_begin();

  REQUIRE(ser.get_begin() == buf_1);
  REQUIRE(ser.get_end() == buf_1 + 25);
  REQUIRE(ser.get_position() == buf_1);
  REQUIRE(ser.get_byte_order() == WLib::byte_order_t::big_endian);

  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_number_of_bytes_left() == 25);
  REQUIRE(ser.get_number_of_bytes_read() == 0);

  WLib::deserializer_t ser_2(buf_2);

  REQUIRE(ser_2(val_6, 5, WLib::byte_order_t::big_endian) == 3);
  REQUIRE(static_cast<int64_t>(0x0706'0504'0302'0100) == val_6);


  REQUIRE(ser_2.get_size() == 3);
  REQUIRE(ser_2.get_number_of_bytes_left() == 0);
  REQUIRE(ser_2.get_number_of_bytes_read() == 3);
}

struct my_type_struct
{
  int8_t   a;
  uint64_t b;
};

template <> struct WLib::serializer_traits<my_type_struct>;

class my_type_class
{
public:
  my_type_class() = default;

private:
  [[maybe_unused]] int8_t   a = 9;
  [[maybe_unused]] uint64_t b = 20;

  friend WLib::serializer_traits<my_type_class>;
};

template <> struct WLib::serializer_traits<my_type_class>;

template <> struct WLib::serializer_traits<my_type_struct>
{
  using type_t                                 = std::remove_cv_t<my_type_struct>;
  static constexpr bool        is_serializable = true;
  static constexpr std::size_t serialized_size = 9;

  static constexpr std::byte const& get_byte_ref(type_t const& obj, std::size_t const& idx)
  {
    if (idx < serialized_size_v<int8_t>)
      return reinterpret_cast<std::byte const*>(&obj)[idx + offsetof(type_t, a)];
    return reinterpret_cast<std::byte const*>(
        &obj)[idx + offsetof(type_t, b) - serialized_size_v<int8_t>];
  }

  static constexpr std::byte& get_byte_ref(type_t& obj, std::size_t const& idx)
  {
    if (idx < serialized_size_v<int8_t>)
      return reinterpret_cast<std::byte*>(&obj)[idx + offsetof(type_t, a)];
    return reinterpret_cast<std::byte*>(
        &obj)[idx + offsetof(type_t, b) - serialized_size_v<int8_t>];
  }
};

template <> struct WLib::serializer_traits<my_type_class>
{
  using type_t                                 = std::remove_cv_t<my_type_class>;
  static constexpr bool        is_serializable = true;
  static constexpr std::size_t serialized_size = 9;

  static constexpr std::byte const& get_byte_ref(type_t const& obj, std::size_t const& idx)
  {
    if (idx < serialized_size_v<int8_t>)
      return reinterpret_cast<std::byte const*>(&obj)[idx + offsetof(type_t, a)];
    return reinterpret_cast<std::byte const*>(
        &obj)[idx + offsetof(type_t, b) - serialized_size_v<int8_t>];
  }

  static constexpr std::byte& get_byte_ref(type_t& obj, std::size_t const& idx)
  {
    if (idx < serialized_size_v<int8_t>)
      return reinterpret_cast<std::byte*>(&obj)[idx + offsetof(type_t, a)];
    return reinterpret_cast<std::byte*>(
        &obj)[idx + offsetof(type_t, b) - serialized_size_v<int8_t>];
  }
};

TEST_CASE("tst_serializer my type")
{
  REQUIRE(WLib::is_serializeable_v<my_type_struct>);
  REQUIRE(WLib::is_serializeable_v<my_type_class>);

  std::byte          buf[25];
  WLib::serializer_t ser(buf, WLib::byte_order_t::big_endian);

  my_type_struct tmp_struct;
  tmp_struct.a = -9;
  tmp_struct.b = 17;

  ser.push_back(tmp_struct);
  ser.set_byte_order(WLib::byte_order_t::little_endian);
  ser.push_back(tmp_struct);

  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_free() == 7);
  REQUIRE(ser.get_used() == 18);

  constexpr char ref_1[18] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, -9,
    -9,   0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };

  for (std::size_t i = 0; i < 18; i++)
    REQUIRE(buf[i] == static_cast<std::byte>(ref_1[i]));

  ser.clear();

  my_type_class tmp_class;

  ser.push_back(tmp_class);
  ser.set_byte_order(WLib::byte_order_t::big_endian);
  ser.push_back(tmp_class);

  REQUIRE(ser.get_size() == 25);
  REQUIRE(ser.get_free() == 7);
  REQUIRE(ser.get_used() == 18);

  constexpr uint8_t ref_2[18] = {
    9,    0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 9,
  };

  for (std::size_t i = 0; i < 18; i++)
    REQUIRE(buf[i] == static_cast<std::byte>(ref_2[i]));
}

TEST_CASE("tst_serializer block_transfer")
{
  std::byte block[60];
  for (std::size_t i = 0; i < std::size(block); i++)
    block[i] = static_cast<std::byte>(i + 10);

  std::byte          buf[25];
  WLib::serializer_t ser(buf);

  std::size_t len = 0;

  len += ser.push_back(block, len);
  REQUIRE(len == 25);

  for (std::size_t i = 0; i < std::size(buf); i++)
    REQUIRE(buf[i] == block[i]);

  ser.clear();
  len += ser.push_back(&block[0], std::size(block), len);

  REQUIRE(len == 50);
  for (std::size_t i = 0; i < std::size(buf); i++)
    REQUIRE(buf[i] == block[i + 25]);

  ser.clear();
  len += ser.push_back(block, len);

  REQUIRE(len == 60);
  for (std::size_t i = 0; i < 10; i++)
    REQUIRE(buf[i] == block[i + 50]);
}