#include "PoolAllocator.h"

#include "CLog.h"

namespace Core
{
    MemoryPool& MemoryPool::GetMemoryPool()
    {
        static MemoryPool memoryPool(500, 50);

        return memoryPool;
    }

    MemoryPool::PoolArena::PoolArena(const size_t size) : storage(new PoolItem[size])
    {
        for (size_t i = 1; i < size; i++)
        {
            storage[i - 1].SetNextItem(&storage[i]);
        }
        storage[size - 1].SetNextItem(nullptr);
    }

    void MemoryPool::PoolArena::SetNextArena(std::unique_ptr<PoolArena>&& nextArena)
    {
        if(next)
        {
            LOG(LOG_ERROR, "PoolAllocator : Tried to override already existing PoolArena inside a linked list.");
            exit(EXIT_FAILURE);
        }

        next = std::move(nextArena);
    }

    MemoryPool::MemoryPool(const size_t size, const size_t sizeNextArenas) : arena(std::make_unique<PoolArena>(PoolArena(size))), arenaSize(size),
        freeList(arena->GetStorage())
    {
        if(sizeNextArenas > 8)
        {
            arenaSize = sizeNextArenas;
        }
        LOG(LOG_INFO, "New MemoryPool created");
    }

    void* MemoryPool::GetVoidPointer(const size_t size)
    {
        if(size > BLOCK_SIZE)
        {
            // If memory to allocate is greater than current block sizes, then just allocate memory with new operator
            return ::operator new(size);
        }

        PoolItem* currentItem;
        {
            std::lock_guard<std::mutex> lock(GetMemoryPool().poolMutex);

            if (GetMemoryPool().freeList == nullptr)
            {
                AddNewArena();
            }

            currentItem = GetMemoryPool().freeList;
            GetMemoryPool().freeList = currentItem->GetNextItem();
        }

        return currentItem->GetStorage();
    }

    void MemoryPool::AddNewArena()
    {
        auto newArena = std::make_unique<PoolArena>(GetMemoryPool().arenaSize);
        newArena->SetNextArena(std::move(GetMemoryPool().arena));
        GetMemoryPool().arena = std::move(newArena);
        GetMemoryPool().freeList = GetMemoryPool().arena->GetStorage();
    }
}
