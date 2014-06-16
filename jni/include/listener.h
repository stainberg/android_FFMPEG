/*
 * Listener.h
 *
 *  Created on: 2012-12-26
 *      Author: stainberg
 */

#ifndef LISTENER_H_
#define LISTENER_H_

#include <jni.h>
#include <pthread.h>
#include "mutex.h"
#include "refBase.h"

#ifdef __cplusplus
extern "C" {
#endif

enum msg_to_java {
	MSG_TEST,
	MSG_FILE_OPEN_OK,
	MSG_MOVIE_PREPARE_OK,
	MSG_PLAYING,
	MSG_PAUSE,
	MSG_BUFFERING,
	MSG_BUFFERED,
	MSG_STOP,
	MSG_RELEASE_MEDIAPLAYER,
	MSG_POSISION,
	MSG_DURATION,
	MSG_PLAY_COMPLETE,
	MSG_SEEK_ERROR,
	MSG_SEEK_START,
	MSG_SEEK_COMPLITION,
	MSG_ERROR,
	MSG_ERROR_FIXED,
};

class Listener {
public:
	Listener();
	Listener(JavaVM* vm, jobject cls, jobject weak_cls);
	virtual ~Listener();
	virtual void notify(pthread_t threadId, int msg, int arg0, const char* arg1);

private:
	My_Mutex mNotifyLock;
	JavaVM* mpjVM;
	jclass mClass;
	jobject mObject;
	jmethodID mMethod;
	pthread_t mjVMThread;
	const char* mListenerName;
};

#ifdef __cplusplus
 }
#endif

#endif /* LISTENER_H_ */
