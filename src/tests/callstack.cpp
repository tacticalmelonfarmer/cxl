#define USING_CALLSTACK
#include <callstack.hpp>
#include <chrono>
#include <future>
#include <iostream>
#include <thread>

void
f1();
bool
f2();
double
f3();
char
f4();

template<unsigned T, unsigned C>
void
recursive(unsigned I, std::promise<unsigned> result)
{
  TRACE(T, "");
  if (I == C) {
    utility::trace<T>::message("I did it!");
    result.set_value(I);
  } else {
    utility::trace<T>::message(std::string("worker thread #") + std::to_string(T) + " recursion #" + std::to_string(I));
    recursive<T, C>(I + 1, std::move(result));
  }
}

int
main()
{
  TRACE(0);
  f4();
  std::promise<unsigned> p1;
  std::future<unsigned> r1 = p1.get_future();
  std::promise<unsigned> p2;
  std::future<unsigned> r2 = p2.get_future();
  std::thread th1(recursive<1, 5>, 1, std::move(p1)), th2(recursive<2, 10>, 1, std::move(p2));
  r1.wait();
  r2.wait();
  th1.join();
  th2.join();
}

void
f1()
{
  TRACE(0);
}
bool
f2()
{
  TRACE(0);
  utility::trace<0>::message("BOO!... LIAN");
  f1();
  return true;
}
double
f3()
{
  TRACE(0);
  f2();
  return 3.14;
}
char
f4()
{
  TRACE(0);
  f3();
  return '?';
}