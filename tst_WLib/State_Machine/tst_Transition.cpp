#include "tst_State_Machine_Base.h"

#include <State_Machine/Transition.h>
#include <catch.hpp>


TEST_CASE("tst_transition")
{
  namespace N        = WLib::State_Machine::Factory_State_Machine;
  using transition_t = N::Transition<States, Events>;

  SECTION("Empty")
  {
    transition_t t;
    REQUIRE_FALSE(t(States::A, Events::a).has_value());
    REQUIRE_FALSE(t(States::A, Events::b).has_value());
    REQUIRE_FALSE(t(States::A, Events::c).has_value());

    REQUIRE_FALSE(t(States::B, Events::a).has_value());
    REQUIRE_FALSE(t(States::B, Events::b).has_value());
    REQUIRE_FALSE(t(States::B, Events::c).has_value());

    REQUIRE_FALSE(t(States::C, Events::a).has_value());
    REQUIRE_FALSE(t(States::C, Events::b).has_value());
    REQUIRE_FALSE(t(States::C, Events::c).has_value());
  }

  SECTION("from source to target")
  {
    transition_t t(States::A, Events::a, States::B);
    REQUIRE(t(States::A, Events::a).has_value());
    REQUIRE(t(States::A, Events::a).value() == States::B);
    REQUIRE_FALSE(t(States::A, Events::b).has_value());
    REQUIRE_FALSE(t(States::A, Events::c).has_value());

    REQUIRE_FALSE(t(States::B, Events::a).has_value());
    REQUIRE_FALSE(t(States::B, Events::b).has_value());
    REQUIRE_FALSE(t(States::B, Events::c).has_value());

    REQUIRE_FALSE(t(States::C, Events::a).has_value());
    REQUIRE_FALSE(t(States::C, Events::b).has_value());
    REQUIRE_FALSE(t(States::C, Events::c).has_value());
  }

  SECTION("to target")
  {
    transition_t t(Events::c, States::C);
    REQUIRE_FALSE(t(States::A, Events::a).has_value());
    REQUIRE_FALSE(t(States::A, Events::b).has_value());
    REQUIRE(t(States::A, Events::c).has_value());
    REQUIRE(t(States::A, Events::c).value() == States::C);

    REQUIRE_FALSE(t(States::B, Events::a).has_value());
    REQUIRE_FALSE(t(States::B, Events::b).has_value());
    REQUIRE(t(States::B, Events::c).has_value());
    REQUIRE(t(States::A, Events::c).value() == States::C);

    REQUIRE_FALSE(t(States::C, Events::a).has_value());
    REQUIRE_FALSE(t(States::C, Events::b).has_value());
    REQUIRE_FALSE(t(States::C, Events::c).has_value());
  }
}