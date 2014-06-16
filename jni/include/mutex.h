/*
 * mutex.h
 *
 *  Created on: 2012-12-28
 *      Author: stainberg
 */

#ifndef MUTEX_H_
#define MUTEX_H_
#include "android/Errors.h"
#include <pthread.h>
#include <MyLog.h>

#undef _DEBUG_

#ifdef TAG
#undef TAG
#endif
#define TAG "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

using namespace android;

class My_Mutex {
public:
	My_Mutex();
	virtual ~My_Mutex();
	status_t lock();
	void unlock();

	class AutoLock {
	public:
		inline AutoLock(My_Mutex& mutex):mLock(mutex) {mLock.lock();
#ifdef _DEBUG_
		LOGD("lock");
#endif
		}
		inline AutoLock(My_Mutex* mutex):mLock(*mutex) {mLock.lock();
#ifdef _DEBUG_
		LOGD("lock");
#endif
		}
		inline ~AutoLock() {mLock.unlock();
#ifdef _DEBUG_
		LOGD("unlock");
#endif
		}

	private:
		My_Mutex& mLock;
	};

private:
	pthread_mutex_t mMutex;
};

inline My_Mutex::My_Mutex() {
	pthread_mutex_init(&mMutex, NULL);
}

inline My_Mutex::~My_Mutex() {
	pthread_mutex_destroy(&mMutex);
}

inline status_t My_Mutex::lock() {
	return pthread_mutex_lock(&mMutex);
}

inline void My_Mutex::unlock() {
	pthread_mutex_unlock(&mMutex);
}

typedef My_Mutex::AutoLock AutoMutex;

#ifdef __cplusplus
}
#endif


#endif /* MUTEX_H_ */
