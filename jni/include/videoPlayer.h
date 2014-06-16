/*
 * videoPlayer.h
 *
 *  Created on: 2013-1-3
 *      Author: stainberg
 */

#ifndef VIDEOPLAYER_H_
#define VIDEOPLAYER_H_

#include "smartpointer.h"
#include "mutex.h"
#include "decodec.h"
#include "packetQueue.h"
#include "videoState.h"
#include "renderer.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_DISPLAY_QUEUE 1
#define MAX_FRAME_SIZE MAX_DISPLAY_QUEUE*3110400

using namespace android;

class VideoPlayer:public Decodec {
public:
	VideoPlayer();
	VideoPlayer(my_sp<VideoState> vs);
	virtual ~VideoPlayer();
	inline void setQueue(my_sp<PacketQueue> q){mVideoQueue = q;};
	inline void setListener(Listener* l) {mListener = l;};
	inline void setRenderer(my_sp<Renderer> renderer) {mRendererProcess = renderer;};
private:
	void* decode(void* ptr);
	void update_video_pts(double pts, int64_t pos);
	double compute_target_delay();
private:
	VideoPlayer* mVideoProcess;
	my_sp<PacketQueue> mVideoQueue;
	my_sp<VideoState> mVs;
	my_sp<Renderer> mRendererProcess;
	int video_st_index;
	AVFrame *frame;
	YUVBUFFER* buffer;
	Listener* mListener;

	BYTE* display_buff;
};


#ifdef __cplusplus
}
#endif



#endif /* VIDEOPLAYER_H_ */
