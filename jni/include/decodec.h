/*
 * decodec.h
 *
 *  Created on: 2013-1-3
 *      Author: stainberg
 */

#ifndef DECODEC_H_
#define DECODEC_H_

#include "mutex.h"
#include "refBase.h"
#include <pthread.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

class Decodec: public My_RefBase<Decodec> {
public:
	Decodec();
	virtual ~Decodec();
	void start();
	void stop();
	int getThreadExitFlag();
	inline pthread_t getThreadId() {return mThreadId;};
protected:
	virtual void* decode(void* ptr){return 0;};
	int getThreadStatusInterrupt();

private:
	static void* decodecProcess(void* ptr);

protected:
	mutable volatile int mThreadExited;
	mutable volatile int mInterrupt;
	My_Mutex mMutex;
	pthread_t mThreadId;
};

#ifdef __cplusplus
}
#endif

#endif /* DECODEC_H_ */
