#pragma once
#ifndef WLIB_STRINGBUILDER_HPP_INCLUDED
#define WLIB_STRINGBUILDER_HPP_INCLUDED

#include <array>
#include <charconv>
#include <concepts>
#include <cstdint>
#include <exception>
#include <span>
#include <stdexcept>
#include <string_view>

namespace wlib
{
  template <std::size_t N> class StringBuilder;

  template <> class StringBuilder<0>
  {
  public:
    constexpr StringBuilder(std::span<char> buffer)
        : m_buffer{ buffer }
    {
      if (this->m_buffer.size() < 1)
        throw std::runtime_error("buffer to short");

      this->m_buffer[this->max_idx()] = '\0';
    }

    constexpr std::size_t get_remaining_chars() const { return this->max_idx() - this->m_idx; }
    constexpr std::size_t get_length() const { return this->m_idx; }

    constexpr StringBuilder& append(char const* c_str)
    {
      for (; this->m_idx < this->max_idx(); this->m_idx++, c_str++)
      {
        this->m_buffer[this->m_idx] = *c_str;

        if (*c_str == '\0')
          break;
      }
      return *this;
    }

    constexpr StringBuilder& append(std::string_view str)
    {
      auto c_str = str.begin();
      for (; this->m_idx < this->max_idx() && c_str != str.end(); this->m_idx++, c_str++)
      {
        this->m_buffer[this->m_idx] = *c_str;
      }
      return *this;
    }

    constexpr StringBuilder& append(char c, std::size_t count = 1)
    {
      std::size_t const len = this->get_remaining_chars() < count ? this->get_remaining_chars() : count;

      for (std::size_t idx = 0; idx < len; idx++)
        this->m_buffer[this->m_idx++] = c;
      return *this;
    }
    constexpr char const* as_c_str() const { return this->m_buffer.data(); }

    void clear()
    {
      this->m_idx       = 0;
      this->m_buffer[0] = '\0';
      return;
    }

    constexpr std::string_view as_string_view() const { return std::string_view{ this->m_buffer.data(), this->m_idx }; }

  private:
    constexpr std::size_t max_idx() const { return this->m_buffer.size() - 1; }
    std::span<char>       m_buffer;
    std::size_t           m_idx = 0;
  };

  template <std::size_t N> class StringBuilder: public StringBuilder<0>
  {
  public:
    constexpr StringBuilder()
        : StringBuilder<0>(this->m_buffer)
    {
    }

  private:
    std::array<char, N + 1> m_buffer = {};
  };

  namespace Internal
  {
    struct number_format_base_t
    {
      enum class sign_mode_t
      {
        none,
        negativ_only,
        allways,
      };

      static constexpr auto parse_sign(char const*(&fmt)) -> sign_mode_t
      {
        switch (*fmt)
        {
        case '+':
          fmt++;
          return sign_mode_t::allways;
        case '-':
          fmt++;
          return sign_mode_t::negativ_only;

        default:
          return sign_mode_t::none;
        }
      }
      static constexpr auto parse_int(char const*(&fmt), int default_value) -> int
      {
        for (; *fmt == ' '; ++fmt)
        {
        }

        if (*fmt < '0' || '9' < *fmt)
          return default_value;

        int val = 0;
        for (; '0' <= *fmt && *fmt <= '9'; ++fmt)
        {
          val *= 10;
          val += *fmt - '0';
        }
        return val;
      }
      static constexpr auto parse_format(char const*(&fmt)) -> std::chars_format
      {
        switch (*fmt)
        {
        case 'E':
        case 'e':
          fmt++;
          return std::chars_format::scientific;
        case 'F':
        case 'f':
          fmt++;
          return std::chars_format::fixed;
        case 'X':
        case 'x':
          fmt++;
          return std::chars_format::hex;
        default:
          return std::chars_format::general;
        }
      }
      static constexpr auto parse_base(char const*(&fmt)) -> int
      {
        switch (*fmt)
        {
        case 'X':
        case 'x':
          fmt++;
          return 16;
        case 'B':
        case 'b':
          fmt++;
          return 2;
        default:
          return 10;
        }
      }
    };

    class floating_point_number_format_t: public number_format_base_t
    {
    public:
      constexpr floating_point_number_format_t(char const* fmt)
      {
        if (fmt == nullptr || *fmt == '\0')
          return;

        this->m_sign = parse_sign(fmt);

        this->m_width = parse_int(fmt, 0);

        if (*fmt == '.')
        {
          ++fmt;
          this->m_pre = parse_int(fmt, -1);
        }

        if (*fmt == '\0')
          return;

        this->m_fmt = parse_format(fmt);

        if (*fmt != '\0')
          throw std::invalid_argument("Invalid format specifier!");
      }

      constexpr sign_mode_t       get_sign() const { return this->m_sign; }
      constexpr int               get_width() const { return this->m_width; }
      constexpr int               get_precision() const { return this->m_pre; }
      constexpr std::chars_format get_format() const { return this->m_fmt; }

