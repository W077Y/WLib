

#include <WLib_State_Machine.h>
#include <chrono>
#include <optional>
#include <cmath>

namespace Interface
{
  class MeasModule_Interface
  {
  public:
    enum class States
    {
      initializing,
      ready,
      measuring,
      finished,
      aborted,
      aborting,
      error,
    };

    enum class Errors
    {
      value_out_of_range,
      measurement_timeout,
    };

    virtual ~MeasModule_Interface() = default;

    virtual bool                  start_measurement(float const& target_temperature) = 0;
    virtual bool                  abort()                                            = 0;
    virtual bool                  finish()                                           = 0;
    virtual bool                  quit()                                             = 0;
    virtual States                get_state()                                        = 0;
    virtual std::optional<Errors> get_error()                                        = 0;

    virtual float get_value() = 0;
  };
}    // namespace Interface

namespace Implementation
{
  enum class Events
  {
    start_measurement,
    next,
    abort,
    quit,
    error,
  };

  // alias
  using meas_base_state = WLib::State_Machine::Factory_State_Machine::
      State_Base<Interface::MeasModule_Interface::States, Events>;

  class initializing_state final: public meas_base_state
  {
  public:
    initializing_state(std::size_t init_count);

    virtual ~initializing_state();

    virtual std::optional<Events> operator()() noexcept override;

  private:
    std::size_t m_count;
  };

  class ready_state final: public meas_base_state
  {
  public:
    ready_state(std::optional<Interface::MeasModule_Interface::Errors>& error,
                float&                                                  measurement_result);

    virtual ~ready_state();

    virtual std::optional<Events> operator()() noexcept override;

  private:
  };

  class measurement_state final: public meas_base_state
  {
  public:
    measurement_state(float const&                                            meas_temperature,
                      std::size_t const&                                      meas_count,
                      std::optional<Interface::MeasModule_Interface::Errors>& error,
                      float&                                                  measurement_result,
                      float (&meas_func)());

    virtual ~measurement_state();

    virtual std::optional<Events> operator()() noexcept override;
    virtual std::optional<Events> handle_event(Events const& evt) noexcept override;

  private:
    std::optional<Interface::MeasModule_Interface::Errors>& m_error;
    float&                                                  m_result;
    float (&m_meas_fuc)();
    std::size_t m_count;
  };

  class finished_state final: public meas_base_state
  {
  public:
    finished_state();

    virtual ~finished_state();

    virtual std::optional<Events> operator()() noexcept override;

  private:
  };

  class aborted_state final: public meas_base_state
  {
  public:
    aborted_state();

    virtual ~aborted_state();

    virtual std::optional<Events> operator()() noexcept override;

  private:
  };

  class error_state final: public meas_base_state
  {
  public:
    error_state(std::optional<Interface::MeasModule_Interface::Errors> const& errors);

    virtual ~error_state();

    virtual std::optional<Events> operator()() noexcept override;

  private:
    std::optional<Interface::MeasModule_Interface::Errors> const& m_errors;
  };

  class MeasurementModule final: Interface::MeasModule_Interface
  {
    using St         = Interface::MeasModule_Interface::States;
    using Ev         = Events;
    using transition = WLib::State_Machine::Factory_State_Machine::Transition<St, Ev>;

    static constexpr transition m_table[] = {
      { St::initializing, Ev::next, St::ready },
      { St::ready, Ev::start_measurement, St::measuring },
      { St::measuring, Ev::next, St::finished },
      { St::aborting, Ev::abort, St::aborted },
      { St::finished, Ev::next, St::ready },
      { St::aborted, Ev::next, St::ready },
      { Ev::error, St::error },
      { St::error, Ev::quit, St::ready },
    };

  public:
    MeasurementModule(float (&meas_fuc)());

    virtual bool start_measurement(float const& target_temperature) override;

    virtual bool abort() override;

    virtual bool finish() override;

    virtual bool quit() override;

    virtual States                get_state() override;
    virtual std::optional<Errors> get_error() override;

    virtual float get_value() override;

    void tick();

  private:
    class factory final
        : public WLib::State_Machine::Factory_State_Machine::Placement_State_Factory<
              Interface::MeasModule_Interface::States,
              Events,
              initializing_state,
              ready_state,
              measurement_state,
              aborted_state,
              finished_state,
              error_state>
    {
    public:
      factory(float&                                                  target_temperature,
              std::optional<Interface::MeasModule_Interface::Errors>& errors,
              float&                                                  measurement_result,
              float (&meas_fuc)());

      virtual meas_base_state&
      create_state(const Interface::MeasModule_Interface::States& st) override;

    private:
      float&                                                  m_target_temperature;
      std::optional<Interface::MeasModule_Interface::Errors>& m_errors;
      float&                                                  m_measurement_result;
      float (&m_meas_fuc)();
    };

