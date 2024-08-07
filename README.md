# test-brpc-jemalloc

this is a test project from issue: https://github.com/apache/brpc/issues/2726

## mock_brpc
因为不想直接再编译一次brpc，以及它的相关依赖。这里我提供了一个`mock_brpc_mutex.cpp`

文件的内容基本是从：https://github.com/apache/brpc/blob/master/src/bthread/mutex.cpp
里拷贝出来的，为了方便说明，仅做了一些删减。

基本上只包含了pthread_mutex_trylock的相关代码。

## build
dockerfile文件夹下，有我使用的测试环境的dockerfile。
在这个镜像下，可以直接在mock_brpc文件夹下执行
```bash
bash buid.sh
```

之后会产出一个可执行文件`main_exe`

可以直接
```bash
gdb ./main_exe
```

此时进程会卡住，`control + c`后可以通过bt命令查看栈，

即复现了issue中的栈结构
```
#0  futex_wait (private=0, expected=1, futex_word=0x46e4d4 <bthread::init_sys_mutex_lock_once>) at ../sysdeps/nptl/futex-internal.h:141
#1  futex_wait_simple (private=0, expected=1, futex_word=0x46e4d4 <bthread::init_sys_mutex_lock_once>) at ../sysdeps/nptl/futex-internal.h:172
#2  __pthread_once_slow (once_control=0x46e4d4 <bthread::init_sys_mutex_lock_once>, init_routine=0x4025c0 <bthread::init_sys_mutex_lock()>) at pthread_once.c:105
#3  0x00000000004025a5 in bthread::first_sys_pthread_mutex_trylock (mutex=0x46e530 <init_lock+64>) at mock_brpc_mutex.cpp:18
#4  0x0000000000402675 in bthread::pthread_mutex_trylock_internal (mutex=0x46e530 <init_lock+64>) at mock_brpc_mutex.cpp:23
#5  0x0000000000402645 in bthread::pthread_mutex_trylock_impl<pthread_mutex_t> (mutex=0x46e530 <init_lock+64>) at mock_brpc_mutex.cpp:32
#6  0x0000000000402605 in pthread_mutex_trylock (__mutex=0x46e530 <init_lock+64>) at mock_brpc_mutex.cpp:42
#7  0x0000000000406349 in malloc_mutex_trylock_final (mutex=<optimized out>) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/include/jemalloc/internal/mutex.h:161
#8  malloc_mutex_lock (tsdn=0x0, mutex=<optimized out>) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/include/jemalloc/internal/mutex.h:220
#9  malloc_init_hard () at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:1739
#10 malloc_init () at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:223
#11 imalloc_init_check (sopts=<optimized out>, dopts=<optimized out>) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:2229
#12 imalloc (sopts=<optimized out>, dopts=<optimized out>) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:2260
#13 calloc (num=num@entry=1, size=size@entry=32) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:2494
#14 0x00007fa798908c05 in _dlerror_run (operate=operate@entry=0x7fa798908490 <dlsym_doit>, args=args@entry=0x7fff3c0b2ae0) at dlerror.c:148
#15 0x00007fa798908525 in __dlsym (handle=<optimized out>, name=0x45b004 "pthread_mutex_trylock") at dlsym.c:70
#16 0x00000000004025da in bthread::init_sys_mutex_lock () at mock_brpc_mutex.cpp:14
#17 0x00007fa79891e4df in __pthread_once_slow (once_control=0x46e4d4 <bthread::init_sys_mutex_lock_once>, init_routine=0x4025c0 <bthread::init_sys_mutex_lock()>) at pthread_once.c:116
#18 0x00000000004025a5 in bthread::first_sys_pthread_mutex_trylock (mutex=0x46e530 <init_lock+64>) at mock_brpc_mutex.cpp:18
#19 0x0000000000402675 in bthread::pthread_mutex_trylock_internal (mutex=0x46e530 <init_lock+64>) at mock_brpc_mutex.cpp:23
#20 0x0000000000402645 in bthread::pthread_mutex_trylock_impl<pthread_mutex_t> (mutex=0x46e530 <init_lock+64>) at mock_brpc_mutex.cpp:32
#21 0x0000000000402605 in pthread_mutex_trylock (__mutex=0x46e530 <init_lock+64>) at mock_brpc_mutex.cpp:42
#22 0x00000000004040a2 in malloc_mutex_trylock_final (mutex=<optimized out>) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/include/jemalloc/internal/mutex.h:161
#23 malloc_mutex_lock (tsdn=0x0, mutex=<optimized out>) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/include/jemalloc/internal/mutex.h:220
#24 malloc_init_hard () at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:1739
#25 malloc_init () at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:223
#26 imalloc_init_check (sopts=<optimized out>, dopts=<optimized out>) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:2229
#27 imalloc (sopts=<optimized out>, dopts=<optimized out>) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:2260
#28 je_malloc_default (size=72704) at /root/.conan/data/jemalloc/5.2.1/_/_/build/55d721bf422a34e3db4f17a58c2f8d839c0b6932/src/src/jemalloc.c:2289
#29 0x00007fa7987cba6a in ?? () from /lib/x86_64-linux-gnu/libstdc++.so.6
#30 0x00007fa798949b9a in call_init (l=<optimized out>, argc=argc@entry=1, argv=argv@entry=0x7fff3c0b2d88, env=env@entry=0x7fff3c0b2d98) at dl-init.c:72
#31 0x00007fa798949ca1 in call_init (env=0x7fff3c0b2d98, argv=0x7fff3c0b2d88, argc=1, l=<optimized out>) at dl-init.c:30
#32 _dl_init (main_map=0x7fa798967190, argc=1, argv=0x7fff3c0b2d88, env=0x7fff3c0b2d98) at dl-init.c:119
#33 0x00007fa79893913a in _dl_start_user () from /lib64/ld-linux-x86-64.so.2
#34 0x0000000000000001 in ?? ()
#35 0x00007fff3c0b389f in ?? ()
#36 0x0000000000000000 in ?? ()
```