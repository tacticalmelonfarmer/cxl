#ifdef TRACE_CALLS

namespace debug
{

#include <string>
#include <iostream>
using std::string;

string operator*(unsigned count, const string& str)
{
    string result;
    for(auto i=0; i < count; i++)
    {
        result += str;
    }
    return result;
}

struct trace
{
    static size_t depth;
    static string pretty;
    static void message(const string& Message)
    {
        pretty += Message;
    }
    static void clear()
    {
        depth = 0;
        pretty = "";
    }
    trace(const string& id)
    {
        if(depth == 0)
        {
            ++depth;
            pretty += "$" + id + ":\n";
        }else{
            ++depth;
            pretty += (depth * string("|")) + id + ":\n";
        }
    }
    ~trace()
    {
        --depth;
        if(depth == 0)
            std::cout << pretty << std::endl;
    }
};
size_t trace::depth = 0;
string trace::pretty = "";

}

#define TRACE0() auto _scope_trace = debug::trace(__PRETTY_FUNCTION__)
#define TRACE1(id) auto _scope_trace = debug::trace(#id)
#define GET_TRACE_MACRO(_1, MACRO, ...) MACRO
#define TRACE(...) GET_TRACE_MACRO(__VA_ARGS__, TRACE1, TRACE0)(__VA_ARGS__)

#else

#define TRACE0() 
#define TRACE1(id) 
#define GET_TRACE_MACRO(_1, MACRO, ...) MACRO
#define TRACE(...) GET_TRACE_MACRO(__VA_ARGS__, TRACE1, TRACE0)(__VA_ARGS__) 

#endif