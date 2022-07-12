#pragma once
#include <thread>
#include <queue>
#include <functional>
#include <mutex>

namespace Core
{
	using Task = std::function<void()>;

    /**
	 * Enum of pool sizes. Either hardware's number of cores, or number of cores minus one (to let a thread for the main program).
	 */
	enum class EPoolSize
	{
		HARDWARE = 0,
		HARDWARE_MINUS_ONE = -1
	};

    /**
	 * Pool of fixed number of threads that can execute multiple given functions with specified arguments simultaneously.
	 * All the threads are the same from the creation to the destruction of the pool.
	 */
	class ThreadPool
	{
	public:
		ThreadPool() = delete;
		explicit ThreadPool(const int threadQuantity) : ThreadPool(static_cast<EPoolSize>(threadQuantity)) {}
		explicit ThreadPool(EPoolSize poolSize);
		ThreadPool(ThreadPool const& other) = delete;
		ThreadPool(ThreadPool&& other) = delete;
		~ThreadPool();

		ThreadPool& operator=(ThreadPool const& other) = delete;
		ThreadPool& operator=(ThreadPool&& other) = delete;

        /**
		 * Returns number of threads in the pool.
		 * 
		 * @return Number of threads.
		 */
		[[nodiscard]] int	GetSize() const { return static_cast<int>(threads.size()); }

        /**
		 * Add a task to the queue of pending tasks for it to be executed by a thread.
		 * 
		 * @tparam Callable - Callable function template
		 * @tparam Args - Callable function arguments template
		 * @param function - Callable function to be added to the pending tasks
		 * @param args - Callable function arguments
		 * @return Result of the executed callable function
		 */
		template<class Callable, class ...Args>
		auto	AddTask(Callable&& function, Args&& ...args);

	private:
        /**
		 * Function executed by each thread until the thread is killed.
		 * When there is no pending task, the thread is put into sleep.
		 */
		void	Work();

        /**
		 * Pool mutex. To ensure the pending tasks queue and the isStop boolean variable are accessed by only one thread.
		 */
		std::mutex				poolMutex;

        /**
		 * Threads condition variable. Used to notify a single thread when a new tasks is added to the queue,
		 * or notify all of them when the pool is stopped.
		 */
		std::condition_variable	poolCondVar;

        /**
		 * Boolean variable indicating if the pool is stopped (true) or not (false).
		 */
		bool					isStop = false;

        /**
		 * Vector of pool's threads. They are the same through the whole life time of the thread pool.
		 */
		std::vector<std::thread>	threads;

        /**
		 * Pending tasks queue. Whenever a thread execute a new task, it is removed from the queue.
		 */
		std::queue<Task>			pendingTasks;
	};
}

#include "ThreadPool.inl"
