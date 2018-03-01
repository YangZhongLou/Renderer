#include "threadpool.h"

namespace Concise
{
	ThreadPool::ThreadPool(UInt32 count)
	{
		m_threads.clear();
		for (auto i = 0; i < count; i++)
		{
			m_threads.push_back(std::make_unique<Thread>());
		}
	}

	ThreadPool::~ThreadPool()
	{
	}
}