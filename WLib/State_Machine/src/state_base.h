#pragma once

#include <cstdint>
#include <optional>
#include <type_traits>

namespace WLib::State_Machine::Factory_State_Machine
{
  template <typename St,
            typename Ev,
            typename = std::enable_if_t<std::is_enum_v<St> && std::is_enum_v<Ev>>>
  class State_Interface
  {
  protected:
  public:
    virtual ~State_Interface() noexcept = default;

    virtual std::optional<Ev> operator()() noexcept                = 0;
    virtual std::optional<Ev> handle_event(Ev const& evt) noexcept = 0;
    virtual St                get_state() const noexcept           = 0;
  };

  template <typename St, typename Ev> class State_Base: public State_Interface<St, Ev>
  {
  protected:
    St m_state;

  public:
    constexpr State_Base(St const& state) noexcept
        : m_state(state)
    {
    }
    virtual ~State_Base() noexcept = default;

    virtual std::optional<Ev> operator()() noexcept override { return {}; };
    virtual std::optional<Ev> handle_event(Ev const& evt) noexcept override { return evt; };
    virtual St                get_state() const noexcept override { return this->m_state; }
                              operator St() const noexcept { return this->get_state(); }
  };

}    // namespace WLib::State_Machine::Factory_State_Machine
