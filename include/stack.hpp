#pragma once

#include <cstdlib>
#include <cstdint>
#include <stdexcept>

template<typename T>
class Stack
{
private:
    size_t capacity = 0;
    size_t current = 0;
    T* data = nullptr;

    void Expand()
    {
        T* new_data = new T[capacity * 2];

        for (size_t idx = 0; idx < capacity; idx++)
            new_data[idx] = data[idx];

        delete[] data;
        data = new_data;
        
        capacity *= 2;
    }

public:
    Stack(size_t initial_capacity = 4)
    {
        capacity = initial_capacity;
        data = new T[capacity];
    }

    ~Stack()
    {
        if (data)
        {
            delete[] data;
            data = nullptr;
        }
    }

    void Push(T elem)
    {
        if (current >= capacity)
            Expand();

        data[current++] = elem;
    }

    T Pop()
    {
        if (current == 0)
            throw std::runtime_error("Can't pop from empty stack");
        return data[--current];
    }

    void Clear()
    {
        current = 0;
    }
};
