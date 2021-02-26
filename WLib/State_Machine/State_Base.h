#pragma once

#include <cstdint>
#include <optional>
#include <type_traits>

namespace WLib::State_Machine::Factory_State_Machine
{

  template <typename St,
            typename Ev,
            typename = std::enable_if_t<std::is_enum_v<St> && std::is_enum_v<Ev>>>
  class State_Base
  {
  protected:
    St m_state;

  public:
    constexpr State_Base(St const& state) noexcept
        : m_state(state)
    {
    }
    virtual ~State_Base() = default;

    virtual std::optional<Ev> operator()() { return {}; };
    virtual std::optional<Ev> handle_event(Ev const& evt) { return evt; };
    virtual St                get_state() const noexcept { return this->m_state; }
                              operator St() const noexcept { return this->get_state(); }
  };

}    // namespace WLib::State_Machine::Factory_State_Machine
