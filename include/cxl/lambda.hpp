#pragma once
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

template<typename CallSign>
struct wrap;

template<typename Return, typename... Parameters>
struct wrap<Return(Parameters...)>
{
  wrap()
    : empty_(true)
  {}

  template<typename Lambda>
  wrap(Lambda&& lambda)
  {
    auto ptr = access();
    new (ptr) wrap_lambda<Lambda, Return, Parameters...>(std::forward<Lambda>(lambda));
    empty_ = false;
  }

  ~wrap()
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
  std::aligned_storage_t<8> wrapper_;
  bool empty_;
};

template<typename Lambda>
constexpr decltype(auto)
ctor(Lambda&& lambda)
{
  lambda();
  return (lambda);
}

template<typename Lambda>
constexpr auto&&
dtor(Lambda&& lambda)
{
  struct local_t : Lambda
  {
    local_t(Lambda&& _)
      : Lambda(_)
    {}
    using Lambda::operator();
    ~local_t() { (*this)(); }
  } local(std::forward<Lambda>(lambda));
  return local;
}
}
}