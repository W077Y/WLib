#include "tst_State_Machine_Base.h"

#include <WLib_State_Machine.h>
#include <catch.hpp>
#include <functional>

class tst_State_A: public WLib::State_Machine::Factory_State_Machine::State_Base<States, Events>
{
  [[maybe_unused]] char member[10] = {};

public:
  tst_State_A()
      : State_Base(States::A)
  {
  }
};

class tst_State_B: public WLib::State_Machine::Factory_State_Machine::State_Base<States, Events>
{
  [[maybe_unused]] char member[20] = {};

public:
  tst_State_B()
      : State_Base(States::B)
  {
  }
};

class tst_State_C: public WLib::State_Machine::Factory_State_Machine::State_Base<States, Events>
{
  [[maybe_unused]] char member[30] = {};

public:
  tst_State_C()
      : State_Base(States::C)
  {
  }
};

class tst_StateMachineClass
{
  using transition_t = WLib::State_Machine::Factory_State_Machine::Transition<States, Events>;

  static constexpr transition_t table[] = {
    { States::A, Events::a, States::B },
    { States::B, Events::b, States::C },
    { Events::c, States::A },
  };

  class factory_t
      : public WLib::State_Machine::Factory_State_Machine::
            Placement_State_Factory<States, Events, tst_State_A, tst_State_B, tst_State_C>
  {
  public:
    factory_t() = default;

    virtual WLib::State_Machine::Factory_State_Machine::State_Base<States, Events>&
    create_state(States const& st) override
    {
      switch (st)
      {
        case States::A:
          return *new (&this->m_mem) tst_State_A();

        case States::B:
          return *new (&this->m_mem) tst_State_B();

        case States::C:
          return *new (&this->m_mem) tst_State_C();

        default:
          break;
      }
      throw "Illegal States";
    }
  };
  using engine_t = WLib::State_Machine::Factory_State_Machine::Engine<States, Events>;

  factory_t m_fac;
  engine_t  m_engine = engine_t(table, this->m_fac, States::A);

public:
  States get_state() { return this->m_engine.get_state(); }
  void   tick() { this->m_engine(); }
  void   handle_event(Events const& event) { this->m_engine.handle_event(event); }
};

TEST_CASE("tst_StateMachineClass")
{
  tst_StateMachineClass tmp;

  REQUIRE(tmp.get_state() == States::A);

  tmp.tick();
  tmp.handle_event(Events::b);
  REQUIRE(tmp.get_state() == States::A);

  tmp.handle_event(Events::a);
  REQUIRE(tmp.get_state() == States::B);

  tmp.handle_event(Events::b);
  REQUIRE(tmp.get_state() == States::C);

  tmp.handle_event(Events::c);
  REQUIRE(tmp.get_state() == States::A);
}