#include <ObserverPattern.h>
#include <array>
#include <atomic>
#include <catch.hpp>
#include <condition_variable>
#include <mutex>
#include <optional>

template <typename T> class Observer_Interface;
template <typename T> class Observable_Base;

template <typename T> class Observable_Interface
{
public:
  virtual void subscribe(Observer_Interface<T>& obj)   = 0;
  virtual void unsubscribe(Observer_Interface<T>& obj) = 0;
};

template <typename T> class Observer_Interface
{
  friend Observable_Base<T>;
  Observer_Interface<T>*   m_next = nullptr;
  Observable_Interface<T>& m_observable;

  Observer_Interface()                          = delete;
  Observer_Interface(Observer_Interface const&) = delete;
  Observer_Interface(Observer_Interface&&)      = delete;
  Observer_Interface& operator=(Observer_Interface const&) = delete;
  Observer_Interface& operator=(Observer_Interface&&) = delete;

protected:
  Observer_Interface(Observable_Interface<T>& observable)
      : m_observable(observable)
  {
    this->m_observable.subscribe(*this);
  }

public:
  virtual ~Observer_Interface() { this->m_observable.unsubscribe(*this); }

  virtual void push_value(T const& val) = 0;
};

template <typename T, std::size_t N> class FIFO_Observer: protected Observer_Interface<T>
{
  std::array<T, N + 1>            m_data   = {};
  std::size_t                     m_wr_idx = 0;
  std::size_t                     m_rd_idx = 0;
  mutable std::mutex              m_tex;
  mutable std::condition_variable m_con_var;

  constexpr std::size_t next_idx(std::size_t idx) noexcept
  {
    idx++;
    if (idx == N + 1)
      return 0;
    return idx;
  }

  virtual void push_value(T const& val) override
  {

    this->m_data[this->m_wr_idx] = val;

    {
      std::lock_guard l(this->m_tex);
      this->m_wr_idx = this->next_idx(this->m_wr_idx);
      if (this->m_wr_idx == this->m_rd_idx)
        this->m_rd_idx = this->next_idx(this->m_rd_idx);
    }
    this->m_con_var.notify_all();
  }

public:
  FIFO_Observer(Observable_Interface<T>& observable)
      : Observer_Interface<T>(observable)
  {
  }

  T get_value()
  {
    std::unique_lock<std::mutex> l(this->m_tex);

    while (this->m_wr_idx == this->m_rd_idx)
      this->m_con_var.wait(l);

    std::size_t const idx = this->m_rd_idx;
    this->m_rd_idx        = this->next_idx(this->m_rd_idx);

    return this->m_data[idx];
  }
};

template <typename T> class Observable_Base: protected Observable_Interface<T>
{
protected:
  mutable std::mutex     m_tex;
  Observer_Interface<T>* m_head = nullptr;

  virtual void subscribe(Observer_Interface<T>& obj) override
  {
    std::lock_guard         l(this->m_tex);
    Observer_Interface<T>** cur = &this->m_head;
    while (*cur != nullptr)
    {
      if (*cur == &obj)
        return;

      cur = &((*cur)->m_next);
    }

    *cur = &obj;
  }

  virtual void unsubscribe(Observer_Interface<T>& obj) override
  {
    std::lock_guard         l(this->m_tex);
    Observer_Interface<T>** cur = &this->m_head;
    while (*cur != nullptr)
    {
      if (*cur == &obj)
      {
        *cur = obj.m_next;
        return;
      }
      cur = &((*cur)->m_next);
    }
  }

  void p_publish(T const& value)
  {
    Observer_Interface<T>* cur = this->m_head;
    while (cur != nullptr)
    {
      cur->push_value(value);
      cur = cur->m_next;
    }
  }

public:
  ~Observable_Base() = default;

public:
  virtual bool push_value(T const& value) noexcept = 0;

  template <std::size_t N> FIFO_Observer<T, N> get_FIFO_observer()
  {
    return FIFO_Observer<T, N>(*this);
  }
};

template <typename T, std::size_t N> class IRQ_Observable: public Observable_Base<T>
{
  std::array<T, N + 1>     m_data   = {};
  std::atomic<std::size_t> m_wr_idx = 0;
  std::atomic<std::size_t> m_rd_idx = 0;

  std::atomic<bool>       m_run = true;
  std::thread             m_publisher;
  std::condition_variable m_con_var;

  constexpr std::size_t next_idx(std::size_t idx) noexcept
  {
    if (++idx == N + 1)
      return 0;
    return idx;
  }

  void loop() noexcept
  {
    std::unique_lock l(this->m_tex);

    while (this->m_run)
    {
      std::size_t const cur_wr = this->m_wr_idx;
      std::size_t const cur_rd = this->m_rd_idx;

      if (cur_wr == cur_rd)
      {
        this->m_con_var.wait(l);
        continue;
      }

      std::size_t const next_rd = this->next_idx(cur_rd);
      T                 tmp     = this->m_data[cur_rd];
      this->m_rd_idx            = next_rd;
      this->p_publish(tmp);
    }
  }

public:
  IRQ_Observable()
  {
    this->m_publisher = std::thread([this]() { this->loop(); });
  }
  ~IRQ_Observable()
  {
    this->m_run = false;
    this->m_con_var.notify_all();
    this->m_publisher.join();
  }

  virtual bool push_value(const T& value) noexcept override
  {
    std::size_t const cur_wr = this->m_wr_idx;
    std::size_t const cur_rd = this->m_rd_idx;

    std::size_t const next_wr = this->next_idx(cur_wr);

    if (next_wr == cur_rd)
      return false;

    this->m_data[cur_wr] = value;
    this->m_wr_idx       = next_wr;

    this->m_con_var.notify_all();
    return true;
  }
};

TEST_CASE("tst_ObserverPattern")
{
  IRQ_Observable<float, 3> observable_obj;
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  auto ob_1 = observable_obj.get_FIFO_observer<3>();
  auto ob_2 = observable_obj.get_FIFO_observer<4>();

  observable_obj.push_value(3);
  observable_obj.push_value(5);
  observable_obj.push_value(6);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));
  observable_obj.push_value(7);
  std::this_thread::sleep_for(std::chrono::milliseconds(20));

  REQUIRE(ob_1.get_value() == 5);
  REQUIRE(ob_1.get_value() == 6);
  REQUIRE(ob_1.get_value() == 7);

  observable_obj.push_value(8);
  REQUIRE(ob_1.get_value() == 8);
  REQUIRE(ob_2.get_value() == 5);
  REQUIRE(ob_2.get_value() == 6);
  REQUIRE(ob_2.get_value() == 7);
  REQUIRE(ob_2.get_value() == 8);
}
