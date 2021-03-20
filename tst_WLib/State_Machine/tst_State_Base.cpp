#include "tst_State_Machine_Base.h"

#include <State_Machine/state_base.h>

#include <catch.hpp>


TEST_CASE("State_Base")
{
  WLib::State_Machine::Factory_State_Machine::State_Base<States, Events> state_A(States::A);
  WLib::State_Machine::Factory_State_Machine::State_Base<States, Events> state_B(States::B);

  REQUIRE(state_A.get_state() == States::A);
  REQUIRE(state_B.get_state() == States::B);

  REQUIRE(state_A.handle_event(Events::a).value() == Events::a);
  REQUIRE(state_A.handle_event(Events::b).value() == Events::b);
  REQUIRE(state_A.handle_event(Events::c).value() == Events::c);

  REQUIRE_FALSE(state_A().has_value());
}


