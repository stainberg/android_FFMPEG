/*
 * Listener.cpp
 *
 *  Created on: 2012-12-26
 *      Author: stainberg
 */

#include "listener.h"
#include <stdlib.h>
#include <jni.h>
#include "myLog.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "listener.cpp"

Listener::Listener(JavaVM* vm, jobject thiz, jobject weak_cls) {
	mpjVM = vm;
	JNIEnv* env = NULL;
	mpjVM->AttachCurrentThread((JNIEnv**)&env, NULL);
	mjVMThread = pthread_self();
	mClass = NULL;
	mMethod = NULL;
	mObject = NULL;
	mListenerName = "postEventFromNative";
	jclass clazz = env->GetObjectClass(thiz);
	if(clazz == NULL) {
		return;
	}
	mClass =(jclass) env->NewGlobalRef(clazz);
	mObject = env->NewGlobalRef(weak_cls);
	mMethod = env->GetStaticMethodID(mClass, mListenerName, "(Ljava/lang/Object;IILjava/lang/String;)V");;
}

Listener::~Listener() {
	JNIEnv* env = NULL;
	mpjVM->AttachCurrentThread((JNIEnv**)&env, NULL);
	env->DeleteGlobalRef(mClass);
	env->DeleteGlobalRef(mObject);
	mClass = NULL;
	mObject = NULL;
	mMethod = NULL;
	LOGI("Listener::~Listener()");
}

void Listener::notify(pthread_t threadId, int msg, int arg0, const char* arg1) {
	AutoMutex l(mNotifyLock);
	JNIEnv* env = NULL;
	mpjVM->AttachCurrentThread((JNIEnv**)&env, NULL);
//	LOGD("threadId = %d, env = 0x%x", threadId, env);
	jstring strTmp = env->NewStringUTF(arg1);
	env->CallStaticVoidMethod(mClass, mMethod, mObject, msg, arg0, strTmp);
	env->DeleteLocalRef(strTmp);
	if(mjVMThread != threadId) {
		mpjVM->DetachCurrentThread();
	}
}
