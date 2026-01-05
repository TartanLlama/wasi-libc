#include <pthread.h>
#include <stdio.h>

void *thread_start(void *arg)
{
    static _Thread_local int x = 0;
    printf("TLS address: %p\n", (void *)&x);

    printf("Got: %d\n", (int)arg);
    return (void *)69;
}

int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_t thread;

    printf("Creating thread...\n");
    pthread_create(&thread, &attr, &thread_start, (void *)42);
    printf("Thread created!\n");

    void *ret;
    pthread_join(thread, &ret);

    pthread_create(&thread, &attr, &thread_start, (void *)42);
    printf("Returned: %d\n", (int)ret);
}