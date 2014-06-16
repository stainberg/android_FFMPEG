#ifndef PERFORMANCE_H
#define PERFORMANCE_H

#include <android/log.h>

#define COMPUTE_PERFORMANCE_BEGIN \
    double time = (double)av_gettime()/1000000.0;
	
#define COMPUTE_PERFORMANCE_END(per,average) \
    static double totaltime = 0.0; \
	static int count = 0; \
	double duration = (double)av_gettime()/1000000.0 - time; \
	count ++; \
	totaltime += duration; \
	__android_log_print(ANDROID_LOG_INFO, TAG, #per" =%f "#average" = %f",duration,totaltime/count);

#endif //PERFORMANCE_H
