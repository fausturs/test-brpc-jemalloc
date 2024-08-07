
#include <pthread.h>
#include <dlfcn.h>                               // dlsym

namespace bthread {

typedef int (*MutexOp)(pthread_mutex_t*);
int first_sys_pthread_mutex_trylock(pthread_mutex_t* mutex);
static MutexOp sys_pthread_mutex_trylock = first_sys_pthread_mutex_trylock;
static pthread_once_t init_sys_mutex_lock_once = PTHREAD_ONCE_INIT;

static void init_sys_mutex_lock() {
    // In some system, _dl_sym may cause symbol lookup error: undefined symbol: pthread_mutex_trylock.
    sys_pthread_mutex_trylock = (MutexOp)dlsym(RTLD_NEXT, "pthread_mutex_trylock");
}

int first_sys_pthread_mutex_trylock(pthread_mutex_t* mutex) {
    pthread_once(&init_sys_mutex_lock_once, init_sys_mutex_lock);
    return sys_pthread_mutex_trylock(mutex);
}

inline int pthread_mutex_trylock_internal(pthread_mutex_t* mutex) {
    int rc = sys_pthread_mutex_trylock(mutex);
    // if (0 == rc) {
    //     ++tls_pthread_lock_count;
    // }
    return rc;
}

template <typename Mutex>
inline int pthread_mutex_trylock_impl(Mutex* mutex) {
    return pthread_mutex_trylock_internal(mutex);
}


} // namespace bthread


extern "C" {

int pthread_mutex_trylock(pthread_mutex_t* __mutex) {
    return bthread::pthread_mutex_trylock_impl(__mutex);
}

}  // extern "C"