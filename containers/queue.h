template <typename T>
class Queue
{
public:
    Queue() : front(nullptr), rear(nullptr), m_count(0) {}

    ~Queue()
    {
        clear();
    }

    // Const and non-const overload of frontElement combined
    const T &frontElement() const
    {
        if (isEmpty())
        {
            throw std::runtime_error("Queue is empty!");
        }
        return front->data;
    }

    T &frontElement()
    {
        if (isEmpty())
        {
            throw std::runtime_error("Queue is empty!");
        }
        return front->data;
    }

    Queue(const Queue &other)
    {
        front = rear = nullptr;
        m_count = 0;
        Node *temp = other.front;
        while (temp != nullptr)
        {
            enqueue(temp->data);
            temp = temp->next;
        }
    }

    Queue &operator=(const Queue &other)
    {
        if (this != &other)
        {
            clear();
            Node *temp = other.front;
            while (temp != nullptr)
            {
                enqueue(temp->data);
                temp = temp->next;
            }
        }
        return *this;
    }

    Queue(Queue &&other) noexcept
    {
        front = other.front;
        rear = other.rear;
        m_count = other.m_count;
        other.front = other.rear = nullptr;
        other.m_count = 0;
    }

    Queue &operator=(Queue &&other) noexcept
    {
        if (this != &other)
        {
            clear();
            front = other.front;
            rear = other.rear;
            m_count = other.m_count;
            other.front = other.rear = nullptr;
            other.m_count = 0;
        }
        return *this;
    }

    void enqueue(const T &item)
    {
        Node *newNode = new Node(item);
        m_count++;
        if (rear == nullptr)
        {
            front = rear = newNode;
            return;
        }
        rear->next = newNode;
        rear = newNode;
    }

    T dequeue()
    {
        if (front == nullptr)
        {
            throw std::runtime_error("Queue is empty!"); // Handle empty queue scenario
        }
        Node *temp = front;
        T data = front->data;  // Store the data to return it
        front = front->next;
        if (front == nullptr)
        {
            rear = nullptr;
        }
        delete temp;
        m_count--;
        return data;
    }

    size_t size() const
    {
        return m_count;
    }

    bool isEmpty() const
    {
        return front == nullptr;
    }

    void clear()
    {
        while (!isEmpty())
        {
            dequeue();
        }
    }

private:
    struct Node
    {
        T data;
        Node *next;
        Node(T val) : data(val), next(nullptr) {}
    };

    Node *front;
    Node *rear;
    size_t m_count;
};