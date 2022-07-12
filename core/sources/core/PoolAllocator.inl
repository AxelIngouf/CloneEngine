#pragma once
#include "PoolAllocator.h"
#include <cassert>

namespace Core
{
    template <typename T>
    MemoryPool::PoolItem* MemoryPool::PoolItem::StorageToItem(T* t)
    {
        auto* currentItem = reinterpret_cast<PoolItem*>(t);
        return currentItem;
    }

    template <typename T, typename... Args>
    T* MemoryPool::Alloc(Args &&... args)
    {
        GetMemoryPool().storage++;
        T* result = static_cast<T*>(GetMemoryPool().GetVoidPointer(sizeof T));
        new (result) T(std::forward<Args>(args)...);

        return result;
    }

    template <typename T>
    void MemoryPool::Free(T* t)
    {
        if (t != nullptr)
        {
            GetMemoryPool().storage--;
            if constexpr (sizeof T > BLOCK_SIZE)
            {
                ::operator delete(t);
            }
            else
            {
                t->T::~T();
                PoolItem* currentItem = PoolItem::StorageToItem(t);
                {
                    std::lock_guard<std::mutex> lock(GetMemoryPool().poolMutex);
                    currentItem->SetNextItem(GetMemoryPool().freeList);
                    GetMemoryPool().freeList = currentItem;
                }
            }
        }
    }
}