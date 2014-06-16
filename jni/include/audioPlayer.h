/*
 * audioPlayer.h
 *
 *  Created on: 2013-1-3
 *      Author: stainberg
 */

#ifndef AUDIOPLAYER_H_
#define AUDIOPLAYER_H_

#include "smartpointer.h"
#include "mutex.h"
#include "packetQueue.h"
#include "videoState.h"
#include "decodec.h"

#ifdef __cplusplus
extern "C" {
#endif

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

using namespace android;

class AudioPlayer:public Decodec {
public:
	AudioPlayer();
	AudioPlayer(my_sp<VideoState> vs);
	virtual ~AudioPlayer();
	inline void setQueue(my_sp<PacketQueue> q){mAudioQueue = q;};
	inline void setListener(Listener* l) {mListener = l;};
	void createEngine();
	void createBufferQueueAudioPlayer();
	static void playerCallback(SLAndroidSimpleBufferQueueItf bq, void *context);
	int decode_frame(uint8_t *stream);
private:
	void* decode(void* ptr);
private:
	my_sp<PacketQueue> mAudioQueue;
	my_sp<VideoState> mVs;
	int audio_st_index;
	AVFrame *frame;
	SLObjectItf engineObject;
	SLEngineItf engineEngine;
	SLObjectItf outputMixObject;
	SLObjectItf bqPlayerObject;
	SLPlayItf bqPlayerPlay;
	SLAndroidSimpleBufferQueueItf bqPlayerBufferQueue;
	AVPacket packet;
	Listener* mListener;
};


#ifdef __cplusplus
}
#endif


#endif /* AUDIOPLAYER_H_ */
