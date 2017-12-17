#define USING_CALLSTACK
#include <callstack.hpp>
#include <functional>
#include <vector>
#include <thread>
#include <future>
#include <chrono>
#include <iostream>

using std::function;
using std::vector;

void f1();
bool f2();
double f3();
char f4();

template <unsigned T, unsigned C>
void recursive(unsigned I, std::promise<unsigned> result)
{
    TRACE(T);
    if(I == C)
        result.set_value(I);
    else
    {
        //std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        recursive<T, C>(I + 1, std::move(result));
    }
}

int main()
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

void f1()
{
    TRACE(0);
}
bool f2()
{
    TRACE(0, "BOO!... LIAN");
    f1();
}
double f3()
{
    TRACE(0);
    f2();
}
char f4()
{
    TRACE(0);
    f3();
}