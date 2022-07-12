#pragma once
#include <mutex>

int constexpr BLOCK_SIZE = 512;

namespace Core
{
    /**
     * MemoryPool that allows the allocation of pre-reserved memory .
     */
    class MemoryPool
    {
        using StorageType = char[BLOCK_SIZE];

        /**
         * Every item is a union of a BLOCK_SIZE sized block and a pointer
         * to the next PoolItem of the freelist linked-list.
         */
        union PoolItem
        {
            PoolItem() = default;

            /**
             * Returns next PoolItem of the LinkedList.
             * 
             * @return Next PoolItem of the LinkedList.
             */
            [[nodiscard]] PoolItem* GetNextItem() const { return next; }

            /**
             * Set the 'next' PoolItem pointer variable to the given one.
             * 
             * @param nextItem - PoolItem pointer to set as 'next' variable.
             */
            void    SetNextItem(PoolItem* nextItem) { next = nextItem; }

            /**
             * Returns the storage of the current PoolItem.
             * 
             * @return Storage of the current PoolItem.
             */
            void*   GetStorage() { return data; }

            /**
             * Properly casts a T object pointer to a PoolItem pointer.
             * 
             * @param t - T object pointer to cast.
             * @return PoolItem pointer.
             */
            template<typename T>
            static PoolItem* StorageToItem(T* t);

        private:
            static constexpr size_t alignSize = alignof(std::max_align_t);

            /**
             * BLOCK_SIZE sized block.
             */
            alignas(alignSize) StorageType data;

            /**
             * Pointer to the next PoolItem of the freelist linked-list
             */
            PoolItem* next = nullptr;
        };

        /**
         * A PoolArena contains the first PoolItem pointer of a PoolItem linked list,
         * and the pointer to the next PoolArena.
         */
        struct PoolArena
        {
            PoolArena() = delete;
            PoolArena(const PoolArena&) = delete;
            PoolArena(PoolArena&&) = default;
            PoolArena& operator=(const PoolArena&) = delete;
            PoolArena& operator=(PoolArena&&) = delete;
            ~PoolArena() = default;

            /**
             * Create a new PoolArena with a given number of fixed size blocks it can hold.
             * 
             * @param size Number of fixed size blocks the arena can hold.
             */
            explicit PoolArena(size_t size);

            /**
             * Returns a pointer to the first PoolItem of the storage list. This is useful when creating a new PoolArena.
             *
             * @return A pointer to the first PoolItem of the storage list.
             */
            [[nodiscard]] PoolItem* GetStorage() const;

            /**
             * Add a new PoolArena after the current one.
             * 'next' variable needs to be null, otherwise the program will crash.
             * 
             * @param nextArena - Next PoolArena pointer.
             */
            void        SetNextArena(std::unique_ptr<PoolArena>&& nextArena);

        private:
            std::unique_ptr<PoolItem[]> storage;
            std::unique_ptr<PoolArena> next;
        };

    public:
        MemoryPool() = delete;
        MemoryPool(const MemoryPool&) = delete;
        MemoryPool(MemoryPool&&) = delete;
        MemoryPool& operator=(const MemoryPool&) = delete;
        MemoryPool& operator=(MemoryPool&&) = delete;
        ~MemoryPool() = default;

        /**
         * Initializes the memory pool with a single PoolArena of given size.
         * The size corresponds to the number of blocks of fixed size (BLOCK_SIZE) the arenas can hold.
         * When all arenas are full, meaning 'freeList' is equal to nullptr,
         * a new one is automatically created when trying to allocate memory.
         * 
         * @param sizeFirstArena - Size of the first PoolArena
         * @param sizeNextArenas - Size of all the following PoolArenas, defaulted to the size of the first PoolArena.
         */
         explicit MemoryPool(size_t sizeFirstArena, size_t sizeNextArenas = 0);

        /**
         * Allocates memory for a given T object type with its constructor arguments.
         * 
         * @tparam T - Object type to allocate memory to.
         * @tparam Args - T object constructor arguments typename.
         * @param args - T object constructor arguments.
         * @return Pointer to allocated object.
         */
        template <typename T, typename... Args>
        T* Alloc(Args &&... args);

        /**
         * Free the specified object memory and put it back in the memory pool.
         * 
         * @tparam T - Pointer object type.
         * @param t - T Object pointer to free.
         */
        template <typename T>
        void    Free(T* t);

    private:
        /**
         * Add a new PoolArena at front of the arenas linked list.
         */
        void    AddNewArena();

        /**
         * Returns a usable void pointer. If memorySize is lesser than or equal to BLOCK_SIZE,
         * returns an already allocated void pointer* of BLOCK_SIZE size,
         * otherwise returns a freshly allocated void pointer of memorySize size.
         *
         * @return Usable void pointer.
         */
         void* GetVoidPointer(size_t memorySize);

        /**
         * Pointer to the front PoolArena of the arenas linked list.
         */
        std::unique_ptr<PoolArena> arena;

        /**
         * The size of each PoolArena created by the MemoryPool.
         */
        size_t      arenaSize;

        /**
         * List of every free (not used) PoolItem.
         */
        PoolItem* freeList;

        /**
         * PoolAllocator mutex. Used to ensure it's thread-safe.
         */
        std::mutex  poolMutex;
    };
}

#include "PoolAllocator.inl"