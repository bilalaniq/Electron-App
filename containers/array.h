// -----------------------------------std:array------------------------------
/*
in this code we are creating an std::array like class which will work the same
so heres the code
*/

#include <initializer_list>

template <typename Array, typename T = typename Array::valueType>
class Iterator
{
    using valueType = T;
    using PointerType = valueType *;
    using ReferenceType = valueType &;
    using ConstPointerType = const valueType *;
    using ConstReferenceType = const valueType &;

public:
    Iterator(PointerType ptr)
        : m_ptr(ptr) {}

    Iterator(ConstPointerType ptr)
        : m_ptr(ptr) {}

    Iterator &operator++()
    {
        m_ptr++;
        return *this;
    }

    Iterator operator++(int)
    {
        Iterator iterator = *this;
        ++(*this);
        return iterator;
    }

    Iterator &operator--()
    {
        m_ptr--;
        return *this;
    }

    Iterator operator--(int)
    {
        Iterator iterator = *this;
        --(*this);
        return iterator;
    }

    ReferenceType operator[](int index)
    {
        return *(m_ptr + index);
    }

    ConstReferenceType operator[](int index) const
    {
        return *(m_ptr + index);
    }

    PointerType operator->()
    {
        return m_ptr;
    }

    ConstPointerType operator->() const
    {
        return m_ptr;
    }

    ReferenceType operator*()
    {
        return *m_ptr;
    }

    ConstReferenceType operator*() const
    {
        return *m_ptr;
    }

    bool operator==(const Iterator &other) const
    {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const Iterator &other) const
    {
        return !(*this == other);
    }

private:
    PointerType m_ptr;
};

template <typename T, size_t N>
class Array
{
public:
    using valueType = T;
    using Iterator = Iterator<Array<T>>;

private:
    T array[N];

public:
    Array()
    {
        for (size_t i = 0; i < N; ++i)
        {
            array[i] = T();
        }
    }

    Array(std::initializer_list<T> const &init_list) // passing it by const refrence
    {
        if (init_list.size() > N)
        {
            throw std::out_of_range("Initializer list size exceeds array size");
        }
        size_t i = 0;
        for (; i < init_list.size(); ++i)
        {
            array[i] = init_list.begin()[i]; // Copy the elements from the initializer list
        }
        // Fill the remaining elements with default values (in case the initializer list is smaller than N)
        for (; i < N; ++i)
        {
            array[i] = T(); // Default-construct the remaining elements
        }
    }

    // or you cold also d it by using iterators as the std::inializer list works better with iterators not with subscripting
    // so this is preffered more then the above way
    // Array(std::initializer_list<T> const& init_list)
    // {
    //     size_t i = 0;
    //     for (auto& value : init_list)
    //     {
    //         array[i] = value;
    //         i++;
    //     }
    //     for (; i < N; ++i)
    //     {
    //         array[i] = T();
    //     }
    // }

    T &operator[](size_t index)
    {
        if (index >= N) // No need to check for negative index
        {
            throw std::out_of_range("Index out of range"); // Use exceptions instead of exit
        }
        return array[index];
    }

    const T &operator[](size_t index) const
    {
        if (index >= N) // No need to check for negative index
        {
            throw std::out_of_range("Index out of range"); // Use exceptions instead of exit
        }
        return array[index];
    }

    T *data()
    {
        return array; // Return the underlying array
    }

    const T *data() const
    {
        return array; // Return the underlying array
    }

    constexpr size_t size() const { return N; } // Marked as const

    Iterator begin()
    {
        return Iterator(array);
    }
    Iterator end()
    {
        return Iterator(array + N);
    }

    const Iterator begin() const
    {
        return Iterator(array);
    }

    const Iterator end() const
    {
        return Iterator(m_Data + N);
    }

    ~Array() {}
};
