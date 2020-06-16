#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <inttypes.h>
#include <sys/time.h>
#include "fastcommon/logger.h"
#include "fastcommon/shared_func.h"
#include "fastcommon/sched_thread.h"
#include "fastcommon/pthread_func.h"
#include "fastcommon/ini_file_reader.h"
#include "fastcommon/fast_allocator.h"

#define LOOP_COUNT (100 * 1000 * 1000)
#define barrier()  __asm__ __volatile__("" ::: "memory")

static volatile int counter = 0;

#define THREAD_COUNT 9
pthread_t tids[THREAD_COUNT];

void *thread2_func(void *args)
{
    int i;
    for (i=0; i<LOOP_COUNT; i++) {
        __sync_add_and_fetch(&counter, 1);
    }
    return NULL;
}

void *thread1_func(void *args)
{
    int i;
    for (i=0; i<LOOP_COUNT; i++) {
        __sync_sub_and_fetch(&counter, 1);
    }
    return NULL;
}

void *wait_thread_func(void *args)
{
    int i;
    for (i=0; i<LOOP_COUNT; i++) {
        __sync_add_and_fetch(&counter, 0);
    }
    return NULL;
}


int test()
{
    int result;
    int i;
    pthread_t *tid;

    tid = tids;
    for (i=0; i<THREAD_COUNT / 2; i++) {
        if ((result=pthread_create(tid++, NULL, thread1_func, NULL)) != 0) {
            logError("file: "__FILE__", line: %d, "
                    "create thread fail, "
                    "errno: %d, error info: %s",
                    __LINE__, result, STRERROR(result));
            return result;
        }
    }

    for (i=0; i<THREAD_COUNT / 2; i++) {
        if ((result=pthread_create(tid++, NULL, thread2_func, NULL)) != 0) {
            logError("file: "__FILE__", line: %d, "
                    "create thread fail, "
                    "errno: %d, error info: %s",
                    __LINE__, result, STRERROR(result));
            return result;
        }
    }

    if ((result=pthread_create(tid++, NULL, wait_thread_func, NULL)) != 0) {
        logError("file: "__FILE__", line: %d, "
                "create thread fail, "
                "errno: %d, error info: %s",
                __LINE__, result, STRERROR(result));
        return result;
    }

    for (i=0; i<THREAD_COUNT; i++) {
        pthread_join(tids[i], NULL);
    }
    return 0;
}

int main(int argc, char *argv[])
{
	int result;
	int64_t start_time;

	log_init();
	srand(time(NULL));
	g_log_context.log_level = LOG_DEBUG;
	
	start_time = get_current_time_ms();
    result = test();
	printf("counter: %d, time used: %"PRId64" ms\n",
            __sync_add_and_fetch(&counter, 0),
            get_current_time_ms() - start_time);

	return result;
}
