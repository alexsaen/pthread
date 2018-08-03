#include "threadPool.h"
#include "pthread.h"
#include <vector>
#include <assert.h>

class ThreadPoolImpl {
public:
	typedef void(*ThreadFunc)(void*);

	ThreadPoolImpl(int threadCount);
	~ThreadPoolImpl();

	void	runTask(ThreadFunc f, void* arg);
	void	waitAll();

private:
	volatile int	threadsRunning;
	int		startIdx;

	pthread_mutex_t	pmutex;
	pthread_cond_t	pcond;

	class Thread {
		ThreadPoolImpl	*parent;

		enum State {	TS_WAITING, TS_RUNNING, TS_FINISHED		};

		volatile State	state;

		pthread_t	thread;
		pthread_mutex_t	mutex;
		pthread_cond_t	cond;
		ThreadFunc	currentTask;
		void*		taskArg;

		static	void*	threadFunc(void* arg);
	public:
		Thread();
		~Thread();
		void	init(ThreadPoolImpl *p);
		void	run();
		bool	runTask(ThreadFunc task, void * arg);

		bool	isWaiting() {	return state == TS_WAITING;	}

	};
	std::vector<Thread>	threads;

	void	taskFinished(ThreadFunc task);
};

ThreadPoolImpl::ThreadPoolImpl(int threadCount): threads(threadCount), startIdx(0) {
	pthread_mutex_init(&pmutex, 0);
	pthread_cond_init(&pcond, 0);

	for(size_t i=0; i<threads.size(); ++i)	// init threads
		threads[i].init(this);

	threadsRunning = 0;
}

ThreadPoolImpl::~ThreadPoolImpl() {
	pthread_mutex_destroy(&pmutex);
	pthread_cond_destroy(&pcond);
}

void ThreadPoolImpl::runTask(ThreadFunc task, void* data) {

	pthread_mutex_lock(&pmutex);					// semaphore wait
	while(threadsRunning==threads.size())
		pthread_cond_wait(&pcond, &pmutex);
	threadsRunning++;
	pthread_mutex_unlock(&pmutex);

	for(int i=0; i<threads.size(); ++i) {
		int idx = (i+startIdx) % threads.size();
		if(threads[idx].runTask(task, data)) {
			startIdx = (startIdx+1) % threads.size();
			return;
		}
	}

	assert(false);
}

void ThreadPoolImpl::taskFinished(ThreadFunc task) {
	pthread_mutex_lock(&pmutex);					// semaphore post
	threadsRunning--;
	pthread_cond_signal(&pcond);
	pthread_mutex_unlock(&pmutex);
}

void ThreadPoolImpl::waitAll() {

//	for(int i=0; i< threads.size(); ++i)
//		printf("%d ", threads[i].isWaiting());
//	printf("       \n");

	pthread_mutex_lock(&pmutex);
	while(threadsRunning!=0)
		pthread_cond_wait(&pcond, &pmutex);
	pthread_mutex_unlock(&pmutex);
}

void* ThreadPoolImpl::Thread::threadFunc(void* arg) {
	Thread *t = (Thread*)arg;
	t->run();
	return 0;
}

ThreadPoolImpl::Thread::Thread(): parent(0), state(TS_FINISHED)	{}

void ThreadPoolImpl::Thread::init(ThreadPoolImpl *p) {
	parent = p;
	state = TS_WAITING;
	pthread_mutex_init(&mutex, 0);
	pthread_cond_init(&cond, 0);
	pthread_create(&thread, 0, threadFunc, this);
}

ThreadPoolImpl::Thread::~Thread()	{
	if(!parent)
		return;

	pthread_mutex_lock(&mutex);
	state = TS_FINISHED;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);

	pthread_join(thread, 0);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
}


void ThreadPoolImpl::Thread::run() {
	int rc = pthread_mutex_lock(&mutex);

	for(;;) {
		rc = pthread_cond_wait(&cond, &mutex);

		if(state == TS_FINISHED)				// special case for finish
			break;

		currentTask(taskArg);

		currentTask = 0;
		state = TS_WAITING;

		parent->taskFinished(currentTask);		// inform parent what tread is freed
	}

	rc = pthread_mutex_unlock(&mutex);
}

bool ThreadPoolImpl::Thread::runTask(ThreadFunc task, void* arg) {
	if(state != TS_WAITING) 
		return false;

	int rc = pthread_mutex_lock(&mutex);

	currentTask = task;
	taskArg = arg;
	state = TS_RUNNING;

	rc = pthread_cond_signal(&cond);			// run task
	rc = pthread_mutex_unlock(&mutex);

	return true;
}

ThreadPool::ThreadPool(int threadCount) {
	impl = new ThreadPoolImpl(threadCount);
}

ThreadPool::~ThreadPool() {
	delete impl;
}

void ThreadPool::runTask(ThreadFunc f, void* arg) {
	impl->runTask(f, arg);
}

void ThreadPool::waitAll() {
	impl->waitAll();
}

ThreadPool defaultThreadPool(4);

