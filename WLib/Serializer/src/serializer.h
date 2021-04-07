#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <serializer_traits.h>

namespace WLib
{
  namespace Internal
  {
    template <typename T>
    void deserialize_native(std::byte const*&  iter,
                            std::size_t const& len,
                            std::byte*         ptr,
                            std::size_t const& off)
    {
      for (std::size_t idx = off; idx < (off + len); ++iter, ++idx)
        serializer_traits<T>::get_byte_ref(ptr, idx) = *iter;
    }
    template <typename T>
    void deserialize_reverse(std::byte const*&  iter,
                             std::size_t const& len,
                             std::byte*         ptr,
                             std::size_t const& off)
    {
      for (std::size_t idx = off; idx < (off + len); ++iter, ++idx)
        serializer_traits<T>::set_byte_ref(ptr, serialized_size_v<T> - 1 - idx) = *iter;
    }
  }    // namespace Internal

  class Serializer_Interface
  {
  public:
    virtual ~Serializer_Interface() = default;

    virtual std::size_t get_size() const = 0;
    virtual std::size_t get_free() const = 0;
    virtual std::size_t get_used() const = 0;

    virtual byte_order_t get_byte_order() const              = 0;
    virtual void         set_byte_order(byte_order_t const&) = 0;

    virtual void        clear()                     = 0;
    virtual std::size_t push_back(std::byte const&) = 0;

    template <typename T>
    std::size_t push_back(T const& value, std::size_t byte_offset, byte_order_t byte_order)
    {
      return this->push_back(&value, 1, byte_offset, byte_order);
    }

    template <typename T> std::size_t push_back(T const& value, std::size_t byte_offset)
    {
      return this->push_back(value, byte_offset, this->get_byte_order());
    }

    template <typename T> std::size_t push_back(T const& value, byte_order_t byte_order)
    {
      return this->push_back(value, 0, byte_order);
    }

    template <typename T> std::size_t push_back(T const& value)
    {
      return this->push_back(value, 0, this->get_byte_order());
    }

    template <typename T, std::size_t N>
    std::size_t push_back(T const (&value)[N], std::size_t byte_offset, byte_order_t byte_order)
    {
      return this->push_back(value, N, byte_offset, byte_order);
    }

    template <typename T, std::size_t N>
    std::size_t push_back(T const (&value)[N], std::size_t byte_offset)
    {
      return this->push_back(value, N, byte_offset, this->get_byte_order());
    }

    template <typename T, std::size_t N>
    std::size_t push_back(T const (&value)[N], byte_order_t byte_order)
    {
      return this->push_back(value, N, 0, byte_order);
    }

    template <typename T, std::size_t N> std::size_t push_back(T const (&value)[N])
    {
      return this->push_back(value, N, 0, this->get_byte_order());
    }

    template <typename T>
    std::size_t push_back(T const* value, std::size_t N, std::size_t byte_offset)
    {
      return this->push_back(value, N, byte_offset, this->get_byte_order());
    }

    template <typename T>
    std::size_t
    push_back(T const* value, std::size_t N, std::size_t byte_offset, byte_order_t byte_order)
    {
      std::size_t len = 0;
      while (byte_offset < N * serialized_size_v<T>)
      {
        std::size_t const elm_idx = byte_offset / serialized_size_v<T>;

        std::size_t const off =
            (byte_order == byte_order_t::native) ?
                (byte_offset % serialized_size_v<T>) :
                (serialized_size_v<T> - 1 - (byte_offset % serialized_size_v<T>));

        if (this->push_back(serializer_traits<T>::get_byte_ref(value[elm_idx], off)) == 0)
          break;

        len++;
        byte_offset++;
      }
      return len;
    }
  };

  class serializer_t: public Serializer_Interface
  {
  public:
    constexpr serializer_t(std::byte*       begin,
                           std::byte const* end,
                           std::byte*       position,
                           byte_order_t     byte_order = byte_order_t::native) noexcept
        : m_beg(begin)
        , m_end(end)
        , m_pos(position)
        , m_byte_order(byte_order)
    {
    }

    template <std::size_t N>
    constexpr serializer_t(std::byte (&buffer)[N],
                           byte_order_t byte_order = byte_order_t::native) noexcept
        : serializer_t(buffer, buffer + N, buffer, byte_order)
    {
    }

    virtual ~serializer_t() = default;

    virtual void set_byte_order(byte_order_t const& byte_order) noexcept override
    {
      this->m_byte_order = byte_order;
    }
    virtual byte_order_t get_byte_order() const noexcept override { return this->m_byte_order; }

    virtual void clear() noexcept override { this->m_pos = this->m_beg; }

