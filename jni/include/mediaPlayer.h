/*
 * mediaPlayer.h
 *
 *  Created on: 2012-12-28
 *      Author: stainberg
 */

#ifndef MEDIAPLAYER_H_
#define MEDIAPLAYER_H_
#include "refBase.h"
#include "smartpointer.h"
#include "onLoad.h"
#include "mutex.h"
#include <stdint.h>
#include "audioPlayer.h"
#include "videoPlayer.h"
#include "videoState.h"
#include "packetQueue.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "cmdutils.h"

typedef int  (*MediaPlayer_Abort_Cond_CB)(void* mediaPlayerPtr);

class MediaPlayer;

class MediaPlayer {
public:
	MediaPlayer(OnLoad* load);
	virtual ~MediaPlayer();
	status_t init();
	status_t startDecodeMovie();
	status_t setDataSource();
	status_t prepare();
	status_t start();
	status_t setDataSource(const char* url);
	status_t stop();
	status_t prepareAudio();
	status_t prepareVideo();
	status_t prepareRenderer(ANativeWindow* window);
	status_t pause();
	status_t seekTo(double position);
	double getDuration();
	double getPosition();
	bool isPlaying();


	int abort_Cond_CB(void* mediaPlayerPtr);
	status_t initSurface(ANativeWindow* window);
private:
	static void* decodeProc(void* ptr);
	static void* openSourceProc(void* ptr);
	static void* prepareProc(void* ptr);
private:
	my_sp<VideoState> mVideoState;
	MediaPlayer* mPlayer;
	OnLoad* mpLoad;
	Listener* mListener;
	My_Mutex mLock;
	my_sp<AudioPlayer> mAudioProcess;
	my_sp<PacketQueue> mAudioQueue;
	my_sp<VideoPlayer> mVideoProcess;
	my_sp<PacketQueue> mVideoQueue;
	my_sp<Renderer> mRendererProcess;

	pthread_t mMainThread;
	pthread_t mPrepare;
	pthread_t mSetDataSource;
	int64_t mDuration;
	int64_t mCurrentposition;
	int64_t mSeekposition;
	int mExitFlag;
	bool mRendererInitFlag;
	AVFormatContext*  pFormatCtx;
	int NUM_PROCS;
	char* _url;
};

#ifdef __cplusplus
}
#endif

#endif /* MEDIAPLAYER_H_ */
