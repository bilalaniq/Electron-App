template <typename T>
class SinglyLinkedList
{
private:
    // Node structure
    struct Node
    {
        T data;     // Data field
        Node *next; // Pointer to the next node

        template <typename... Args>
        Node(Args &&...args) : data(std::forward<Args>(args)...), next(nullptr) {}
    };

    Node *head; // Pointer to the head of the list
    size_t count;

public:
    SinglyLinkedList() : head(nullptr), count(0) {} // Constructor initializes head to nullptr
    SinglyLinkedList(const SinglyLinkedList &obj)
        : head(nullptr), count(0)
    {
        if (!obj.head)
        {
            return;
        }

        Node *current = obj.head;
        while (current)
        {
            this->push_back(current->data);
            current = current->next;
        }
    }

    SinglyLinkedList &operator=(const SinglyLinkedList &obj)
    {
        if (this == &obj)
        {
            return *this;
        }
        this->clear();

        if (!obj.head)
        {
            return *this;
        }

        Node *current = obj.head;
        while (current)
        {
            this->push_back(current->data);
            current = current->next;
        }
        return *this;
    }

    SinglyLinkedList(SinglyLinkedList &&obj) noexcept
        : head(obj.head), count(obj.count)
    {
        obj.head = nullptr;
        obj.count = 0;
    }

    SinglyLinkedList &operator=(SinglyLinkedList &&obj) noexcept
    {
        if (this == &obj)
        {
            return *this;
        }
        head = obj.head;
        count = obj.count;
        obj.head = nullptr;
        obj.count = 0;

        return *this;
    }

    // Insert a new node at the beginning
    void push_front(const T &value)
    {
        Node *newNode = new Node(value);
        newNode->next = head; // New node points to current head
        head = newNode;       // Head now points to the new node
        count++;
    }

    // Insert a new node at the end
    void push_back(const T &value)
    {
        Node *newNode = new Node(value);
        if (!head)
        {
            head = newNode; // If the list is empty, set head to the new node
            count++;
            return;
        }
        Node *current = head;
        while (current->next)
        {                            // cheaks if the next pointer is not nullptr if it is then current is the last node
            current = current->next; // Traverse to the last node
        }
        current->next = newNode; // Link the new node
        count++;
    }

    template <typename... ARGS>
    void emplace_back(ARGS &&...args)
    {
        Node *newnode = new Node(std::forward<ARGS>(args)...);
        if (!head)
        {
            head = newnode;
            count++;
            return;
        }
        Node *current = head;
        while (current->next)
        {
            current = current->next;
        }

        current->next = newnode;
        count++;
    }

    void insertAT(const T &value, size_t location)
    {
        if (location > count)
        {
            throw std::out_of_range("Position out of bounds");
        }
        if (location == 0)
        {
            push_front(value);
        }
        else if (location == count)
        {
            push_back(value);
        }
        else
        {
            Node *current = head;
            for (size_t i = 0; i < location - 1; i++)
            {
                current = current->next;
            }
            Node *newnode = new Node(value);
            newnode->next = current->next;
            current->next = newnode;
            count++;
        }
    }

    void deleteByValue(const T &value, bool deleteAll = false)
    {
        while (head && head->data == value)
        {
            Node *temp = head;
            head = head->next; 
            delete temp;       
            count--;
            if (!deleteAll)
            {
                return;
            }
        }

        Node *current = head;
        while (current)
        {
            // Check if the next node needs to be deleted
            if (current->next && current->next->data == value)
            {
                Node *temp = current->next;
                current->next = temp->next; // Bypass the node to delete
                delete temp;                // Free memory
                count--;

                // If we're deleting only one occurrence, exit the loop
                if (!deleteAll)
                {
                    return;
                }
            }
            else
            {
                current = current->next; // Move to the next node
            }
        }
    }

    void pop_back()
    {
        if (!head)
        {
            return;
        }

        if (!head->next)
        {
            delete head;
            head = nullptr;
        }
        else
        {
            Node *current = head;
            Node *pre = nullptr;
            while (current->next)
            {
                pre = current;
                current = current->next;
            }
            delete current;
            if (pre)
            {
                pre->next = nullptr;
            }
        }
        count--;
    }

    size_t size() const
    {
        return count;
    }

    // Destructor to free memory
    ~SinglyLinkedList()
    {
        clear();
    }

    void clear()
    {
        while (head)
        {
            Node *temp = head;
            head = head->next;
            delete temp;
        }
        head = nullptr;
        count = 0;
    }
};
