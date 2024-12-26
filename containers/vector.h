#include <initializer_list>

template <typename VECTOR, typename T = typename VECTOR::valueType>
class VectorIterator
{
    using valueType = T;
    using PointerType = valueType *;
    using ReferenceType = valueType &;
    using ConstPointerType = const valueType *;
    using ConstReferenceType = const valueType &;

public:
    VectorIterator(PointerType ptr)
        : m_ptr(ptr) {}

    VectorIterator(ConstPointerType ptr)
        : m_ptr(ptr) {}

    VectorIterator &operator++()
    {
        m_ptr++;
        return *this;
    }

    VectorIterator operator++(int)
    {
        VectorIterator iterator = *this;
        ++(*this);
        return iterator;
    }

    VectorIterator &operator--()
    {
        m_ptr--;
        return *this;
    }

    VectorIterator operator--(int)
    {
        VectorIterator iterator = *this;
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

    bool operator==(const VectorIterator &other) const
    {
        return m_ptr == other.m_ptr;
    }

    bool operator!=(const VectorIterator &other) const
    {
        return !(*this == other);
    }

private:
    PointerType m_ptr;
};

template <typename T>
class VECTOR
{
public:
    using valueType = T;
    using Iterator = VectorIterator<VECTOR<T>>;

public:
    VECTOR()
    {
        Realloc(2);
    }

    VECTOR(size_t size)
    {
        Realloc(size);
    }

    VECTOR(const VECTOR<T> &obj)
    {
        for (const auto &elements : obj)
        {
            this->PushBack(elements);
        }
    }

    VECTOR<T> &operator=(const VECTOR<T> &obj)
    {
        if (this != &obj)
        {
            this->Clear();
            for (const auto &elements : obj)
            {
                this->PushBack(elements);
            }
        }
        return *this;
    }

    VECTOR(VECTOR<T> &&obj) noexcept : m_Data(nullptr), m_Size(0), m_Capacity(0)
    {
        m_Data = obj.m_Data;
        m_Size = obj.m_Size;
        m_Capacity = obj.m_Capacity;

        obj.m_Data = nullptr;
        obj.m_Size = 0;
        obj.m_Capacity = 0;
    }

    VECTOR<T> &operator=(VECTOR<T> &&obj) noexcept
    {
        if (this != &obj)
        {
            this->Clear();

            m_Data = obj.m_Data;
            m_Size = obj.m_Size;
            m_Capacity = obj.m_Capacity;

            // Leave obj in a valid state
            obj.m_Data = nullptr;
            obj.m_Size = 0;
            obj.m_Capacity = 0;
        }
        return *this;
    }

    VECTOR(std::initializer_list<T> const &init_list)
    {
        Realloc(init_list.size());
        // m_Size = init_list.size();

        // Copy or move elements from the initializer list to the container
        for (const auto &value : init_list)
        {
            PushBack(value); // Calls PushBack(const T& value)
        }
    }

    ~VECTOR()
    {
        Clear();
        ::operator delete(m_Data, m_Capacity * sizeof(T));
    }

    void PushBack(const T &value)
    {
        if (m_Size >= m_Capacity)
            Realloc(m_Capacity + m_Capacity / 2);

        new (&m_Data[m_Size]) T(value);
        // m_Data[m_Size] = value;
        m_Size++;
    }

    void PushBack(T &&value)
    {
        if (m_Size >= m_Capacity)
            Realloc(m_Capacity + m_Capacity / 2);

        new (&m_Data[m_Size]) T(std::move(value));

        // m_Data[m_Size] = std::move(value);

        m_Size++;
    }

    template <typename... Args>
    T &EmplaceBack(Args &&...args) // this is an inplace constructor
    {
        if (m_Size >= m_Capacity)
            Realloc(m_Capacity + m_Capacity / 2);

        new (&m_Data[m_Size]) T(std::forward<Args>(args)...);

        // m_Data[m_Size] = T(std::forward<Args>(args)...);

        // The expression T(std::forward<Args>(args)...) creates a temporary object of type T using the forwarded
        //  arguments. This temporary object will be constructed first and then assigned to m_Data[m_Size].
        // When you perform the assignment m_Data[m_Size] = ..., you're invoking the assignment operator of type T.
        //  This means that the existing object at m_Data[m_Size] will be overwritten by the new temporary object.
        // so use placement new
        return m_Data[m_Size++];
    }

    void reserve(size_t newcapacity)
    {
        if (newcapacity > m_Capacity)
        {
            Realloc(newcapacity);
        }
    }

    void resize(size_t newcapacity)
    {
        Realloc(newcapacity);
    }

    void assign(size_t newsize, const T &value)
    {
        resize(newsize);

        for (size_t i = 0; i < newsize; i++)
        {
            m_Data[i] = value;
        }

        //   you can also do it like this
        // by using iterators
        // std::fill(this->begin() , this->end() , value);
        // or by using using just size
        // std::fill( m_Data, m_Data + m_Size value);
    };

    void PopBack()
    {
        if (m_Size > 0)
        {
            m_Size--;
            m_Data[m_Size].~T(); // this cause the double delesion this is why i have used placement new instead of new expression
        }
    }

    void Clear()
    {
        for (size_t i = 0; i < m_Size; i++)
            m_Data[i].~T();
        m_Size = 0;
    }

    bool operator==(const VECTOR<T> &obj) const
    {
        if (this->Size() != obj.Size())
        {
            return false;
        }

        for (size_t i = 0; i < this->Size(); ++i)
        {
            if (this->m_Data[i] != obj.m_Data[i])
            { // Calls the CTF_player != operator
                return false;
            }
        }
        return true;
    }

    const T &operator[](size_t index) const { return m_Data[index]; }
    T &operator[](size_t index) { return m_Data[index]; }

    size_t Size() const { return m_Size; }

    Iterator begin()
    {
        return Iterator(m_Data);
    }
    Iterator end()
    {
        return Iterator(m_Data + m_Size);
    }

    const Iterator begin() const
    {
        return Iterator(m_Data);
    }

    const Iterator end() const
    {
        return Iterator(m_Data + m_Size);
    }

    bool empty() const
    {
        return m_Size == 0;
    }

private:
    void Realloc(size_t newcapacity)
    {
        // T* newBlock = new T[newcapacity];    we could do the same but this is wrong as it calls the constructors
        // class that vector has been storing so in future we will be deleting
        // this block of memory in the future by using the clear function and if the
        // code end it will then call the distructor of the vector class which will
        // again call the distructor of the classes and will give us an error
        // so we will use new operator instead

        T *newBlock = (T *)::operator new(newcapacity * sizeof(T));

        // note that this is an operator new not placement new
        // note that placemnet new uses new expression not new operator

        if (newcapacity < m_Size)
        {
            m_Size = newcapacity;
        }

        for (size_t i = 0; i < m_Size; i++)
        {
            new (&newBlock[i]) T(std::move(m_Data[i]));

            // newBlock[i] = std::move(m_Data[i]);

            // this is wrong for std::string because the newblock is an place for std::string but is not object itself because we have
            // used new operator not new expression it tries to tidy allocate the string which we have in the new block but the current
            //  string never existed because we have allocated memory with out calling the constructor
        }

        for (size_t i = 0; i < m_Size; i++)
        {
            m_Data[i].~T();
        }

        ::operator delete(m_Data, m_Capacity * sizeof(T));
        m_Data = newBlock;
        m_Capacity = newcapacity;
    }

private:
    T *m_Data = nullptr;
    size_t m_Size = 0;
    size_t m_Capacity = 0;
};
