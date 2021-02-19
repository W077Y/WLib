#pragma once

#include <State_Machine/State_Base.h>
#include <State_Machine/Transition.h>
#include <cstdint>
#include <optional>

namespace WLib::State_Machine::Factory_State_Machine
{
  template <typename St,
            typename Ev,
            typename = std::enable_if_t<std::is_enum_v<St> && std::is_enum_v<Ev>>>
  class State_Factory_Interface
  {
  public:
    virtual State_Base<St, Ev>& create_state(const St&)            = 0;
    virtual void                destroy_state(State_Base<St, Ev>&) = 0;
  };

  template <typename St, typename Ev, typename... T>
  class Placement_State_Factory: public State_Factory_Interface<St, Ev>
  {
  protected:
    std::aligned_union_t<0, T...> m_mem = {};

  public:
    virtual State_Base<St, Ev>& create_state(const St&) = 0;
    virtual void destroy_state(State_Base<St, Ev>& state) override { state.~State_Base(); };
  };

  template <typename St,
            typename Ev,
            typename = std::enable_if_t<std::is_enum_v<St> && std::is_enum_v<Ev>>>
  class Engine
  {
    State_Base<St, Ev>*              m_cur = nullptr;
    Transition<St, Ev> const*        m_tab = nullptr;
    std::size_t                      m_len = 0;
    State_Factory_Interface<St, Ev>& m_fac;

    bool search_in_table(Ev const& event)
    {
      for (std::size_t i = 0; i < this->m_len; i++)
      {
        std::optional<St> next = this->m_tab[i](this->m_cur->get_state(), event);
        if (next.has_value())
        {
          this->m_fac.destroy_state(*this->m_cur);
          this->m_cur = &this->m_fac.create_state(next.value());
          return true;
        }
      }
      return false;
    }

  public:
    constexpr Engine(Transition<St, Ev> const*        table,
                     std::size_t const&               table_length,
                     State_Factory_Interface<St, Ev>& state_factory,
                     St const&                        inital_state) noexcept
        : m_tab(table)
        , m_len(table_length)
        , m_fac(state_factory)
    {
      this->m_cur = &this->m_fac.create_state(inital_state);
    }

    template <std::size_t N>
    constexpr Engine(Transition<St, Ev> const (&tab)[N],
                     State_Factory_Interface<St, Ev>& factory,
                     St const&                        inital_state) noexcept
        : Engine(tab, N, factory, inital_state)
    {
    }

    virtual ~Engine() { this->m_fac.destroy_state(*this->m_cur); }

    std::optional<Ev> tick()
    {
      std::optional<Ev> opt_evt = (*this->m_cur)();

      if (!opt_evt.has_value())
        return {};

      if (this->search_in_table(opt_evt.value()))
        return {};

      return opt_evt;
    }

    bool handle_event(Ev const& event)
    {
      if (this->search_in_table(event))
        return true;

      return this->m_cur->handle_event(event);
    }

    St get_state() const noexcept { return this->m_cur->get_state(); }
  };
}    // namespace WLib::State_Machine::Factory_State_Machine
