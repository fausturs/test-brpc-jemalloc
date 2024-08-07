#include <iostream>
#include <pthread.h>

extern "C" {
int pthread_mutex_trylock(pthread_mutex_t* __mutex);
}

int main() {
    int * p = (int*)malloc(sizeof(int));
    std::cout << (intptr_t)p << std::endl;
    std::cout << (intptr_t)pthread_mutex_trylock << std::endl;
    return 0;
}