#pragma once
#include <new>
#include <type_traits>
#include <utility>

namespace cxl {

namespace lambda {

inline namespace detail {

template<typename Return, typename... Parameters>
struct Iwrap_lambda
{
  virtual ~Iwrap_lambda() {}
  virtual Return operator()(Parameters&&...) = 0;
  virtual Return operator()(Parameters&&...) const = 0;
};

template<typename Lambda, typename Return, typename... Parameters>
struct wrap_lambda final : public Iwrap_lambda<Return, Parameters...>
{
  wrap_lambda(Lambda lambda)
    : lambda_(std::move(lambda))
  {}

  Return operator()(Parameters&&... arguments) override { return lambda_(std::forward<Parameters>(arguments)...); }

  Return operator()(Parameters&&... arguments) const override
  {
    return lambda_(std::forward<Parameters>(arguments)...);
  }

private:
  Lambda lambda_;
};
}

template<typename CallSign>
struct wrap;

template<typename Return, typename... Parameters>
struct wrap<Return(Parameters...)>
{
  template<typename Lambda>
  wrap(Lambda&& lambda)
  {
    new (access()) wrap_lambda<Lambda, Return, Parameters...>(std::forward<Lambda>(lambda));
  }

  ~wrap() { access()->~Iwrap_lambda<Return, Parameters...>(); }

  Return operator()(Parameters&&... arguments) { return access()->operator()(std::forward<Parameters>(arguments)...); }

  Return operator()(Parameters&&... arguments) const
  {
    return access()->operator()(std::forward<Parameters>(arguments)...);
  }

private:
  auto access() { return reinterpret_cast<Iwrap_lambda<Return, Parameters...>*>(&wrapper_); }
  auto access() const { return reinterpret_cast<const Iwrap_lambda<Return, Parameters...>*>(&wrapper_); }
  std::aligned_storage_t<8> wrapper_;
};
}
}