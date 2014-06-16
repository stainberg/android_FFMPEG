/*
 * refBase.h
 *
 *  Created on: 2013-1-4
 *      Author: stainberg
 */

#ifndef REFBASE_H_
#define REFBASE_H_
#ifdef TAG
#undef TAG
#endif
#define TAG "refBase.h"
#include "atomic.h"
#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>

template <class T>
class My_RefBase {
protected:
	inline My_RefBase():mCount(0) {}
	inline virtual ~My_RefBase() {}
public:
	inline void incStrong(const void* ptr) const {
		android_atomic_inc(&mCount);
	}
	inline void decStrong(const void* ptr) const {
		if(android_atomic_dec(&mCount) == 1) {
			delete static_cast<const T*>(this);
		}
	}
	inline int32_t getCount() const {
		return mCount;
	}
	typedef My_RefBase<T> basetype;
private:
	mutable volatile int32_t mCount;
};

#endif /* REFBASE_H_ */
