#include "ThreadPool.h"

#include "CLog.h"

namespace Core
{
	ThreadPool ThreadPool::defaultThreadPool(EPoolSize::HARDWARE_MINUS_ONE);

	ThreadPool::ThreadPool(const EPoolSize poolSize)
	{
		int size;
		switch (poolSize)
		{
		case EPoolSize::HARDWARE:
			size = static_cast<int>(std::thread::hardware_concurrency());
			break;
		case EPoolSize::HARDWARE_MINUS_ONE:
			size = static_cast<int>(std::thread::hardware_concurrency()) - 1;
			break;
		}

		size = std::max(size, 1);

		threads.reserve(size);

		for (int threadIndex = 0; threadIndex < size; threadIndex++)
		{
			threads.emplace_back(&ThreadPool::Work, this);
		}

		LOG(LOG_INFO, "A new thread pool was created.");
	}

	ThreadPool::~ThreadPool()
	{
		{
			std::lock_guard<std::mutex> lock{poolMutex};
			isStop = true;
		}
		poolCondVar.notify_all();

		for (auto& thread : threads)
        {
            thread.join();
		}

		threads.clear();
	}

	void ThreadPool::Work()
	{
		std::unique_lock<std::mutex> lock{ poolMutex, std::defer_lock };

        while (true)
		{
			lock.lock();
			while (pendingTasks.empty() && isStop == false)
			{
				poolCondVar.wait(lock);
			}

			if (isStop && pendingTasks.empty())
			{
				break;
			}

			Task task = std::move(pendingTasks.front());
			pendingTasks.pop();

			lock.unlock();

			task();
		}
	}
}
