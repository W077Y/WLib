#pragma once

namespace WLib
{
  enum class byte_order_t
  {
    native,
    reverse,

    // todo: big Endian ?
    little_endian = native,
    big_endian    = reverse,
  };
}
