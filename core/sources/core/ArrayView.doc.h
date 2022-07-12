#pragma once

namespace Core
{
    /**
    * ArrayView is a wrapper class use to pass std::vector data between dll.
    *
    * @author		Francis Blanchet
    * @version		1.0
    * @since		1.0
    */
    template<typename T>
    class  ArrayView
    {
    public:
        ArrayView(const T* ptr, size_t size) : m_data(ptr), m_size(size) {}

        const T& operator[](size_t index) const { return m_data[index]; }

        /**
        * Return the number of element in the underlying contiguous memory.
        *
        * @return			Number of element.
        */
        size_t Size() const { return m_size; }

    private:
        const T* const m_data;
        const size_t m_size;
    };
}
