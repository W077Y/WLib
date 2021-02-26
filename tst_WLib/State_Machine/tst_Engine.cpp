#include "tst_State_Machine_Base.h"

#include <State_Machine/Engine.h>
#include <catch.hpp>

struct tst_counter
{
  std::size_t entry  = 0;
  std::size_t state  = 0;
  std::size_t exit   = 0;
  std::size_t handle = 0;

  tst_counter() = default;
  tst_counter(std::size_t ent, std::size_t state, std::size_t ex, std::size_t han)
      : entry(ent)
      , state(state)
      , exit(ex)
      , handle(han)
  {
  }

  bool operator==(tst_counter const& val) const
  {
    return this->entry == val.entry && this->state == val.state && this->exit == val.exit &&
           this->handle == val.handle;
  }
};

class tst_state_t: public WLib::State_Machine::Factory_State_Machine::State_Base<States, Events>
{
  tst_counter& m_counter;

public:
  tst_state_t(States const& st, tst_counter& counter)
      : State_Base(st)
      , m_counter(counter)
  {
    this->m_counter.entry++;
  }
  virtual ~tst_state_t() override { this->m_counter.exit++; }

  virtual std::optional<Events> operator()() override
  {
    this->m_counter.state++;

    if (this->m_counter.state > 10)
      return Events::b;
    return {};
  }

  virtual std::optional<Events> handle_event(Events const& event) override
  {
    this->m_counter.handle++;

    if (event == Events::c)
      return {};

    return event;
  }
};

TEST_CASE("tst_engine")
{
  namespace N = WLib::State_Machine::Factory_State_Machine;

  using transition_t = N::Transition<States, Events>;

  tst_counter count_A;
  tst_counter count_B;
  tst_counter count_C;

  class placement_new_factory: public N::State_Factory_Interface<States, Events>
  {
    using mem_t = std::aligned_union_t<0, tst_state_t>;
    tst_counter& count_A;
    tst_counter& count_B;
    tst_counter& count_C;
    mem_t        mem = {};

  public:
    placement_new_factory(tst_counter& count_A, tst_counter& count_B, tst_counter& count_C)
        : count_A(count_A)
        , count_B(count_B)
        , count_C(count_C)
    {
    }

    virtual N::State_Base<States, Events>& create_state(States const& st) override
    {
      switch (st)
      {
        case States::A:
          return *new (&mem) tst_state_t(States::A, count_A);

        case States::B:
          return *new (&mem) tst_state_t(States::B, count_B);

        case States::C:
          return *new (&mem) tst_state_t(States::C, count_C);

        default:
          break;
      }
      throw "Illegal States";
    };
    virtual void destroy_state(N::State_Base<States, Events>& state) override
    {
      state.~State_Base();
    }
  };

  using engine_t = N::Engine<States, Events>;

  constexpr transition_t tab[] = {
    { States::A, Events::a, States::B },
    { States::B, Events::b, States::C },
    { Events::d, States::A },
  };

  placement_new_factory fac(count_A, count_B, count_C);

  {
    engine_t engine(tab, fac, States::A);

    REQUIRE(engine.get_state() == States::A);

    REQUIRE(count_A == tst_counter{ 1, 0, 0, 0 });
    REQUIRE(count_B == tst_counter{ 0, 0, 0, 0 });
    REQUIRE(count_C == tst_counter{ 0, 0, 0, 0 });

    for (std::size_t i = 0; i < 10; i++)
      REQUIRE_FALSE(engine().has_value());

    std::optional<Events> opt_evt = engine();

    REQUIRE(opt_evt.has_value());
    REQUIRE(opt_evt.value() == Events::b);

    REQUIRE(count_A == tst_counter{ 1, 11, 0, 0 });
    REQUIRE(count_B == tst_counter{ 0, 0, 0, 0 });
    REQUIRE(count_C == tst_counter{ 0, 0, 0, 0 });

    REQUIRE_FALSE(engine.handle_event(Events::b));

    REQUIRE(count_A == tst_counter{ 1, 11, 0, 1 });
    REQUIRE(count_B == tst_counter{ 0, 0, 0, 0 });
    REQUIRE(count_C == tst_counter{ 0, 0, 0, 0 });

    REQUIRE(engine.handle_event(Events::c));

    REQUIRE(count_A == tst_counter{ 1, 11, 0, 2 });
    REQUIRE(count_B == tst_counter{ 0, 0, 0, 0 });
    REQUIRE(count_C == tst_counter{ 0, 0, 0, 0 });

    REQUIRE(engine.get_state() == States::A);
    REQUIRE(engine.handle_event(Events::a));
    REQUIRE(engine.get_state() == States::B);

    REQUIRE(count_A == tst_counter{ 1, 11, 1, 3 });
    REQUIRE(count_B == tst_counter{ 1, 0, 0, 0 });
    REQUIRE(count_C == tst_counter{ 0, 0, 0, 0 });

    REQUIRE(engine.get_state() == States::B);
    for (std::size_t i = 0; i < 10; i++)
      REQUIRE_FALSE(engine().has_value());
    REQUIRE(engine.get_state() == States::B);
    REQUIRE_FALSE(engine().has_value());
    REQUIRE(engine.get_state() == States::C);

    REQUIRE(count_A == tst_counter{ 1, 11, 1, 3 });
    REQUIRE(count_B == tst_counter{ 1, 11, 1, 0 });
    REQUIRE(count_C == tst_counter{ 1, 0, 0, 0 });

    engine();
    engine();
    engine();
    REQUIRE(engine.get_state() == States::C);
    engine.handle_event(Events::d);
    REQUIRE(engine.get_state() == States::A);

    REQUIRE(count_A == tst_counter{ 2, 11, 1, 3 });
    REQUIRE(count_B == tst_counter{ 1, 11, 1, 0 });
    REQUIRE(count_C == tst_counter{ 1, 3, 1, 1 });
  }

  REQUIRE(count_A == tst_counter{ 2, 11, 2, 3 });
  REQUIRE(count_B == tst_counter{ 1, 11, 1, 0 });
  REQUIRE(count_C == tst_counter{ 1, 3, 1, 1 });
}
