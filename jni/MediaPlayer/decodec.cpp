/*
 * decodec.cpp
 *
 *  Created on: 2013-1-3
 *      Author: stainberg
 */
#include "decodec.h"
#include "myLog.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "decodec.cpp"

Decodec::Decodec() {
	mInterrupt = 1;
}

Decodec::~Decodec() {

}

void Decodec::start() {
	mInterrupt = 0;
	pthread_create(&mThreadId, NULL, decodecProcess, this);
	return;
}

void Decodec::stop() {
	if(mInterrupt == 1) {
		return;
	}
	do {
	} while(android_atomic_cmpxchg(mInterrupt, !mInterrupt, &mInterrupt));
	return;
}

int Decodec::getThreadStatusInterrupt() {
	return mInterrupt;
}

int Decodec::getThreadExitFlag() {
//	LOGI("mThreadExited = %d", mThreadExited);
	return mThreadExited;
}

void* Decodec::decodecProcess(void* ptr) {
	Decodec* dec = (Decodec*)ptr;
	dec->mThreadExited = 0;
	dec->decode(ptr);
	return 0;
}
