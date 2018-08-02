#ifndef PTHREAD_H
#define PTHREAD_H

#include <windows.h>
#include <setjmp.h>
#include <errno.h>
#include <sys/timeb.h>

/* Windows doesn't have this, so declare it ourselves. */
struct timespec
{
	/* long long in windows is the same as long in unix for 64bit */
	long long tv_sec;
	long long tv_nsec;
};

typedef struct _pthread_cleanup _pthread_cleanup;
struct _pthread_cleanup
{
	void (*func)(void *);
	void *arg;
	_pthread_cleanup *next;
};

struct _pthread_v
{
	void *ret_arg;
	void *(* func)(void *);
	_pthread_cleanup *clean;
	HANDLE h;
	int cancelled;
	unsigned p_state;
	int keymax;
	void **keyval;

	jmp_buf jb;
};

typedef struct _pthread_v *pthread_t;

typedef struct pthread_barrier_t pthread_barrier_t;
struct pthread_barrier_t
{
	int count;
	int total;
	CRITICAL_SECTION m;
	CONDITION_VARIABLE cv;
};

typedef struct pthread_attr_t pthread_attr_t;
struct pthread_attr_t
{
	unsigned p_state;
	void *stack;
	size_t s_size;
};

typedef long pthread_once_t;
typedef unsigned pthread_mutexattr_t;
typedef SRWLOCK pthread_rwlock_t;
typedef CRITICAL_SECTION pthread_mutex_t;
typedef unsigned pthread_key_t;
typedef void *pthread_barrierattr_t;
typedef long pthread_spinlock_t;
typedef int pthread_condattr_t;
typedef CONDITION_VARIABLE pthread_cond_t;
typedef int pthread_rwlockattr_t;

#ifdef __cplusplus
extern "C" {
#endif

int pthread_create(pthread_t *th, pthread_attr_t *attr, void *(* func)(void *), void *arg);
int pthread_join(pthread_t t, void **res);
int pthread_detach(pthread_t t);
int pthread_exit(void *res);
int pthread_cancel(pthread_t t);
void pthread_testcancel(void);

int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);

pthread_t pthread_self(void);
int pthread_once(pthread_once_t *o, void (*func)(void));
int pthread_equal(pthread_t t1, pthread_t t2);

int pthread_create_wrapper(void *args);

int pthread_get_concurrency(int *val);
int pthread_set_concurrency(int val);

int pthread_key_delete(pthread_key_t key);

void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *value);

int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setdetachstate(pthread_attr_t *a, int flag);
int pthread_attr_getdetachstate(pthread_attr_t *a, int *flag);
int pthread_attr_setinheritsched(pthread_attr_t *a, int flag);
int pthread_attr_getinheritsched(pthread_attr_t *a, int *flag);
int pthread_attr_setscope(pthread_attr_t *a, int flag);
int pthread_attr_getscope(pthread_attr_t *a, int *flag);
int pthread_attr_getstackaddr(pthread_attr_t *attr, void **stack);
int pthread_attr_setstackaddr(pthread_attr_t *attr, void *stack);
int pthread_attr_getstacksize(pthread_attr_t *attr, size_t *size);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t size);

int pthread_mutex_init(pthread_mutex_t *m, pthread_mutexattr_t *a);
int pthread_mutex_destroy(pthread_mutex_t *m);
int pthread_mutex_timedlock(pthread_mutex_t *m, struct timespec *ts);
int pthread_mutex_lock(pthread_mutex_t *m);
int pthread_mutex_unlock(pthread_mutex_t *m);
int pthread_mutex_trylock(pthread_mutex_t *m);

