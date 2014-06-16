/*
 * com_stainberg_MediaPlayer.h
 *
 *  Created on: 2012-12-26
 *      Author: stainberg
 */

#ifndef COM_STAINBEGRG_MEDIAPLAYER_H_
#define COM_STAINBEGRG_MEDIAPLAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

extern int registerMethods(JNIEnv* env);
int jniThrowException(JNIEnv* env, const char* className, const char* msg);
int jniRegisterNativeMethods(JNIEnv* env, const char* className, const JNINativeMethod* methods, int count);

#ifdef __cplusplus
}
#endif

#endif /* COM_STAINBEGRG_MEDIAPLAYER_H_ */
