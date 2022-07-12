#pragma once
#include <mutex>

int constexpr BLOCK_SIZE = 512;

namespace Core
{
    class MemoryPool
    {
        using StorageType = char[BLOCK_SIZE];
        union PoolItem
        {
            PoolItem() = default;

            [[nodiscard]] PoolItem*   GetNextItem() const { return next; }
            void        SetNextItem(PoolItem* nextItem) { next = nextItem; }
            void*       GetStorage() { return data; }

            template<typename T>
            static PoolItem*    StorageToItem(T* t);

        private:

            static constexpr size_t alignSize = alignof(std::max_align_t);
            alignas(alignSize) StorageType data;

            PoolItem* next = nullptr;
        };

        struct PoolArena
        {
            PoolArena() = delete;
            PoolArena(const PoolArena&) = delete;
            PoolArena(PoolArena&&) = default;
            PoolArena& operator=(const PoolArena&) = delete;
            PoolArena& operator=(PoolArena&&) = delete;
            ~PoolArena() = default;

            explicit PoolArena(size_t size);

            [[nodiscard]] PoolItem* GetStorage() const { return storage.get(); }

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

        explicit MemoryPool(size_t sizeFirstArena, size_t sizeNextArenas = 0);

        template <typename T, typename... Args>
        static T*      Alloc(Args &&... args);

        template <typename T>
        static void    Free(T* t);

    private:
        static MemoryPool& GetMemoryPool();

        static void    AddNewArena();
        static void*   GetVoidPointer(size_t memorySize);

        std::unique_ptr<PoolArena> arena;
        size_t      arenaSize;
        PoolItem*   freeList;

        std::mutex  poolMutex;

        int storage = 0;
    };
}

#include "PoolAllocator.inl"