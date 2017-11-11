#ifdef USING_CALLSTACK
#include <string>
#include <iostream>


namespace utility
{
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

template <unsigned U>
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
template <unsigned U>
size_t trace<U>::depth = 0;
template <unsigned U>
string trace<U>::pretty = "";

}

#define TRACE0(channel) auto _scope_trace = utility::trace<channel>(__PRETTY_FUNCTION__)
#define TRACE1(channel, signature) auto _scope_trace = utility::trace<channel>(signature)
#define GET_TRACE_MACRO(_1, _2, MACRO, ...) MACRO

#define TRACE(...) GET_TRACE_MACRO(__VA_ARGS__, TRACE1, TRACE0)(__VA_ARGS__)
#define MESSAGE(channel, message) utility::trace<channel>::message(message)

#else

#define DO_NOTHING() 
#define TRACE(...) DO_NOTHING()

#endif