    std::byte*       get_begin() const noexcept { return this->m_beg; }
    std::byte const* get_end() const noexcept { return this->m_end; }
    std::byte*       get_position() const noexcept { return this->m_pos; }

    virtual std::size_t get_free() const noexcept override { return this->m_end - this->m_pos; }
    virtual std::size_t get_used() const noexcept override { return this->m_pos - this->m_beg; }
    virtual std::size_t get_size() const noexcept override { return this->m_end - this->m_beg; }

    using Serializer_Interface::push_back;
    virtual std::size_t push_back(std::byte const& val) noexcept override
    {
      if (this->m_pos < this->m_end)
      {
        *this->m_pos = val;
        this->m_pos++;
        return 1;
      }
      return 0;
    }

  private:
    std::byte* const       m_beg;
    std::byte const* const m_end;
    std::byte*             m_pos;
    byte_order_t           m_byte_order = byte_order_t::native;
  };

  class deserializer_t
  {
  public:
    constexpr deserializer_t(std::byte const* begin,
                             std::byte const* end,
                             std::byte const* position,
                             byte_order_t     byte_order = byte_order_t::native) noexcept
        : m_beg(begin)
        , m_end(end)
        , m_pos(position)
        , m_byte_order(byte_order)
    {
    }

    template <std::size_t N>
    constexpr deserializer_t(std::byte const (&buffer)[N],
                             byte_order_t byte_order = byte_order_t::native) noexcept
        : deserializer_t(buffer, buffer + N, buffer, byte_order)
    {
    }

    void set_byte_order(byte_order_t byte_order) noexcept { this->m_byte_order = byte_order; }
    byte_order_t get_byte_order() const noexcept { return this->m_byte_order; }

    std::byte const* get_begin() const noexcept { return this->m_beg; }
    std::byte const* get_end() const noexcept { return this->m_end; }
    std::byte const* get_position() const noexcept { return this->m_pos; }

    std::size_t get_number_of_bytes_left() const noexcept { return this->m_end - this->m_pos; }
    std::size_t get_number_of_bytes_read() const noexcept { return this->m_pos - this->m_beg; }
    std::size_t get_size() const noexcept { return this->m_end - this->m_beg; }
    void        back_to_begin() noexcept { this->m_pos = this->m_beg; }

    template <typename T>
    std::size_t operator()(T& value, std::size_t byte_offset, byte_order_t byte_order)
    {
      return this->operator()(&value, 1, byte_offset, byte_order);
    }

    template <typename T> std::size_t operator()(T& value, std::size_t byte_offset)
    {
      return this->operator()(value, byte_offset, this->m_byte_order);
    }

    template <typename T> std::size_t operator()(T& value, byte_order_t byte_order)
    {
      return this->operator()(value, 0, byte_order);
    }

    template <typename T> std::size_t operator()(T& value)
    {
      return this->operator()(value, 0, this->m_byte_order);
    }

    template <typename T, std::size_t N>
    std::size_t operator()(T (&value)[N], std::size_t byte_offset, byte_order_t byte_order)
    {
      return this->operator()(value, N, byte_offset, byte_order);
    }

    template <typename T, std::size_t N>
    std::size_t operator()(T (&value)[N], std::size_t byte_offset)
    {
      return this->operator()(value, N, byte_offset, this->m_byte_order);
    }

    template <typename T, std::size_t N>
    std::size_t operator()(T (&value)[N], byte_order_t byte_order)
    {
      return this->operator()(value, N, 0, byte_order);
    }

    template <typename T, std::size_t N> std::size_t operator()(T (&value)[N])
    {
      return this->operator()(value, N, 0, this->m_byte_order);
    }

    template <typename T> std::size_t operator()(T* value, std::size_t N, std::size_t byte_offset)
    {
      return this->operator()(value, N, byte_offset, this->m_byte_order);
    }

    template <typename T>
    std::size_t
    operator()(T* value, std::size_t N, std::size_t byte_offset, byte_order_t byte_order)
    {
      std::byte const* const init_pos = this->m_pos;
      while (this->m_pos < this->m_end && byte_offset < N * serialized_size_v<T>)
      {
        std::size_t const elm_idx = byte_offset / serialized_size_v<T>;

        std::size_t const off =
            (byte_order == byte_order_t::native) ?
                (byte_offset % serialized_size_v<T>) :
                (serialized_size_v<T> - 1 - (byte_offset % serialized_size_v<T>));

        serializer_traits<T>::get_byte_ref(value[elm_idx], off) = *this->m_pos;

        this->m_pos++;
        byte_offset++;
      }
      return this->m_pos - init_pos;
    }

  private:
    std::byte const* const m_beg;
    std::byte const* const m_end;
    std::byte const*       m_pos;
    byte_order_t           m_byte_order = byte_order_t::native;
  };

}    // namespace WLib