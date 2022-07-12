#pragma once
#include <future>

namespace Core
{
	template<class Callable, class ...Args>
	auto ThreadPool::AddTask(Callable&& function, Args&& ...args)
	{
		auto task = std::bind(std::forward<Callable>(function), std::forward<Args>(args)...);

		auto wrapper = std::make_shared<std::packaged_task<decltype(task()) ()>>(task);

		{
			std::lock_guard<std::mutex> lock{ poolMutex };
			pendingTasks.emplace([wrapper] { (*wrapper)(); });
		}

		poolCondVar.notify_one();
		return (*wrapper).get_future();
	}
}
