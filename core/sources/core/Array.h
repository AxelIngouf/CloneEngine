#pragma once

#include <cstring>
#include <vector>

namespace Core
{
    template<typename T>
    class ArrayIterator
    {
    public:
        ArrayIterator(const T* current) :
            current(current)
        {}

        bool operator!=(const ArrayIterator<T>& rhs) const { return current != rhs.current; }
        const T& operator*() const { return *current; }
        void operator++() { current++; }

    private:
        const T* current;

    };

    template<typename T>
    class Array // Todo : doc
    {
    public:
        Array() = delete;
        Array(size_t size) : m_data(new T[size]), m_size(size) {}
        Array(const Array& other) : m_data(new T[other.m_size]), m_size(other.m_size) { if (m_size) memcpy(m_data, other.m_data, m_size * sizeof(T)); }
        Array(Array&& other) noexcept : m_data(other.m_data), m_size(other.m_size) { other.m_data = nullptr; }
        Array(const std::vector<T>& source) : m_data(new T[source.size()]), m_size(source.size()) { if (m_size) memcpy(m_data, &source[0], m_size * sizeof(T)); }
        //Array(const ArrayView& source); // todo
        //Array(std::vector<T>&& source) : m_data(&source[0]), m_size(source.size()) { source. ? ? ? ; } // Todo: figure out ???
        ~Array() { if (m_data) delete[] m_data; }

        Array& operator=(const Array& other) { m_data = new T[other.m_size]; m_size = other.m_size; if (m_size) memcpy(m_data, other.m_data, m_size * sizeof(T)); return *this; }
        Array& operator=(Array&& other) noexcept { m_data = other.m_data; m_size = other.m_size; other.m_data = nullptr; return *this; }
        //Array& operator=(const std::vector<T>& source); // todo
        //Array& operator=(const ArrayView& source); // todo

        T& operator[](size_t index) { return m_data[index]; }

        size_t Size() const { return m_size; }
        T* Data() const { return m_data; }

        ArrayIterator<T> begin() const { return ArrayIterator<T>(m_data); }
        ArrayIterator<T> end() const { return ArrayIterator<T>(&m_data[m_size]); }

    private:
        T* m_data;
        size_t m_size;
    };


    template<typename T>
    class ArrayView // Todo : redo doc
    {
    public:
        ArrayView() = delete;
        ArrayView(const T* ptr, size_t size) : m_data(ptr), m_size(size) {}
        ArrayView(const ArrayView& other) = default;
        ArrayView(ArrayView&& other) noexcept = default;
        ArrayView(const Array<T>& source) : m_data(source.Data()), m_size(source.Size()) {}
        ArrayView(const std::vector<T>& source) : m_data(source.size() ? &source[0] : nullptr), m_size(source.size()) {}
        ~ArrayView() = default;

        ArrayView& operator=(const ArrayView& other) = default;
        ArrayView& operator=(ArrayView&& other) noexcept = default;
        //ArrayView& operator=(const Array<T>& source); // todo
        //ArrayView& operator=(const std::vector<T>& source); // todo

        const T& operator[](size_t index) const { return m_data[index]; }

        size_t Size() const { return m_size; }
        const T* const Data() const { return m_data; }

        ArrayIterator<T> begin() const { return ArrayIterator<T>(m_data); }
        ArrayIterator<T> end() const { return ArrayIterator<T>(&m_data[m_size]); }

    private:
        const T* const m_data;
        const size_t m_size;
    };
}
