/*
 * com_stainberg_MediaPlayer.c
 *
 *  Created on: 2012-12-26
 *      Author: stainberg
 */

#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer
#include "listener.h"
#include "mediaPlayer.h"
#include "onLoad.h"
#include "myLog.h"
#include "com_stainberg_MediaPlayer_MediaPlayer.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "com_stainberg_MediaPlayer_MediaPlayer.cpp"

static OnLoad* spLoad = NULL;
static MediaPlayer* spMediaPlayer = NULL;
static ANativeWindow* spWindow = NULL;
static jstring result = NULL;

static void
com_stainberg_MediaPlayer_MediaPlayer_init(JNIEnv* env, jobject thiz) {
	if(spMediaPlayer != NULL) {
		spMediaPlayer->init();
	}
}

static void
com_stainberg_MediaPlayer_MediaPlayer_finalize(JNIEnv* env, jobject thiz) {
	if(spMediaPlayer != NULL) {
		delete spMediaPlayer;
		spMediaPlayer = NULL;
	}
	if(spLoad != NULL) {
		delete spLoad;
		spLoad = NULL;
	}
	if(spWindow != NULL) {
		ANativeWindow_release(spWindow);
		spWindow = NULL;
	}
}

static void
com_stainberg_MediaPlayer_MediaPlayer_setup(JNIEnv* env, jobject thiz, jobject weak_thiz) {
	if(spLoad != NULL) {
		spLoad->setListener(spLoad->getJVM(), thiz, weak_thiz);
		spMediaPlayer = new MediaPlayer(spLoad);
	}
}

static void
com_stainberg_MediaPlayer_MediaPlayer_setDataSource(JNIEnv* env, jobject thiz, jstring filePath) {
	if(filePath != NULL) {
		const char *pathStr = env->GetStringUTFChars(filePath, NULL);
		if(spMediaPlayer != NULL) {
			spMediaPlayer->setDataSource(pathStr);
		}
		env->ReleaseStringUTFChars(filePath, pathStr);
	}

}

static void
com_stainberg_MediaPlayer_MediaPlayer_prepare(JNIEnv* env, jobject thiz) {
	if(spMediaPlayer != NULL) {
		spMediaPlayer->prepare();
	}
}

static void
com_stainberg_MediaPlayer_MediaPlayer_start(JNIEnv* env, jobject thiz) {
	if(spMediaPlayer != NULL) {
		spMediaPlayer->start();
	}
}

static void
com_stainberg_MediaPlayer_MediaPlayer_stop(JNIEnv* env, jobject thiz) {
	if(spMediaPlayer != NULL) {
		spMediaPlayer->stop();
	}
}

static void
com_stainberg_MediaPlayer_MediaPlayer_pause(JNIEnv* env, jobject thiz) {
	if(spMediaPlayer != NULL) {
		spMediaPlayer->pause();
	}
}

static void
com_stainberg_MediaPlayer_MediaPlayer_seekTo(JNIEnv* env, jobject thiz, double position) {
	if(spMediaPlayer != NULL) {
		spMediaPlayer->seekTo(position);
	}
}

static double
com_stainberg_MediaPlayer_MediaPlayer_getDuration(JNIEnv* env, jobject thiz) {
	if(spMediaPlayer != NULL) {
		return spMediaPlayer->getDuration();
	}
	return 0;
}

static double
com_stainberg_MediaPlayer_MediaPlayer_getCurrentPosition(JNIEnv* env, jobject thiz) {
	if(spMediaPlayer != NULL) {
		return spMediaPlayer->getPosition();
	}
	return 0;
}

static jboolean
com_stainberg_MediaPlayer_MediaPlayer_isPlaying(JNIEnv* env, jobject thiz) {
	if(spMediaPlayer != NULL) {
		return spMediaPlayer->isPlaying();
	}
	return false;
}

