/**
* Copyright (C) 2008 Happy Fish / YuQing
*
* FastDFS may be copied only under the terms of the GNU General
* Public License V3, which may be found in the FastDFS source kit.
* Please visit the FastDFS Home Page http://www.fastken.com/ for more detail.
**/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include "pthread_func.h"
#include "logger.h"

int init_pthread_lock(pthread_mutex_t *pthread_lock)
{
	pthread_mutexattr_t mat;
	int result;

	if ((result=pthread_mutexattr_init(&mat)) != 0) {
		logError("file: "__FILE__", line: %d, "
			"call pthread_mutexattr_init fail, "
			"errno: %d, error info: %s",
			__LINE__, result, STRERROR(result));
		return result;
	}
	if ((result=pthread_mutexattr_settype(&mat,
			PTHREAD_MUTEX_ERRORCHECK)) != 0)
	{
		logError("file: "__FILE__", line: %d, "
			"call pthread_mutexattr_settype fail, "
			"errno: %d, error info: %s",
			__LINE__, result, STRERROR(result));
		return result;
	}
	if ((result=pthread_mutex_init(pthread_lock, &mat)) != 0) {
		logError("file: "__FILE__", line: %d, "
			"call pthread_mutex_init fail, "
			"errno: %d, error info: %s",
			__LINE__, result, STRERROR(result));
		return result;
	}
	if ((result=pthread_mutexattr_destroy(&mat)) != 0) {
		logError("file: "__FILE__", line: %d, "
			"call thread_mutexattr_destroy fail, "
			"errno: %d, error info: %s",
			__LINE__, result, STRERROR(result));
		return result;
	}

	return 0;
}

int init_pthread_attr(pthread_attr_t *pattr, const int stack_size)
{
	size_t old_stack_size;
	size_t new_stack_size;
	int result;

	if ((result=pthread_attr_init(pattr)) != 0) {
		logError("file: "__FILE__", line: %d, "
			"call pthread_attr_init fail, "
			"errno: %d, error info: %s",
			__LINE__, result, STRERROR(result));
		return result;
	}

	if ((result=pthread_attr_getstacksize(pattr, &old_stack_size)) != 0) {
		logError("file: "__FILE__", line: %d, "
			"call pthread_attr_getstacksize fail, "
			"errno: %d, error info: %s",
			__LINE__, result, STRERROR(result));
		return result;
	}

	if (stack_size > 0) {
		if (old_stack_size != stack_size) {
			new_stack_size = stack_size;
		} else {
			new_stack_size = 0;
		}
	} else if (old_stack_size < 1 * 1024 * 1024) {
		new_stack_size = 1 * 1024 * 1024;
	} else {
		new_stack_size = 0;
	}

	if (new_stack_size > 0) {
		if ((result=pthread_attr_setstacksize(pattr,
				new_stack_size)) != 0)
		{
			logError("file: "__FILE__", line: %d, "
				"call pthread_attr_setstacksize to %d fail, "
				"errno: %d, error info: %s", __LINE__,
                (int)new_stack_size, result, STRERROR(result));
			return result;
		}
	}

	if ((result=pthread_attr_setdetachstate(pattr,
			PTHREAD_CREATE_DETACHED)) != 0)
	{
		logError("file: "__FILE__", line: %d, "
			"call pthread_attr_setdetachstate fail, "
			"errno: %d, error info: %s",
			__LINE__, result, STRERROR(result));
		return result;
	}

	return 0;
}

int create_work_threads(int *count, void *(*start_func)(void *),
		void **args, pthread_t *tids, const int stack_size)
{
#define FIXED_TID_COUNT   256

	int result;
	pthread_attr_t thread_attr;
    void **current_arg;
    pthread_t fixed_tids[FIXED_TID_COUNT];
    pthread_t *the_tids;
	pthread_t *ptid;
	pthread_t *ptid_end;

	if ((result=init_pthread_attr(&thread_attr, stack_size)) != 0) {
		return result;
	}

    if (tids != NULL) {
        the_tids = tids;
    } else {
        if (*count <= FIXED_TID_COUNT) {
            the_tids = fixed_tids;
        } else {
            int bytes;
            bytes = sizeof(pthread_t) * *count;
            the_tids = (pthread_t *)malloc(bytes);
            if (the_tids == NULL) {
                logError("file: "__FILE__", line: %d, "
                        "malloc %d bytes fail", __LINE__, bytes);
                pthread_attr_destroy(&thread_attr);
                return ENOMEM;
            }
        }
    }

	result = 0;
	ptid_end = the_tids + (*count);
	for (ptid=the_tids,current_arg=args; ptid<ptid_end;
            ptid++,current_arg++)
    {
		if ((result=pthread_create(ptid, &thread_attr,
			start_func, *current_arg)) != 0)
		{
			*count = ptid - the_tids;
			logError("file: "__FILE__", line: %d, "
				"create threads #%d fail, "
				"errno: %d, error info: %s",
				__LINE__, *count,
				result, STRERROR(result));
			break;
		}
	}

    if (the_tids != tids && the_tids != fixed_tids) {
        free(the_tids);
    }

	pthread_attr_destroy(&thread_attr);
	return result;
}

int create_work_threads_ex(int *count, void *(*start_func)(void *),
		void *args, const int elment_size, pthread_t *tids,
        const int stack_size)
{
#define FIXED_ARG_COUNT   256

    void *fixed_args[FIXED_ARG_COUNT];
    void **pp_args;
    char *p;
    int result;
    int i;

    if (*count <= FIXED_ARG_COUNT) {
        pp_args = fixed_args;
    } else {
        int bytes;
        bytes = sizeof(void *) * (*count);
        pp_args = (void **)malloc(bytes);
        if (pp_args == NULL) {
            logError("file: "__FILE__", line: %d, "
                    "malloc %d bytes fail", __LINE__, bytes);
            return ENOMEM;
        }
    }

    p = (char *)args;
    for (i=0; i<*count; i++) {
        pp_args[i] = p;
        p += elment_size;
    }
    result = create_work_threads(count, start_func,
           pp_args, tids, stack_size);
    if (pp_args != fixed_args) {
        free(pp_args);
    }
    return result;
}

int kill_work_threads(pthread_t *tids, const int count)
{
	int result;
	pthread_t *ptid;
	pthread_t *ptid_end;

	ptid_end = tids + count;
	for (ptid=tids; ptid<ptid_end; ptid++) {
		if ((result=pthread_kill(*ptid, SIGINT)) != 0) {
			logError("file: "__FILE__", line: %d, "
				"kill thread failed, "
				"errno: %d, error info: %s",
				__LINE__, result, STRERROR(result));
		}
	}

	return 0;
}

int fc_create_thread(pthread_t *tid, void *(*start_func)(void *),
        void *args, const int stack_size)
{
	int result;
	pthread_attr_t thread_attr;

	if ((result=init_pthread_attr(&thread_attr, stack_size)) != 0) {
		return result;
	}

    if ((result=pthread_create(tid, &thread_attr, start_func, args)) != 0) {
        logError("file: "__FILE__", line: %d, "
                "create thread fail, "
                "errno: %d, error info: %s",
                __LINE__, result, STRERROR(result));
    }

	pthread_attr_destroy(&thread_attr);
	return result;
}