    float m_target_temperature = std::numeric_limits<float>::quiet_NaN();
    std::optional<Interface::MeasModule_Interface::Errors> m_errors = {};
    float m_measurement_result = std::numeric_limits<float>::quiet_NaN();

    factory                                                            m_fac;
    WLib::State_Machine::Factory_State_Machine::Engine<States, Events> m_engine;
  };
}    // namespace Implementation

float global_meas_value = std::numeric_limits<float>::quiet_NaN();

float meas_value_dummy() { return global_meas_value; }

int main()
{
  using St = Interface::MeasModule_Interface::States;
  using Er = Interface::MeasModule_Interface::Errors;

  Implementation::MeasurementModule module(meas_value_dummy);

  if (module.get_state() != St::initializing)
    return -1;

  module.tick();
  module.tick();
  module.tick();
  module.tick();
  module.tick();

  if (module.get_state() != St::ready)
    return -2;

  if (!module.start_measurement(27.5f))
    return -3;

  if (module.get_state() != St::measuring)
    return -4;

  module.tick();
  module.tick();
  module.tick();
  module.tick();
  module.tick();
  module.tick();
  module.tick();

  if (module.get_state() != St::error)
    return -5;

  if (!module.get_error().has_value())
    return -6;

  if (module.get_error().value() != Er::measurement_timeout)
    return -7;

  if (!module.quit())
    return -8;

  if (module.get_state() != St::ready)
    return -9;

  if (module.quit())
    return -10;

  module.tick();
  module.tick();

  if (!module.start_measurement(30.5f))
    return -11;

  module.tick();
  module.tick();

  global_meas_value = std::numeric_limits<float>::infinity();

  module.tick();

  if (module.get_state() != St::error)
    return -12;

  if (module.get_error().value() != Er::value_out_of_range)
    return -13;

  module.tick();
  module.tick();

  if (!module.quit())
    return -14;

  if (module.get_state() != St::ready)
    return -15;

  global_meas_value = std::numeric_limits<float>::quiet_NaN();

  if (!module.start_measurement(15.0f))
    return -16;

  module.tick();
  module.tick();

  if (!module.abort())
    return -17;

  module.tick();
  module.tick();

  if (!module.finish())
    return -18;

  module.tick();
  module.tick();

  if (!module.start_measurement(10.0f))
    return -19;

  module.tick();
  module.tick();

  global_meas_value = 3.14f;

  module.tick();
  module.tick();

  if (module.get_state() != St::finished)
    return -20;

  if (module.get_value() != 3.14f)
    return -21;

  if (!module.finish())
    return -22;

  if (module.get_state() != St::ready)
    return -23;

  if (!std::isnan(module.get_value()))
    return -24;

  return 0;
}

Implementation::initializing_state::initializing_state(std::size_t init_count)
    : meas_base_state(Interface::MeasModule_Interface::States::initializing)
    , m_count(init_count)
{
  printf("enter init state\n");
}

Implementation::initializing_state::~initializing_state() { printf("exiting init state\n"); }

std::optional<Implementation::Events> Implementation::initializing_state::operator()() noexcept
{
  printf("  init ...\n");
  if (--this->m_count)
    return {};
  return Events::next;
}

Implementation::ready_state::ready_state(
    std::optional<Interface::MeasModule_Interface::Errors>& error,
    float&                                                  measurement_result)
    : meas_base_state(Interface::MeasModule_Interface::States::ready)
{
  printf("enter ready state\n");
  error              = {};
  measurement_result = std::numeric_limits<float>::quiet_NaN();
}
Implementation::ready_state::~ready_state() { printf("exiting ready state\n"); }

std::optional<Implementation::Events> Implementation::ready_state::operator()() noexcept
{
  printf("  ready ...\n");
  return {};
}

Implementation::measurement_state::measurement_state(
    float const&                                            meas_temperature,
    std::size_t const&                                      meas_count,
    std::optional<Interface::MeasModule_Interface::Errors>& error,
    float&                                                  measurement_result,
    float (&meas_func)())
    : meas_base_state(Interface::MeasModule_Interface::States::measuring)
    , m_error(error)
    , m_result(measurement_result)
    , m_meas_fuc(meas_func)
    , m_count(meas_count)
{
  printf("enter measure state for a measurment at %f degC\n", meas_temperature);
}

Implementation::measurement_state::~measurement_state() { printf("exiting measure state\n"); }

std::optional<Implementation::Events> Implementation::measurement_state::operator()() noexcept
{
  if (this->m_state == Interface::MeasModule_Interface::States::aborting)
  {
    printf("  abort ...\n");
    return Events::abort;
  }

  printf("  measure ...\n");

  float meas_value = this->m_meas_fuc();

  if (std::isfinite(meas_value))
  {
    this->m_result = meas_value;
    return Events::next;
  }

  if (std::isinf(meas_value))
  {
    this->m_error = Interface::MeasModule_Interface::Errors::value_out_of_range;
    return Events::error;
  }

  if (--this->m_count)
    return {};

  this->m_error = Interface::MeasModule_Interface::Errors::measurement_timeout;
  return Events::error;
}

