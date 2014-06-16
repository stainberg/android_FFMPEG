/*
 * smartpointer.h
 *
 *  Created on: 2013-1-4
 *      Author: stainberg
 */

#ifndef SMARTPOINTER_H_
#define SMARTPOINTER_H_

#include <stdint.h>
#include <sys/types.h>
#include <stdlib.h>

#ifdef TAG
#undef TAG
#endif
#define TAG "smartpointer.h"

template<class T>
class my_sp {
public:
	inline my_sp():m_ptr(0) {}
	my_sp(T* other);
	my_sp(const my_sp<T>& other);
	template<class U> my_sp(U* other);
	template<class U> my_sp(const my_sp<U>& other);
	virtual ~my_sp();

	my_sp& operator = (T* other);
	my_sp& operator = (const my_sp<T>& other);
	template<class U> my_sp& operator = (U* other);
	template<class U> my_sp& operator = (const my_sp<U>& other);

	void clear();

	inline T& operator* () const {return *m_ptr;}
	inline T* operator-> () const {return m_ptr;}
	inline T* get() const {return m_ptr;}

private:
	template<class Y> friend class my_sp;
	T* m_ptr;
};

template<class T>
my_sp<T>::my_sp(T* other):m_ptr(other) {
	if(other) {
		other->incStrong(this);
	}
}

template<class T>
my_sp<T>::my_sp(const my_sp<T>& other):m_ptr(other.m_ptr) {
	if(m_ptr) {
		m_ptr->incStrong(this);
	}
}

template<class T>
template<class U>
my_sp<T>::my_sp(U* other):m_ptr(other) {
	if(other) {
		other->incStrong(this);
	}
}

template<class T>
template<class U>
my_sp<T>::my_sp(const my_sp<U>& other):m_ptr(other.m_ptr) {
	if(m_ptr) {
		m_ptr->incStrong(this);
	}
}

template<class T>
my_sp<T>::~my_sp() {
	if(m_ptr) {
		m_ptr->decStrong(this);
	}
}

template<class T>
my_sp<T>& my_sp<T>::operator =(T* other) {
	if(other) {
		other->incStrong(this);
	}
	if(m_ptr) {
		m_ptr->decStrong(this);
	}
	m_ptr = other;
	return *this;
}

template<class T>
my_sp<T>& my_sp<T>::operator =(const my_sp<T>& other) {
	if(other.m_ptr) {
		other.m_ptr->incStrong(this);
	}
	if(m_ptr) {
		m_ptr->decStrong(this);
	}
	m_ptr = other.m_ptr;
	return *this;
}

template<class T>
template<class U>
my_sp<T>& my_sp<T>::operator =(U* other) {
	if(other) {
		other->incStrong(this);
	}
	if(m_ptr) {
		m_ptr->decStrong(this);
	}
	m_ptr = other;
	return *this;
}

template<class T>
template<class U>
my_sp<T>& my_sp<T>::operator =(const my_sp<U>& other) {
	if(other.m_ptr) {
		other.m_ptr->incStrong(this);
	}
	if(m_ptr) {
		m_ptr->decStrong(this);
	}
	m_ptr = other.m_ptr;
	return *this;
}

template<class T>
void my_sp<T>::clear() {
	if(m_ptr) {
		m_ptr->decStrong(this);
		m_ptr = 0;
	}
}



#endif /* SMARTPOINTER_H_ */