    private:
      sign_mode_t       m_sign  = sign_mode_t::none;
      int               m_width = 0;
      int               m_pre   = -1;
      std::chars_format m_fmt   = std::chars_format::general;
    };

    class integer_number_format_t: public number_format_base_t
    {
    public:
      constexpr integer_number_format_t(char const* fmt)
      {
        if (fmt == nullptr || *fmt == '\0')
          return;

        this->m_sign = parse_sign(fmt);

        this->m_width = parse_int(fmt, 0);

        if (*fmt == '\0')
          return;

        this->m_base = parse_base(fmt);

        if (*fmt != '\0')
          throw std::invalid_argument("Invalid format specifier!");
      }

      constexpr sign_mode_t get_sign() const { return this->m_sign; }
      constexpr int         get_width() const { return this->m_width; }
      constexpr int         get_base() const { return this->m_base; }

    private:
      sign_mode_t m_sign  = sign_mode_t::none;
      int         m_width = 0;
      int         m_base  = 10;
    };
  }    // namespace Internal

  template <typename T> class formator_t;

  template <std::floating_point T> class formator_t<T>
  {
    using fmt_t = Internal::floating_point_number_format_t;

  public:
    constexpr formator_t(char const* fmt, T const& val)
        : m_fmt{ fmt }
        , m_val(val)
    {
    }

    constexpr StringBuilder<0>& append_to(StringBuilder<0>& builder) const
    {
      char                 buf[50] = {};
      std::to_chars_result res     = { .ptr{ buf }, .ec{} };

      if (this->m_fmt.get_precision() < 0)
        res = std::to_chars(res.ptr, buf + sizeof(buf), this->m_val, this->m_fmt.get_format());
      else
        res = std::to_chars(res.ptr, buf + sizeof(buf), this->m_val, this->m_fmt.get_format(), this->m_fmt.get_precision());

      std::size_t const len  = res.ptr - buf;
      std::size_t       fill = this->m_fmt.get_width() < 0                             ? 0 :
                               len < static_cast<std::size_t>(this->m_fmt.get_width()) ? static_cast<std::size_t>(this->m_fmt.get_width()) - len :
                                                                                         0;

      bool const is_negativ = this->m_val < T{ 0 };

      if (this->m_fmt.get_sign() == fmt_t::sign_mode_t::none)
      {
        builder.append(' ', fill);
      }
      else if (this->m_fmt.get_sign() == fmt_t::sign_mode_t::negativ_only)
      {
        if (is_negativ && fill > 0)
          builder.append(' ', fill - 1);
        else
          builder.append(' ', fill);
      }
      if (this->m_fmt.get_sign() == fmt_t::sign_mode_t::allways)
      {
        if (fill > 0)
          builder.append(' ', fill - 1);
        if (!is_negativ)
          builder.append('+');
      }
      return builder.append(buf);
    }

  private:
    fmt_t    m_fmt;
    T const& m_val;
  };

  template <std::integral T> class formator_t<T>
  {
    using fmt_t = Internal::integer_number_format_t;

  public:
    constexpr formator_t(char const* fmt, T const& val)
        : m_fmt{ fmt }
        , m_val(val)
    {
    }

    constexpr StringBuilder<0>& append_to(StringBuilder<0>& builder) const
    {
      char                 buf[50] = {};
      std::to_chars_result res     = { .ptr{ buf }, .ec{} };

      res = std::to_chars(res.ptr, buf + sizeof(buf), this->m_val, this->m_fmt.get_base());

      std::size_t const len        = res.ptr - buf;
      std::size_t       fill       = this->m_fmt.get_width() < 0                             ? 0 :
                                     len < static_cast<std::size_t>(this->m_fmt.get_width()) ? static_cast<std::size_t>(this->m_fmt.get_width()) - len :
                                                                                               0;
      bool const        is_negativ = this->m_val < T{ 0 };

      if (this->m_fmt.get_sign() == fmt_t::sign_mode_t::none)
      {
        builder.append(' ', fill);
      }
      else if (this->m_fmt.get_sign() == fmt_t::sign_mode_t::negativ_only)
      {
        if (is_negativ && fill > 0)
          builder.append(' ', fill - 1);
        else
          builder.append(' ', fill);
      }
      if (this->m_fmt.get_sign() == fmt_t::sign_mode_t::allways)
      {
        if (fill > 0)
          builder.append(' ', fill - 1);
        if (!is_negativ)
          builder.append('+');
      }
      return builder.append(buf);
    }

  private:
    fmt_t    m_fmt;
    T const& m_val;
  };

  template <typename T> constexpr auto fmt(char const* fmt, T const& val) { return formator_t<T>{ fmt, val }; }

  template <typename T> constexpr StringBuilder<0>& operator<<(StringBuilder<0>& builder, T str) { return builder.append(str); }
  template <typename T> constexpr StringBuilder<0>& operator<<(StringBuilder<0>& builder, formator_t<T> const& val) { return val.append_to(builder); }

}    // namespace wlib

#endif
