#pragma once
#include <thread>
#include <queue>
#include <functional>
#include <mutex>

namespace Core
{
	using Task = std::function<void()>;

	enum class EPoolSize
	{
		HARDWARE = 0,
		HARDWARE_MINUS_ONE = -1
	};

	class ThreadPool
	{
	public:
		ThreadPool() = delete;
		 explicit ThreadPool(const int threadQuantity) : ThreadPool(static_cast<EPoolSize>(threadQuantity)) {}
		 explicit  ThreadPool(EPoolSize poolSize);
		ThreadPool(ThreadPool const& other) = delete;
		ThreadPool(ThreadPool&& other) = delete;
		 ~ThreadPool();

		ThreadPool& operator=(ThreadPool const& other) = delete;
		ThreadPool& operator=(ThreadPool&& other) = delete;

        [[nodiscard]] int	GetSize() const { return static_cast<int>(threads.size()); }
		
		template<class Callable, class ...Args>
		auto	AddTask(Callable&& function, Args&& ...args);

		static ThreadPool defaultThreadPool;

	private:
		void	Work();

		std::mutex				poolMutex;
		std::condition_variable	poolCondVar;
		bool					isStop = false;

		std::vector<std::thread>	threads;
		std::queue<Task>			pendingTasks;
	};
}

#include "ThreadPool.inl"
