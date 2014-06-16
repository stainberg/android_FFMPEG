/*
 * videoState.h
 *
 *  Created on: 2013-1-11
 *      Author: stainberg
 */

#ifndef VIDEOSTATE_H_
#define VIDEOSTATE_H_

#include "myLog.h"
#include "mutex.h"
#include "refBase.h"
#include "packetQueue.h"
#include "listener.h"
#include <android/native_window.h> // requires ndk r5 or newer
#include <android/native_window_jni.h> // requires ndk r5 or newer

#ifdef TAG
#undef TAG
#endif
#define TAG "videoState.h"

#ifdef __cplusplus
extern "C"
{
#endif

#include "libavutil/avstring.h"
#include "libavutil/colorspace.h"
#include "libavutil/mathematics.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libavutil/dict.h"
#include "libavutil/parseutils.h"
#include "libavutil/samplefmt.h"
#include "libavutil/avassert.h"
#include "libavutil/internal.h"
#include "libavutil/opt.h"
#include "libavformat/avformat.h"
#include "libavformat/internal.h"
#include "libavdevice/avdevice.h"
#include "libswscale/swscale.h"
#include "libavcodec/avfft.h"
#include "libswresample/swresample.h"

enum
{
	MEDIA_TEST = -0xFFFF,
	MEDIA_INFO_CANT_OPEN = -1,
	MEDIA_INFO_OPEN = 0,
};

#define AV_NOSYNC_THRESHOLD 10.0
#define AV_SYNC_THRESHOLD 0.04

enum media_player_states//player status
{
	MEDIA_PLAYER_STATE_ERROR = 0,
	MEDIA_PLAYER_IDLE = 1 << 0,
	MEDIA_PLAYER_INITIALIZED = 1 << 1,
	MEDIA_PLAYER_PREPARING = 1 << 2,
	MEDIA_PLAYER_PREPARED = 1 << 3,
	MEDIA_PLAYER_DECODED = 1 << 4,
	MEDIA_PLAYER_STARTED = 1 << 5,
	MEDIA_PLAYER_PAUSED = 1 << 6,
	MEDIA_PLAYER_STOPPED = 1 << 7,
	MEDIA_PLAYER_PLAYBACK_COMPLETE = 1 << 8
};

class VideoState:public My_RefBase<VideoState>
{
public:
	VideoState();
	VideoState(Listener* l);
	virtual ~VideoState();
	inline void setFileName(const char* url){ \
		unsigned len = strlen(url); \
		if(mFileName) { \
			av_free(mFileName); \
			mFileName = NULL;\
		} \
		mFileName = (char*) av_mallocz(len + 1); \
		memcpy(mFileName, url, len + 1);};
	inline const char* getFileName(){return mFileName;};
	inline my_sp<PacketQueue> getAudioQueue() {return mAudioQueue;};
	inline int getAudioIndex(){return mAudio_st_index;};
	inline void setAudioIndex(int index){mAudio_st_index = index;};
	inline my_sp<PacketQueue> getVideoQueue() {return mVideoQueue;};
	inline int getVideoIndex(){return mVideo_st_index;};
	inline void setVideoIndex(int index){mVideo_st_index = index;};

	inline AVFormatContext* getAVFormatCtx(){return pFormatCtx;};
	inline AVStream* getAVStream(int index) {return pFormatCtx->streams[index];};

	inline void setWindow(ANativeWindow* window) {mWindow = window;};
	inline ANativeWindow* getWindow() {return mWindow;};

	void initVideoState();
	double get_audio_clock();
	double get_video_clock();
	double get_master_clock();
private:
	My_Mutex mLock;
	char* mFileName;
	my_sp<PacketQueue> mAudioQueue;
	int mAudio_st_index;
	my_sp<PacketQueue> mVideoQueue;
	int mVideo_st_index;

	ANativeWindow* mWindow;
	Listener* mListener;
public:
	double duration;
	double position;
	media_player_states mState;
	bool paused;
	bool last_paused;
	int seekReq, seekReq_2;
	int64_t seek_pos;
	int64_t seek_rel;
	int seek_flags;
	int seek_step;
	AVFormatContext* pFormatCtx;
	AVStream* video_st;
	AVStream* audio_st;
	int bufferingFlag;
	int syncVideoFlag;
	int errorFlag;

	double audio_clock;
	double audio_current_pts;
	double audio_current_pts_drift;

	double video_clock;
	double video_current_pts;
	double video_current_pts_drift;
	int64_t video_current_pos;

	double frame_last_pts;
	double frame_last_duration;
	double frame_timer;

	int complete;

	pthread_mutex_t seek_mutex;
	pthread_cond_t seek_cond;

	int setDataSourceFlag;

	AVInputFormat *iformat;
	AVDictionary *format_opts, *codec_opts;
};

inline VideoState::VideoState()
{
	position = 0;
	duration = 0;
	seek_pos = 0;
	seek_rel = 0;
	seek_step = 0;
	bufferingFlag = 0;
	syncVideoFlag = 0;
	errorFlag = 0;
	setDataSourceFlag = 0;
	last_paused = paused = false;
	mListener = NULL;
	seek_flags = AVSEEK_FLAG_BACKWARD;
	seekReq = seekReq_2 = 0;
//	seekBufferReq = 0;
	mFileName = NULL;
	mWindow = NULL;
	pFormatCtx = avformat_alloc_context();
	//maximum time (in AV_TIME_BASE units) during which the input should be analyzed in avformat_find_stream_info().
//	pFormatCtx->max_analyze_duration = 1000;//max time for analyze stream information u can set it or don't, and it will be take long time to analyze stream. maybe more than 15s.
//	pFormatCtx->probesize = 4096;//size of data to probe
	video_st = NULL;
	audio_st = NULL;
	mAudioQueue = new PacketQueue();//PacketQueueList,audio & video packet queue.
	mVideoQueue = new PacketQueue();
	mVideo_st_index = -1;//video & audio stream index.
	mAudio_st_index = -1;
	audio_clock = audio_current_pts = audio_current_pts_drift = 0;//use for AV sync
	video_clock = video_current_pts = video_current_pts_drift = video_current_pos = frame_last_pts = frame_last_duration = frame_timer = 0;//use for AV sync
	mState = MEDIA_PLAYER_IDLE;//Player status
	complete = 0;
	pthread_mutex_init(&seek_mutex, NULL);
	pthread_cond_init(&seek_cond, NULL);
}

inline VideoState::VideoState(Listener* l)
{
	position = 0;
	duration = 0;
	seek_pos = 0;
	seek_rel = 0;
	seek_step = 0;
	mListener = l;
	bufferingFlag = 0;
	syncVideoFlag = 0;
	errorFlag = 0;
	setDataSourceFlag = 0;
	last_paused = paused = false;
	seek_flags = AVSEEK_FLAG_BACKWARD;
	seekReq = seekReq_2 = 0;
//	seekBufferReq = 0;
	mFileName = NULL;
	mWindow = NULL;
	pFormatCtx = avformat_alloc_context();
//	pFormatCtx->max_analyze_duration = 1000;
//	pFormatCtx->probesize = 4096;
	video_st = NULL;
	audio_st = NULL;
	mAudioQueue = new PacketQueue();
	mVideoQueue = new PacketQueue();
	mVideo_st_index = -1;
	mAudio_st_index = -1;
	audio_clock = audio_current_pts = audio_current_pts_drift = 0;
	video_clock = video_current_pts = video_current_pts_drift = video_current_pos = frame_last_pts = frame_last_duration = frame_timer = 0;
	mState = MEDIA_PLAYER_IDLE;
	complete = 0;
	pthread_mutex_init(&seek_mutex, NULL);
	pthread_cond_init(&seek_cond, NULL);
}

inline VideoState::~VideoState()
{
	avformat_close_input(&pFormatCtx);
	av_free(mFileName);
	mFileName = NULL;
	mAudioQueue = NULL;
	mVideoQueue = NULL;
	video_st = NULL;
	audio_st = NULL;
	mWindow = NULL;
	LOGI("VideoState::~VideoState");
}

inline void VideoState::initVideoState()
{
	position = 0;
	duration = 0;
	seek_pos = 0;
	seek_rel = 0;
	seek_step = 0;
	bufferingFlag = 0;
	syncVideoFlag = 0;
	errorFlag = 0;
	last_paused = paused = false;
	seek_flags = AVSEEK_FLAG_BACKWARD;
	seekReq = seekReq_2 = 0;
	mWindow = NULL;
	pFormatCtx = avformat_alloc_context();
//	pFormatCtx->max_analyze_duration = 1000;
//	pFormatCtx->probesize = 4096;
	video_st = NULL;
	audio_st = NULL;
	mAudioQueue = NULL;
	mVideoQueue = NULL;
	mAudioQueue = new PacketQueue();
	mVideoQueue = new PacketQueue();
	mVideo_st_index = -1;
	mAudio_st_index = -1;
	audio_clock = audio_current_pts = audio_current_pts_drift = 0;
	video_clock = video_current_pts = video_current_pts_drift = video_current_pos = frame_last_pts = frame_last_duration = frame_timer = 0;
	complete = 0;
	mState = MEDIA_PLAYER_IDLE;
}


inline double VideoState::get_audio_clock()
{
//	if (paused) {
		return audio_current_pts;
//	} else {
//		return audio_current_pts_drift + av_gettime() / 1000000.0;
//	}
}

inline double VideoState::get_video_clock()
{
//    if (paused) {
        return video_current_pts;
//    } else {
//        return video_current_pts_drift + av_gettime() / 1000000.0;
//    }
}

inline double VideoState::get_master_clock()
{
    double val;
	if (audio_st)
		val = get_audio_clock();
	else
		val = get_video_clock();
    return val;
}

#ifdef __cplusplus
}
#endif



#endif /* VIDEOSTATE_H_ */
