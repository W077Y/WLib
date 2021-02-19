#pragma once

#include <cstdint>
#include <optional>
#include <type_traits>

namespace WLib::State_Machine::Factory_State_Machine
{
  template <typename St,
            typename Ev,
            typename = std::enable_if_t<std::is_enum_v<St> && std::is_enum_v<Ev>>>
  class Transition
  {
    enum class Type
    {
      empty,
      to_target,
      source_to_target,
    };

    Type const m_type   = Type::empty;
    St const   m_source = St();
    Ev const   m_event  = Ev();
    St const   m_target = St();

  public:
    constexpr Transition() noexcept = default;
    constexpr Transition(Ev const& event, St const& target) noexcept
        : m_type(Type::to_target)
        , m_event(event)
        , m_target(target)
    {
    }
    constexpr Transition(St const& source, Ev const& event, St const& target) noexcept
        : m_type(Type::source_to_target)
        , m_source(source)
        , m_event(event)
        , m_target(target)
    {
    }
    constexpr Transition(Transition const&) = default;

    constexpr std::optional<St> operator()(St const& current_state, Ev const& event) const noexcept
    {
      if (this->m_type == Type::empty)
        return {};

      if (event != this->m_event)
        return {};

      if (this->m_type == Type::to_target)
      {
        if (current_state == this->m_target)
          return {};
        return this->m_target;
      }

      if (this->m_type == Type::source_to_target)
      {
        if (current_state != this->m_source)
          return {};
        return this->m_target;
      }

      return {};
    }
  };
}    // namespace WLib::State_Machine::Factory_State_Machine