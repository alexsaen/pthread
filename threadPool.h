#ifndef THREADPOOL_H
#define THREADPOOL_H

class ThreadPoolImpl;

class ThreadPool {
public:
	typedef void(*ThreadFunc)(void*);

			ThreadPool(int threadCount);
			~ThreadPool();

	void	runTask(ThreadFunc f, void* arg);
	void	waitAll();

private:
	ThreadPoolImpl *impl;
};

extern ThreadPool defaultThreadPool;


#endif
