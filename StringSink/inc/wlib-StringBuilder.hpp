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
  namespace format
  {
    enum class sign_mode_t
    {
      negativ_only,
      space,
      both,
    };

    enum class alignment_t
    {
      left,
      center,
      right,
    };

    using format_t = std::chars_format;
  }    // namespace format

  template <typename T> class formator_t;

  class StringBuilder
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

    template <typename T> constexpr StringBuilder& append(T const& val)
    {
      constexpr formator_t<T> fmt{};
      return fmt.append_to(*this, val);
    }

    void clear()
    {
      this->m_idx       = 0;
      this->m_buffer[0] = '\0';
      return;
    }

    constexpr char const*      as_c_str() const { return this->m_buffer.data(); }
    constexpr std::string_view as_string_view() const { return std::string_view{ this->m_buffer.data(), this->m_idx }; }

  private:
    constexpr std::size_t max_idx() const { return this->m_buffer.size() - 1; }
    std::span<char>       m_buffer;
    std::size_t           m_idx = 0;
  };

  template <std::size_t N> class StaticStringBuilder;

  template <> class StaticStringBuilder<0>: public StringBuilder
  {
  public:
    constexpr StaticStringBuilder(std::span<char> buffer)
        : StringBuilder{ buffer }
    {
    }
  };

  template <std::size_t N> class StaticStringBuilder: public StaticStringBuilder<0>
  {
  public:
    constexpr StaticStringBuilder()
        : StaticStringBuilder<0>(this->m_buffer)
    {
    }

  private:
    std::array<char, N + 1> m_buffer = {};
  };

  template <typename T> class formator_t;

  template <> class formator_t<std::string_view>
  {
  public:
    using alignment_t = format::alignment_t;

    constexpr formator_t() = default;
    constexpr formator_t(std::string_view pre, std::string_view post, int min_width, alignment_t alignment)
        : m_prefix(pre)
        , m_postfix(post)
        , m_width(min_width)
        , m_align(alignment)
    {
    }

    constexpr StringBuilder& append_to(StringBuilder& builder, std::string_view const& value) const
    {
      std::size_t const fill = value.length() < this->m_width ? this->m_width - value.length() : 0;

      builder.append(this->m_prefix);

      switch (this->m_align)
      {
      case alignment_t::right:
        builder.append(' ', fill);
        builder.append(value);
        break;

      case alignment_t::center:
        builder.append(' ', fill / 2);
        builder.append(value);
        builder.append(' ', fill - fill / 2);
        break;

      case alignment_t::left:
      default:
        builder.append(value);
        builder.append(' ', fill);
        break;
      }

      return builder.append(this->m_postfix);
    }

  private:
    std::string_view m_prefix  = {};
    std::string_view m_postfix = {};
    int              m_width   = 0;
    alignment_t      m_align   = alignment_t::left;
  };

  template <std::floating_point T> class formator_t<T>: protected formator_t<std::string_view>
  {
    using base_t = formator_t<std::string_view>;

  public:
    using value_type  = std::remove_cvref_t<T>;
    using sign_mode_t = format::sign_mode_t;
    using format_t    = format::format_t;

    struct floating_value_format_t
    {
      int         min_width = 0;
      int         precision = -1;
      sign_mode_t sign      = sign_mode_t::space;
      format_t    format    = format_t::fixed;
    };

    constexpr formator_t() = default;
    constexpr formator_t(std::string_view pre, std::string_view post, floating_value_format_t format)
        : base_t(pre, post, format.min_width, alignment_t::right)
        , m_fmt(format.format)
        , m_sign(format.sign)
        , m_precision(format.precision)
    {
    }

    constexpr StringBuilder& append_to(StringBuilder& builder, value_type const& value) const
    {
      char                 buf[50] = {};
      std::to_chars_result res     = { .ptr{ buf }, .ec{} };

      if (this->m_sign != sign_mode_t::negativ_only && static_cast<value_type>(0) <= value)
        *res.ptr++ = this->m_sign == sign_mode_t::both ? '+' : ' ';

      res = std::to_chars(res.ptr, buf + sizeof(buf), value, this->m_fmt, this->m_precision);

      if (res.ec != std::errc())
        throw std::runtime_error("formatting not possible");

      return base_t::append_to(builder, std::string_view{ buf, static_cast<std::size_t>(res.ptr - buf) });
    }

  private:
    std::chars_format m_fmt       = std::chars_format::general;
    sign_mode_t       m_sign      = sign_mode_t::negativ_only;
    int               m_precision = -1;
  };

  template <std::integral T> class formator_t<T>: protected formator_t<std::string_view>
  {
    using base_t = formator_t<std::string_view>;

  public:
    using value_type  = std::remove_cvref_t<T>;
    using sign_mode_t = format::sign_mode_t;

    struct integer_value_format_t
    {
      int         min_width = 0;
      sign_mode_t sign      = sign_mode_t::space;
      int         base      = 10;
    };

    constexpr formator_t() = default;
    constexpr formator_t(std::string_view pre, std::string_view post, integer_value_format_t format)
        : base_t(pre, post, format.min_width, alignment_t::right)
        , m_sign(format.sign)
        , m_base(format.base)
    {
    }

    constexpr StringBuilder& append_to(StringBuilder& builder, value_type const& value) const
    {
      char                 buf[50] = {};
      std::to_chars_result res     = { .ptr{ buf }, .ec{} };

      if (this->m_sign != sign_mode_t::negativ_only && static_cast<value_type>(0) <= value)
        *res.ptr++ = this->m_sign == sign_mode_t::both ? '+' : ' ';

      res = std::to_chars(res.ptr, buf + sizeof(buf), value, this->m_base);

      if (res.ec != std::errc())
        throw std::runtime_error("formatting not possible");

      return base_t::append_to(builder, std::string_view{ buf, static_cast<std::size_t>(res.ptr - buf) });
    }

  private:
    sign_mode_t m_sign = sign_mode_t::negativ_only;
    int         m_base = 10;
  };

  template <typename T> constexpr auto fmt(formator_t<T> const& formator, typename formator_t<T>::value_type const& val)
  {
    return std::pair<formator_t<T> const&, typename formator_t<T>::value_type const&>{ formator, val };
  }

  constexpr auto fmt(formator_t<std::string_view> const& formator, const char* str)
  {
    return std::pair<formator_t<std::string_view> const&, std::string_view>{ formator, std::string_view{ str } };
  }
  constexpr auto fmt(formator_t<std::string_view> const& formator, std::string_view const& str)
  {
    return std::pair<formator_t<std::string_view> const&, std::string_view const&>{ formator, str };
  }

  template <typename T> constexpr StringBuilder& operator<<(StringBuilder& builder, T str) { return builder.append(str); }

  constexpr StringBuilder& operator<<(StringBuilder& builder, std::pair<formator_t<std::string_view> const&, std::string_view> const& val)
  {
    return val.first.append_to(builder, val.second);
  }
  constexpr StringBuilder& operator<<(StringBuilder& builder, std::pair<formator_t<std::string_view> const&, std::string_view const&> const& val)
  {
    return val.first.append_to(builder, val.second);
  }

  template <typename T>
  constexpr StringBuilder& operator<<(StringBuilder& builder, std::pair<formator_t<T> const&, typename formator_t<T>::value_type const&> const& val)
  {
    return val.first.append_to(builder, val.second);
  }

}    // namespace wlib

#endif