static void
com_stainberg_MediaPlayer_MediaPlayer_setSurface(JNIEnv* env, jobject thiz, jobject surface) {
//	if(surface != NULL) {
//		if(spWindow != NULL) {
//			ANativeWindow_release(spWindow);
//			spWindow = NULL;
//		}
//		spWindow = ANativeWindow_fromSurface(env, surface);
//		if(spMediaPlayer != NULL) {
//			spMediaPlayer->initSurface(spWindow);
//		}
//	}
	if(spWindow != NULL) {
		ANativeWindow_release(spWindow);
		spWindow = NULL;
	}
	if(spMediaPlayer != NULL) {
		spMediaPlayer->initSurface(NULL);
	}
	if(surface != NULL) {
		spWindow = ANativeWindow_fromSurface(env, surface);
		if(spMediaPlayer != NULL) {
			spMediaPlayer->initSurface(spWindow);
		}
	}
}

static jstring
com_stainberg_MediaPlayer_MediaPlayer_getVersion(JNIEnv* env, jobject thiz) {
	const char* const version = "release 3.0.1";
	result = env->NewStringUTF(version);
	return result;
}

int jniThrowException(const char* className, const char* msg) {
	int ret = -1;
	JNIEnv* Env = NULL;
	spLoad->getJVM()->AttachCurrentThread((JNIEnv**)&Env, NULL);
	if(Env == NULL) {
		return ret;
	}
	jclass exceptionClass = Env->FindClass(className);
	if(exceptionClass == NULL) {
		return ret;
	}
	if(Env->ThrowNew(exceptionClass, msg) != JNI_OK) {
		return ret;
	}
	return 0;
}

int jniRegisterNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* methods, int count) {
	jclass clazz;
	int ret = -1;
	clazz = env->FindClass(className);
	if(clazz == NULL) {
		return ret;
	}
	if((ret = env->RegisterNatives(clazz,methods, count)) < 0) {
		return ret;
	}
	return ret;
}

static JNINativeMethod mMethods[] = {//method for JAVA. use this to register native method
		{"native_init", "()V", (void*) com_stainberg_MediaPlayer_MediaPlayer_init},
		{"native_setup", "(Ljava/lang/Object;)V", (void*)com_stainberg_MediaPlayer_MediaPlayer_setup},
		{"native_finalize", "()V", (void*) com_stainberg_MediaPlayer_MediaPlayer_finalize},
		{"setDataSource", "(Ljava/lang/String;)V", (void*)com_stainberg_MediaPlayer_MediaPlayer_setDataSource},
		{"_prepare", "()V", (void*)com_stainberg_MediaPlayer_MediaPlayer_prepare},
		{"_start", "()V", (void*)com_stainberg_MediaPlayer_MediaPlayer_start},
		{"_stop", "()V", (void*)com_stainberg_MediaPlayer_MediaPlayer_stop},
		{"_pause", "()V", (void*)com_stainberg_MediaPlayer_MediaPlayer_pause},
		{"_seekTo", "(D)V", (void*)com_stainberg_MediaPlayer_MediaPlayer_seekTo},
		{"_getDuration", "()D", (void*)com_stainberg_MediaPlayer_MediaPlayer_getDuration},
		{"_getCurrentPosition", "()D", (void*)com_stainberg_MediaPlayer_MediaPlayer_getCurrentPosition},
		{"_isPlaying", "()Z", (void*)com_stainberg_MediaPlayer_MediaPlayer_isPlaying},
		{"_setSurface", "(Ljava/lang/Object;)V", (void*)com_stainberg_MediaPlayer_MediaPlayer_setSurface},
		{"getVersion", "()Ljava/lang/String;", (void*) com_stainberg_MediaPlayer_MediaPlayer_getVersion},//new method. return the version.
};

int registerMethods(JNIEnv* env) {
	JavaVM* jVM;
	env->GetJavaVM((JavaVM**)&jVM);
	if(spLoad != NULL) {
		delete spLoad;
	}
	spLoad = new OnLoad(jVM);
	return jniRegisterNativeMethods(env, spLoad->getClassName(), mMethods, sizeof(mMethods)/sizeof(mMethods[0]));
}
