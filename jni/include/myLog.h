/*
 * myLog.h
 *
 *  Created on: 2012-12-26
 *      Author: stainberg
 */

//#define _DEBUG_

#ifndef MYLOG_H_
#define MYLOG_H_

#include <android/log.h>

#define _DEBUG_

	#ifdef _DEBUG_
		#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,TAG,__VA_ARGS__)
		#define  LOGV(...)  __android_log_print(ANDROID_LOG_VERBOSE,TAG,__VA_ARGS__)
		#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,TAG,__VA_ARGS__)
	#else
		#define  LOGI(...)
		#define  LOGV(...)
		#define  LOGD(...)
	#endif

	#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,TAG,__VA_ARGS__)
	#define  LOGW(...)  __android_log_print(ANDROID_LOG_WARN,TAG,__VA_ARGS__)

#endif /* MYLOG_H_ */
