#include <cxl/aggregate.hpp>
#include <cxl/lambda.hpp>
#include <iostream>

struct statistic
{
  int sum, data_points;
};

struct component
{
  statistic value;
  cxl::lambda::wrap<int(int)> method;
};

struct managed_component
{
  statistic value;
  cxl::lambda::wrap<int(int)> method;
  cxl::lambda::wrap<void()> constructor;
  cxl::lambda::wrap<void()> destructor;
};

int
main()
{
  component accumulator{ statistic{ 0, 0 }, [&](auto input) {
                          accumulator.value.sum += input;
                          return accumulator.value.sum;
                        } };

  component average{ statistic{ 0, 0 }, [&](auto input) {
                      auto& stat = average.value;
                      stat.sum += input;
                      stat.data_points += 1;
                      return stat.sum / stat.data_points;
                    } };

  managed_component config{ statistic{ 0, 0 },
                            [&](auto input) { return 0; },
                            cxl::lambda::ctor([&] {
                              config.value.sum = 4;
                              config.value.data_points = 20;
                              std::cout << "hello!\n";
                            }),
                            cxl::lambda::dtor([&] { std::cout << "goodbye!\n"; }) };

  return 0;
}