int pthread_mutexattr_init(pthread_mutexattr_t *a);
int pthread_mutexattr_destroy(pthread_mutexattr_t *a);
int pthread_mutexattr_gettype(pthread_mutexattr_t *a, int *type);
int pthread_mutexattr_settype(pthread_mutexattr_t *a, int type);
int pthread_mutexattr_getpshared(pthread_mutexattr_t *a, int *type);
int pthread_mutexattr_setpshared(pthread_mutexattr_t * a, int type);
int pthread_mutexattr_getprotocol(pthread_mutexattr_t *a, int *type);
int pthread_mutexattr_setprotocol(pthread_mutexattr_t *a, int type);
int pthread_mutexattr_getprioceiling(pthread_mutexattr_t *a, int * prio);
int pthread_mutexattr_setprioceiling(pthread_mutexattr_t *a, int prio);

int pthread_cond_init(pthread_cond_t *c, pthread_condattr_t *a);
int pthread_cond_signal(pthread_cond_t *c);
int pthread_cond_broadcast(pthread_cond_t *c);
int pthread_cond_wait(pthread_cond_t *c, pthread_mutex_t *m);
int pthread_cond_destroy(pthread_cond_t *c);
int pthread_cond_timedwait(pthread_cond_t *c, pthread_mutex_t *m, struct timespec *t);

int pthread_condattr_init(pthread_condattr_t *a);
int pthread_condattr_destroy(pthread_condattr_t *a);
int pthread_condattr_getpshared(pthread_condattr_t *a, int *s);
int pthread_condattr_setpshared(pthread_condattr_t *a, int s);

/*
int pthread_rwlock_init(pthread_rwlock_t *l, pthread_rwlockattr_t *a);
int pthread_rwlock_destroy(pthread_rwlock_t *l);
int pthread_rwlock_rdlock(pthread_rwlock_t *l);
int pthread_rwlock_wrlock(pthread_rwlock_t *l);
int pthread_rwlock_unlock(pthread_rwlock_t *l);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *l);
int pthread_rwlock_trywrlock(pthread_rwlock_t *l);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *l, const struct timespec *ts);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *l, const struct timespec *ts);

int pthread_rwlockattr_init(pthread_rwlockattr_t *a);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *a);
int pthread_rwlockattr_getpshared(pthread_rwlockattr_t *a, int *s);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *a, int s);
*/

int pthread_barrier_init(pthread_barrier_t *b, void *attr, int count);
int pthread_barrier_destroy(pthread_barrier_t *b);
int pthread_barrier_wait(pthread_barrier_t *b);

int pthread_barrierattr_init(void **attr);
int pthread_barrierattr_destroy(void **attr);
int pthread_barrierattr_setpshared(void **attr, int s);
int pthread_barrierattr_getpshared(void **attr, int *s);
int pthread_key_create(pthread_key_t *key, void (* dest)(void *));

int pthread_spin_init(pthread_spinlock_t *l, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *l);
int pthread_spin_lock(pthread_spinlock_t *l);
int pthread_spin_trylock(pthread_spinlock_t *l);
int pthread_spin_unlock(pthread_spinlock_t *l);

#define pthread_mutex_getprioceiling(M, P) ENOTSUP
#define pthread_mutex_setprioceiling(M, P) ENOTSUP

#define pthread_getschedparam(T, P, S) ENOTSUP
#define pthread_setschedparam(T, P, S) ENOTSUP
#define pthread_getcpuclockid(T, C) ENOTSUP

#define pthread_attr_getguardsize(A, S) ENOTSUP
#define pthread_attr_setgaurdsize(A, S) ENOTSUP
#define pthread_attr_getschedparam(A, S) ENOTSUP
#define pthread_attr_setschedparam(A, S) ENOTSUP
#define pthread_attr_getschedpolicy(A, S) ENOTSUP
#define pthread_attr_setschedpolicy(A, S) ENOTSUP

#define pthread_condattr_getclock(A, C) ENOTSUP
#define pthread_condattr_setclock(A, C) ENOTSUP

/* No fork() in windows - so ignore this */
#define pthread_atfork(F1,F2,F3) 0

/* Windows has rudimentary signals support */
#define pthread_kill(T, S) 0
#define pthread_sigmask(H, S1, S2) 0


#ifdef __cplusplus
}
#endif

#endif
