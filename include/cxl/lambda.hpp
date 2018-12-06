#pragma once
#include <functional>
#include <new>
#include <type_traits>
#include <utility>

namespace cxl {

namespace lambda {

namespace error {
struct uninitialized_lambda_call
{};
}
inline namespace detail {

template<typename Return, typename... Parameters>
struct Iwrap_lambda
{
  virtual ~Iwrap_lambda() {}
  virtual Return operator()(Parameters...) = 0;
  virtual Return operator()(Parameters...) const = 0;
};

template<typename Lambda, typename Return, typename... Parameters>
struct wrap_lambda final : public Iwrap_lambda<Return, Parameters...>
{
  wrap_lambda(Lambda lambda)
    : lambda_(std::move(lambda))
  {}

  Return operator()(Parameters... arguments) override { return lambda_(arguments...); }

  Return operator()(Parameters... arguments) const override { return lambda_(arguments...); }

private:
  Lambda lambda_;
};
}

struct ctor
{};
struct dtor
{};

template<typename CallSign>
struct wrap;

/** wrap< Return ( Parameters... ) >
 * this class wraps/holds a lambda whose return type is {Return} and parameters are {Parameters...}
 */
template<typename Return, typename... Parameters>
struct wrap<Return(Parameters...)>
{
  wrap()
    : empty_(true)
  {}

  template<typename Lambda>
  wrap(Lambda&& lambda)
  {
    new (access()) wrap_lambda<Lambda, Return, Parameters...>(std::forward<Lambda>(lambda));
    empty_ = false;
  }

  virtual ~wrap()
  {
    if (!empty_)
      access()->~Iwrap_lambda<Return, Parameters...>();
  }

  template<typename... UParameters>
  Return operator()(UParameters&&... arguments)
  {
    if (empty_)
      throw error::uninitialized_lambda_call{};
    return access()->operator()(std::forward<Parameters>(arguments)...);
  }

  template<typename... UParameters>
  Return operator()(UParameters&&... arguments) const
  {
    if (empty_)
      throw error::uninitialized_lambda_call{};
    return access()->operator()(std::forward<Parameters>(arguments)...);
  }

private:
  auto access() { return reinterpret_cast<Iwrap_lambda<Return, Parameters...>*>(&wrapper_); }
  auto access() const { return reinterpret_cast<const Iwrap_lambda<Return, Parameters...>*>(&wrapper_); }
  std::aligned_storage_t<16> wrapper_;
  bool empty_;
};

/** wrap< ctor > wraps a void-return lambda with no parameters
 * the wrapped lambda is not arbitrarily callable
 * the wrapped lambda is called once upon construction
 */
template<>
struct wrap<ctor> final : private wrap<void()>
{
  using wrap<void()>::wrap;
  template<typename Lambda>
  wrap(Lambda&& lambda)
    : wrap<void()>(std::forward<Lambda>(lambda))
  {
    std::invoke(static_cast<wrap<void()>&>(*this));
  }
};

/** wrap< dtor > wraps a void-return lambda with no parameters
 * the wrapped lambda is not arbitrarily callable
 * the wrapped lambda is called once upon destruction
 */
template<>
struct wrap<dtor> final : private wrap<void()>
{
  using wrap<void()>::wrap;
  ~wrap() { std::invoke(static_cast<wrap<void()>&>(*this)); }
};
}
}