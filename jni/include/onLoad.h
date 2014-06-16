/*
 * onLoad.h
 *
 *  Created on: 2012-12-26
 *      Author: stainberg
 */

#ifndef ONLOAD_H_
#define ONLOAD_H_

#include <stdlib.h>
#include <jni.h>
#include "smartpointer.h"
#include "listener.h"

#ifdef __cplusplus
extern "C" {
#endif

class OnLoad {
public:
	OnLoad(JavaVM* vm);
	virtual ~OnLoad();

public:
	JavaVM* getJVM();
	Listener* getListener();
	void setListener(JavaVM* vm, jobject thiz, jobject weak_thiz);
	const char* getClassName();

private:
	JavaVM* jVM;
	Listener* mListener;
	const char* mClassPathName;
};
#ifdef __cplusplus
}
#endif


#endif /* ONLOAD_H_ */
