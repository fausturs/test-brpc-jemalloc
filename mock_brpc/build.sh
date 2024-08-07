
/usr/bin/clang++-8 -c -g -o mock_brpc_mutex.o mock_brpc_mutex.cpp
ar rcs libmock_brpc_mutex.a mock_brpc_mutex.o

/usr/bin/clang++-8 -c -g -o main.o main.cpp

/usr/bin/clang++-8 \
    -o main_exe \
    main.o \
    ./libmock_brpc_mutex.a \
    /root/.conan/data/jemalloc/5.2.1/_/_/package/55d721bf422a34e3db4f17a58c2f8d839c0b6932/lib/libjemalloc.a \
    -lpthread \
    -ldl
