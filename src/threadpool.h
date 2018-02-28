/**
* Author ZhongLou Yang, 2018-2-28
*/

#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include "types.h"

namespace Concise
{
	/** 
	* Modified from https://github.com/SaschaWillems/Vulkan.git
	*/
	class Thread
	{
	private:
		bool destroying = false;
		std::thread worker;
		std::queue<std::function<void()>> jobQueue;
		std::mutex queueMutex;
		std::condition_variable condition;

		// Loop through all remaining jobs
		void Loop()
		{
			while (true)
			{
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> lock(queueMutex);
					condition.wait(lock, [this] { return !jobQueue.empty() || destroying; });
					if (destroying)
					{
						break;
					}
					job = jobQueue.front();
				}

				job();

				{
					std::lock_guard<std::mutex> lock(queueMutex);
					jobQueue.pop();
					condition.notify_one();
				}
			}
		}

	public:
		Thread()
		{
			worker = std::thread(&Thread::Loop, this);
		}

		~Thread()
		{
			if (worker.joinable())
			{
				Wait();
				queueMutex.lock();
				destroying = true;
				condition.notify_one();
				queueMutex.unlock();
				worker.join();
			}
		}

		// Add a new job to the thread's queue
		void AddJob(std::function<void()> function)
		{
			std::lock_guard<std::mutex> lock(queueMutex);
			jobQueue.push(std::move(function));
			condition.notify_one();
		}

		// Wait until all work items have been finished
		void Wait()
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			condition.wait(lock, [this]() { return jobQueue.empty(); });
		}
	};

	class ThreadPool
	{
	private:
		std::vector<std::unique_ptr<Thread>> m_threads;

		void SetPoolSize(UInt32 count)
		{
			m_threads.clear();
			for (auto i = 0; i < count; i++)
			{
				m_threads.push_back(std::make_unique<Thread>());
			}
		}

		// Wait until all m_threads have finished their work items
		void Wait()
		{
			for (auto & thread : m_threads)
			{
				thread->Wait();
			}
		}
	};
}