std::optional<Implementation::Events>
Implementation::measurement_state::handle_event(Events const& evt) noexcept
{
  if (evt == Events::abort)
  {
    this->m_state = Interface::MeasModule_Interface::States::aborting;
    return {};
  }

  return evt;
}

Implementation::finished_state::finished_state()
    : meas_base_state(Interface::MeasModule_Interface::States::finished)
{
  printf("enter finished state\n");
}
Implementation::finished_state::~finished_state() { printf("exiting finished state\n"); }

std::optional<Implementation::Events> Implementation::finished_state::operator()() noexcept
{
  printf("  finished ...\n");
  return {};
}

Implementation::aborted_state::aborted_state()
    : meas_base_state(Interface::MeasModule_Interface::States::finished)
{
  printf("enter aborted state\n");
}

Implementation::aborted_state::~aborted_state() { printf("exiting aborted state\n"); }

std::optional<Implementation::Events> Implementation::aborted_state::operator()() noexcept
{
  printf("  aborted ...\n");
  return {};
}

Implementation::error_state::error_state(
    std::optional<Interface::MeasModule_Interface::Errors> const& errors)
    : meas_base_state(Interface::MeasModule_Interface::States::error)
    , m_errors(errors)
{
  printf("enter error state\n");
}

Implementation::error_state::~error_state() { printf("exiting error state\n"); }

std::optional<Implementation::Events> Implementation::error_state::operator()() noexcept
{
  if (!this->m_errors.has_value())
  {
    printf("  error unknown ...\n");
    return {};
  }

  switch (this->m_errors.value())
  {
    case Interface::MeasModule_Interface::Errors::measurement_timeout:
      printf("  error timeout ...\n");
      break;

    case Interface::MeasModule_Interface::Errors::value_out_of_range:
      printf("  error value_out_or_range ...\n");
      break;

    default:
      printf("  error unknown ...\n");
      break;
  }
  return {};
}

Implementation::MeasurementModule::MeasurementModule(float (&meas_fuc)())
    : m_fac(this->m_target_temperature, this->m_errors, this->m_measurement_result, meas_fuc)
    , m_engine(this->m_table, this->m_fac, States::initializing)
{
}

bool Implementation::MeasurementModule::start_measurement(float const& target_temperature)
{
  if (this->m_engine.get_state() != States::ready)
    return false;

  this->m_target_temperature = target_temperature;
  return this->m_engine.handle_event(Events::start_measurement).has_value() == false;
}

bool Implementation::MeasurementModule::abort()
{
  return this->m_engine.handle_event(Events::abort).has_value() == false;
}

bool Implementation::MeasurementModule::finish()
{
  St st = this->m_engine.get_state();

  if (st != St::finished && st != St::aborted)
    return false;

  return this->m_engine.handle_event(Events::next).has_value() == false;
}

bool Implementation::MeasurementModule::quit()
{
  return this->m_engine.handle_event(Events::quit).has_value() == false;
}

Interface::MeasModule_Interface::States Implementation::MeasurementModule::get_state()
{
  return this->m_engine.get_state();
}

std::optional<Interface::MeasModule_Interface::Errors>
Implementation::MeasurementModule::get_error()
{
  return this->m_errors;
}

inline float Implementation::MeasurementModule::get_value() { return this->m_measurement_result; }

inline void Implementation::MeasurementModule::tick()
{
  auto opt_evt = this->m_engine();
  if (opt_evt.has_value())
    printf("pubbeling event %u", static_cast<uint32_t>(opt_evt.value()));
}

Implementation::MeasurementModule::factory::factory(
    float&                                                  target_temperature,
    std::optional<Interface::MeasModule_Interface::Errors>& errors,
    float&                                                  measurement_result,
    float (&meas_fuc)())
    : m_target_temperature(target_temperature)
    , m_errors(errors)
    , m_measurement_result(measurement_result)
    , m_meas_fuc(meas_fuc)
{
}

Implementation::meas_base_state& Implementation::MeasurementModule::factory::create_state(
    const Interface::MeasModule_Interface::States& st)
{
  using ST = Interface::MeasModule_Interface::States;
  switch (st)
  {
    case ST::initializing:
      return *new (&this->m_mem) initializing_state(5);

    case ST::ready:
      return *new (&this->m_mem) ready_state(this->m_errors, this->m_measurement_result);

    case ST::measuring:
      return *new (&this->m_mem) measurement_state(this->m_target_temperature, 5, this->m_errors,
                                                   this->m_measurement_result, this->m_meas_fuc);

    case ST::aborted:
      return *new (&this->m_mem) aborted_state();

    case ST::finished:
      return *new (&this->m_mem) finished_state();

    default:
      return *new (&this->m_mem) error_state(this->m_errors);
  }
}
