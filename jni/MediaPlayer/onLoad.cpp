/*
 * onLoad.cpp
 *
 *  Created on: 2012-12-26
 *      Author: stainberg
 */
#include <assert.h>
#include "onLoad.h"
#include "myLog.h"
#include "com_stainberg_MediaPlayer_MediaPlayer.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "onLoad.cpp"

OnLoad::OnLoad(JavaVM* vm) {
	jVM = vm;
	mListener = NULL;
	mClassPathName = "com/stainberg/MediaPlayer/MediaPlayer";
}

OnLoad::~OnLoad() {
	jVM = NULL;
	delete mListener;
	mListener = NULL;
	LOGI("OnLoad::~OnLoad");
}

JavaVM* OnLoad::getJVM() {
	return jVM;
}

const char*  OnLoad::getClassName() {
	return mClassPathName;
}

Listener*  OnLoad::getListener() {
	return mListener;
}

void OnLoad::setListener(JavaVM* vm, jobject thiz, jobject weak_thiz) {
	if(mListener == NULL) {
		mListener = new Listener(vm, thiz, weak_thiz);
	}
}

jint JNI_OnLoad(JavaVM* vm, void* reserved) {//JNI main
	JNIEnv* env = NULL;
	jint ret = JNI_ERR;
	if(vm->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
		LOGE("GetEnv failed!");
		return ret;
	}
	assert(env != NULL);
	if(registerMethods(env) != JNI_OK) {
		LOGE("can not load methods!");
		return ret;
	}
	ret = JNI_VERSION_1_4;
	LOGI("Loaded!");
	return ret;
}

