#include <cstdint>
#include <vector>
#include <memory>
#include <tuple>
#include <algorithm>

namespace utility
{
    template <typename T>
    using pool_ptr = std::tuple<size_t, size_t, std::shared_ptr<T>>;

    template <typename T>
    size_t pool_index(const pool_ptr<T> &from)
    {
        return std::get<0>(from);
    }

    template <typename T>
    size_t pool_chunk(const pool_ptr<T> &from)
    {
        return std::get<1>(from);
    }

    template <typename T>
    operator std::shared_ptr<T> (const pool_ptr<T> &from)
    {
        return std::get<2>(from);
    }

    template <typename T>
    struct dynamic_chunk
    {
        dynamic_chunk(size_t Size, dynamic_chunk<T>* Prev, dynamic_chunk<T>* Next, size_t Index=0)
            : _size(Size), _prev(Prev), _next(Next)
        {
            _trace.resize(Size);
            _content = new T[Size];
        }
        ~dynamic_chunk()
        { delete [] _content; }
        // chunk size in sizeof(T) units
        size_t size() const
        { return _size; }
        // get index of first available spot, return true on success
        bool available(size_t& Out) const
        {
            Out = std::distance(_trace.begin(), std::find(_trace.begin(), _trace.end(), false));
            return Out != trace.end();
        }
        pool_ptr<T> allocate()
        {
            size_t tmp_index;
            if(available(tmp_index))
            {
                _trace[tmp_index] = true;
                return std::make_tuple(tmp_index, _index, & static_cast<T>(*(_content + tmp_index)));
            }
            else
            {
                extend(_size); // we could implement adaptive chunks later
                return _next->allocate(); // i should check for null here?...
            }
        }
        void deallocate(pool_ptr<T>& ptr)
        {
            if(auto with = at_chunk(pool_chunk(ptr)); with->available(pool_index(ptr)))
            {
                ptr->~T();
                with->_trace[pool_index(ptr)] = false;
            }
        }
    private:
        // add a chunk of heap and assign to _next
        void extend(size_t Size)
        {
            _next = new dynamic_chunk<T>(Size, this, nullptr);
        }
        dynamic_chunk<T>* at_chunk(size_t ChunkIndex)
        {
            for(auto i = this; i->_index <= ChunkIndex; i = i->_next)
            {
                if(i->_index == ChunkIndex)
                {
                    return i;
                }
                if(i->_next == nullptr)
                {
                    break;
                }
            }
        }
        size_t const _index;
        size_t const _size;
        std::vector<bool> _trace;
        T* _content;
        dynamic_chunk<T>* _prev, _next;
    };

    struct adaptive_chunk_policy
    {};

    template <typename T, typename ChunkPolicy>
    struct pool_allocator
    {
        typedef ChunkPolicy chunk_policy;
        typedef T value_type;
    };
}