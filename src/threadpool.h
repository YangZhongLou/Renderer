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
		bool m_destroying = false;
		std::thread m_worker;
		std::queue<std::function<void()>> m_jobQueue;
		std::mutex m_queueMutex;
		std::condition_variable m_condition;

	private:
		// Loop through all remaining jobs
		void Loop()
		{
			while (true)
			{
				std::function<void()> job;
				{
					std::unique_lock<std::mutex> lock(m_queueMutex);
					m_condition.wait(lock, [this] { return !m_jobQueue.empty() || m_destroying; });
					if (m_destroying)
					{
						break;
					}
					job = m_jobQueue.front();
				}

				job();

				{
					std::lock_guard<std::mutex> lock(m_queueMutex);
					m_jobQueue.pop();
					m_condition.notify_one();
				}
			}
		}

	public:
		Thread()
		{
			m_worker = std::thread(&Thread::Loop, this);
		}

		~Thread()
		{
			if (m_worker.joinable())
			{
				Wait();
				m_queueMutex.lock();
				m_destroying = true;
				m_condition.notify_one();
				m_queueMutex.unlock();
				m_worker.join();
			}
		}

		// Add a new job to the thread's queue
		void AddJob(std::function<void()> function)
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_jobQueue.push(std::move(function));
			m_condition.notify_one();
		}

		// Wait until all work items have been finished
		void Wait()
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_condition.wait(lock, [this]() { return m_jobQueue.empty(); });
		}
	};

	class ThreadPool
	{
	private:
		std::vector<std::unique_ptr<Thread>> m_threads;


	public:
		ThreadPool(UInt32 count);
		~ThreadPool();
	public:
		/** Wait until all threads have finished their work items */
		void Wait()
		{
			for (auto & thread : m_threads)
			{
				thread->Wait();
			}
		}
	};
}
