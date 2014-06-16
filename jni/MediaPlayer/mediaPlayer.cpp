/*
 * mediaPlayer.cpp
 *
 *  Created on: 2012-12-28
 *      Author: stainberg
 */
#include "mediaPlayer.h"
#include "myLog.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "MediaPlayer.cpp"

MediaPlayer::MediaPlayer(OnLoad* load) {
	mPlayer = this;
	NUM_PROCS = sysconf(_SC_NPROCESSORS_CONF);
	mpLoad = load;
	mExitFlag = 1;
	_url = NULL;
	mRendererInitFlag = false;
	mListener = load->getListener();
	mCurrentposition = mDuration = mSeekposition = 0;
	mVideoState = new VideoState();
	mAudioQueue = mVideoState->getAudioQueue();//get audio & video packet queue.
	mVideoQueue = mVideoState->getVideoQueue();
	pFormatCtx = mVideoState->getAVFormatCtx();//get FormatContext
	mRendererProcess = new Renderer();//new renderer
	LOGI("MediaPlayer");
}

MediaPlayer::~MediaPlayer() {//release memory.
	if(!mVideoState->mState == MEDIA_PLAYER_STATE_ERROR) {
		mAudioProcess->stop();
		mVideoProcess->stop();
		mVideoState->bufferingFlag = 0;
		mVideoState->mState = MEDIA_PLAYER_STOPPED;
		if(mVideoState->mState == MEDIA_PLAYER_STARTED ||
				mVideoState->mState == MEDIA_PLAYER_PAUSED ||
				mVideoState->mState == MEDIA_PLAYER_PLAYBACK_COMPLETE ||
				mVideoState->mState == MEDIA_PLAYER_STOPPED) {
			mAudioProcess->stop();
			mVideoProcess->stop();
			mVideoState->mState = MEDIA_PLAYER_STOPPED;
			while (!(mAudioProcess->getThreadExitFlag() & mVideoProcess->getThreadExitFlag())) {
				usleep(1000 * 50);
			}
			while (!mExitFlag) {
				usleep(1000 * 50);
			}
		}
	}
	mListener->notify(pthread_self(), MSG_RELEASE_MEDIAPLAYER, 1, "MSG_RELEASE_MEDIAPLAYER");
	mPlayer = NULL;
	mpLoad = NULL;
	mListener = NULL;
	mRendererInitFlag = false;
	mCurrentposition = mDuration = mSeekposition = 0;
	mAudioQueue = NULL;
	mVideoQueue = NULL;
	mAudioProcess = NULL;
	mVideoProcess = NULL;
	mRendererProcess = NULL;
	mVideoState = NULL;
	pFormatCtx = NULL;
	LOGI("MediaPlayer::~MediaPlayer");
}

status_t MediaPlayer::init() {//init
	av_register_all();
	avformat_network_init();
	LOGI("MediaPlayer::init");
	return NO_ERROR;
}

status_t MediaPlayer::initSurface(ANativeWindow* window) {//init surface or reset surface
	mRendererProcess->setRendererFlag();
	mRendererProcess->setWindow(window);
	return NO_ERROR;
}

double MediaPlayer::getPosition() {//return current position
	return mVideoState->position;
}

double MediaPlayer::getDuration() {//return duration
	return mVideoState->duration;
}

status_t MediaPlayer::start() {//start to play
	if(mVideoState->mState == MEDIA_PLAYER_STATE_ERROR)
	{
		return NO_ERROR;
	}
	if(mAudioProcess.get() != NULL) {//start audio decode <new thread>
		mAudioProcess->start();
	}
	if(mVideoProcess.get() != NULL) {//start video decode <new thread>
		mVideoProcess->start();
	}
	startDecodeMovie();//start main looper. used for get packet from stream, demux audio&video stream etc... <new thread>
	return NO_ERROR;
}

status_t MediaPlayer::stop() {//stop play
	if(mVideoState->mState == MEDIA_PLAYER_STATE_ERROR) {
		return NO_ERROR;
	}
	if(mAudioProcess.get() != NULL) {//stop audio decode
		mAudioProcess->stop();
	}
	if(mVideoProcess.get() != NULL) {//stop video decode
		mVideoProcess->stop();
	}
	mVideoState->bufferingFlag = 0;
	mVideoState->mState = MEDIA_PLAYER_STOPPED;
	return NO_ERROR;
}

status_t MediaPlayer::pause() {//pause paly
	if(mVideoState->mState == MEDIA_PLAYER_STATE_ERROR) {
		return NO_ERROR;
	}
	mVideoState->paused = !mVideoState->paused;
	if (mVideoState->paused) {
		mVideoState->mState = MEDIA_PLAYER_PAUSED;
		mListener->notify(pthread_self(), MSG_PAUSE, 1, "MSG_PAUSE");
		av_read_pause(pFormatCtx);
	} else {
		mVideoState->mState = MEDIA_PLAYER_STARTED;
		mListener->notify(pthread_self(), MSG_PLAYING, 1, "MSG_PLAYING");
		av_read_play(pFormatCtx);
	}
	return NO_ERROR;
}

status_t MediaPlayer::seekTo(double position) {//seek to position
	mVideoState->complete = 0;
	mVideoState->seek_pos = position*AV_TIME_BASE;
	mVideoState->seekReq = 1;
	mVideoState->seekReq_2 = 1;
	mListener->notify(pthread_self(), MSG_SEEK_START, 1, "MSG_SEEK_START");
	return NO_ERROR;
}

bool MediaPlayer::isPlaying() {//get player's status
	if(mVideoState->mState == MEDIA_PLAYER_STARTED) {
		return true;
	} else {
		return false;
	}
}

int MediaPlayer::abort_Cond_CB(void* mediaPlayerPtr) {//interrupt method
	MediaPlayer *pThis = (MediaPlayer*) mediaPlayerPtr;
	int ret = pThis->mVideoState->mState != MEDIA_PLAYER_DECODED
			&& pThis->mVideoState->mState != MEDIA_PLAYER_STOPPED
			&& pThis->mVideoState->mState != MEDIA_PLAYER_STATE_ERROR
			&& pThis->mVideoState->mState != MEDIA_PLAYER_IDLE;
	return ret;
}

status_t MediaPlayer::setDataSource(const char* url) {//set url <this run in the main thread, and setDataSource() will start new thread to parse stream>
	LOGI("MediaPlayer::setDataSource in");
	if(mVideoState->setDataSourceFlag) {
		return NO_ERROR;
	}
	if(_url) {
		av_free(_url);
	}
	if (url != NULL) {
		mVideoState->setFileName(url);
		unsigned len = strlen(url);
		_url = (char*) av_mallocz(len + 1);
		memcpy(_url, url , len + 1);
		mVideoState->setDataSourceFlag = 1;
		mVideoState->mState = MEDIA_PLAYER_IDLE;
		LOGD("mVideoState->getFileName() = %s", mVideoState->getFileName());
		mListener->notify(pthread_self(), MSG_TEST, 1, mVideoState->getFileName());
		setDataSource();
		return NO_ERROR;
	} else {
		return BAD_VALUE;
	}
	LOGI("MediaPlayer::setDataSource out");
}

status_t MediaPlayer::setDataSource() {//start new parse thread
	mListener->notify(pthread_self(), MSG_BUFFERING, 1, "MSG_BUFFERING");
	pthread_create(&mSetDataSource, NULL, openSourceProc, this);
}

void* MediaPlayer::openSourceProc(void* ptr) {//real parse mathod.
	LOGI("MediaPlayer::openSourceProc create thread");
	AVDictionary **opts;
	AVInputFormat* file_iformat;
	MediaPlayer* player = (MediaPlayer*) ptr;
	int ret, i;

	if(player->mAudioProcess.get() != NULL)//if play not stoped, then stop it first.
	{
		player->mAudioProcess->stop();
		while (!player->mAudioProcess->getThreadExitFlag())
		{
			usleep(1000 * 50);
		}
	}
	if(player->mVideoProcess.get() != NULL) 
	{
		player->mVideoProcess->stop();
		while(!player->mVideoProcess->getThreadExitFlag())
		{
			usleep(1000 * 50);
		}
	}
	while (!player->mExitFlag)//sync thread
	{
		usleep(1000 * 50);
	}
	player->mAudioProcess = NULL;
	player->mVideoProcess = NULL;
	player->mAudioQueue = NULL;
	player->mVideoQueue = NULL;
	player->mVideoState->initVideoState();
	player->mVideoState->setFileName(player->_url);
	player->mAudioQueue = player->mVideoState->getAudioQueue();
	player->mVideoQueue = player->mVideoState->getVideoQueue();
	player->pFormatCtx = player->mVideoState->getAVFormatCtx();

//	LOGI("1 time = %f", (double)av_gettime()/1000000.0);

	ret = avformat_open_input(&player->pFormatCtx, player->mVideoState->getFileName(),//open stream
			NULL, NULL);

//	LOGI("2 time = %f", (double)av_gettime()/1000000.0);

	if (ret < 0) {
		player->mListener->notify(pthread_self(), MSG_ERROR, 1, "MSG_ERROR");
		player->mVideoState->setDataSourceFlag = 0;
		return 0;
	}
	opts = setup_find_stream_info_opts(player->pFormatCtx, NULL);

//	LOGI("3 time = %f", (double)av_gettime()/1000000.0);

	ret = avformat_find_stream_info(player->pFormatCtx, opts);//get stream infomation, it will toke so many time to parse stream, so u can set max_anaylze_duration to reduce the time.

//	LOGI("4 time = %f", (double)av_gettime()/1000000.0);

	for (i = 0; i < player->pFormatCtx->nb_streams; i++)//free dict
		av_dict_free(&opts[i]);
	av_freep(&opts);

//	LOGI("5 time = %f", (double)av_gettime()/1000000.0);

	player->mVideoState->mState = MEDIA_PLAYER_INITIALIZED;//set status and send msg to JAVA
	player->mVideoState->duration = player->pFormatCtx->duration/AV_TIME_BASE;
	player->mListener->notify(pthread_self(), MSG_DURATION, (int)(player->mVideoState->duration*1000), "MSG_DURATION");
	player->mListener->notify(pthread_self(), MSG_POSISION, (int)(player->getPosition()*1000), "MSG_POSISION");
	player->mListener->notify(pthread_self(), MSG_BUFFERED, 1, "MSG_BUFFERED");
	player->mVideoState->setDataSourceFlag = 0;
	LOGI("MediaPlayer::openSourceProc create thread out");
	player->mListener->notify(pthread_self(), MSG_FILE_OPEN_OK, 1, "MSG_FILE_OPEN_OK");
	return 0;
}

status_t MediaPlayer::prepare() {//prepare video & audio device
	LOGI("MediaPlayer::prepare");
	if(mVideoState->mState == MEDIA_PLAYER_STATE_ERROR)
	{
		return NO_ERROR;
	}
	pthread_create(&mPrepare, NULL, prepareProc, this);
	return NO_ERROR;
}

void* MediaPlayer::prepareProc(void* ptr) {
	LOGI("MediaPlayer::prepareProc create thread in");
	MediaPlayer* player = (MediaPlayer*) ptr;
	AVFormatContext* pFormatCtx = player->pFormatCtx;
	player->mVideoState->mState = MEDIA_PLAYER_PREPARING;
	for (int i = 0; i < pFormatCtx->nb_streams; i++) {
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
			player->mVideoState->setVideoIndex(i);
			continue;
		}
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
			player->mVideoState->setAudioIndex(i);
			continue;
		}
	}
	LOGI("video index = %d, audio index = %d", player->mVideoState->getVideoIndex(), player->mVideoState->getAudioIndex());
	if(player->mVideoState->getAudioIndex() >= 0) {
		player->prepareAudio();
	}
	if(player->mVideoState->getVideoIndex() >= 0) {
		player->prepareVideo();
	}
	player->mVideoState->mState = MEDIA_PLAYER_PREPARED;
	player->mListener->notify(pthread_self(), MSG_MOVIE_PREPARE_OK, 1, "MSG_MOVIE_PREPARE_OK");
	LOGI("MediaPlayer::prepareProc create thread out");
	return 0;
}

status_t MediaPlayer::prepareAudio() {
	int ret;
	AVCodec* codec;
	AVCodecContext* pCodecCtx;
	AVFormatContext* pFormatCtx = mPlayer->pFormatCtx;
	AVDictionary *opts;
	pCodecCtx = pFormatCtx->streams[mPlayer->mVideoState->getAudioIndex()]->codec;
	codec = avcodec_find_decoder(pCodecCtx->codec_id);
	opts = filter_codec_opts(codec_opts, codec, pFormatCtx, pFormatCtx->streams[mPlayer->mVideoState->getAudioIndex()]);
	ret = avcodec_open2(pCodecCtx, codec, &opts);
	if (ret < 0) {
		LOGE("prepareAudio ERROR");
		return UNKNOWN_ERROR;
	}
	LOGV("channels = %d", pCodecCtx->channels);
	mVideoState->audio_st = pFormatCtx->streams[mPlayer->mVideoState->getAudioIndex()];
	mAudioProcess = new AudioPlayer(mVideoState);
	mAudioProcess->setQueue(mAudioQueue);
	mAudioProcess->setListener(mListener);
	LOGI("audio prepare Ok!");
	return NO_ERROR;
}

status_t MediaPlayer::prepareVideo() {
	int ret;
	AVCodec* codec;
	AVCodecContext* pCodecCtx;
	AVFormatContext* pFormatCtx = mPlayer->pFormatCtx;
	AVDictionary *opts;
	pCodecCtx = pFormatCtx->streams[mPlayer->mVideoState->getVideoIndex()]->codec;
	codec = avcodec_find_decoder(pCodecCtx->codec_id);
	opts = filter_codec_opts(codec_opts, codec, pFormatCtx, pFormatCtx->streams[mPlayer->mVideoState->getVideoIndex()]);
	ret = avcodec_open2(pCodecCtx, codec, NULL);
	if (ret < 0) {
		LOGE("prepareVideo ERROR");
		return UNKNOWN_ERROR;
	}
	LOGV("width = %d, height = %d", pCodecCtx->width, pCodecCtx->height);
	mVideoProcess = new VideoPlayer(mVideoState);
	mVideoProcess->setQueue(mVideoQueue);
	mVideoProcess->setListener(mListener);
	mVideoProcess->setRenderer(mRendererProcess);
	mRendererProcess->listener = mListener;
	mVideoState->video_st = pFormatCtx->streams[mPlayer->mVideoState->getVideoIndex()];
	LOGI("video prepare Ok!");
	return NO_ERROR;
}

status_t MediaPlayer::prepareRenderer(ANativeWindow* window) {
	mRendererProcess->setWindow(window);
	LOGI("renderer prepare Ok!");
	return NO_ERROR;
}

status_t MediaPlayer::startDecodeMovie() {
	if(mVideoState->mState == MEDIA_PLAYER_STATE_ERROR)
	{
		return NO_ERROR;
	}
	pthread_create(&mMainThread, NULL, decodeProc, this);
}

void* MediaPlayer::decodeProc(void* ptr) {//get packet looper, it's importent thread
	LOGI("begin decode!");
	int ret = -1;
	AVPacket pkt1, *pkt = &pkt1;
	MediaPlayer* player = (MediaPlayer*) ptr;
	AVFormatContext *formatCtx = player->pFormatCtx;
	player->mExitFlag = 0;
	player->mVideoState->setDataSourceFlag = 0;
	while (player->abort_Cond_CB(ptr)) {

		if(player->mVideoState->seekReq) {
			int64_t seek_target = player->mVideoState->seek_pos;
			int64_t seek_min    = player->mVideoState->seek_rel > 0 ? seek_target - player->mVideoState->seek_rel + 2: INT64_MIN;
			int64_t seek_max    = player->mVideoState->seek_rel < 0 ? seek_target - player->mVideoState->seek_rel - 2: INT64_MAX;
			avformat_seek_file(player->pFormatCtx, -1, seek_min, seek_target, seek_max,player->mVideoState->seek_flags);
			player->mVideoQueue->PacketQueue_Flush();
			player->mAudioQueue->PacketQueue_Flush();
			player->mVideoQueue->PacketQueue_Put(&player->mVideoQueue->flush_pkt);
			player->mAudioQueue->PacketQueue_Put(&player->mAudioQueue->flush_pkt);
			player->mVideoState->seekReq = 0;
		}

		if (player->mAudioQueue->PacketQueue_Get_Data_Size() + player->mVideoQueue->PacketQueue_Get_Data_Size() >= MAX_SIZE) {
			usleep(5000);
			continue;
		}
		ret = av_read_frame(formatCtx, pkt);//get packet from stream

		if (ret < 0) {
			if (ret == AVERROR_EOF || url_feof(formatCtx->pb)) {//check stream
				player->mVideoState->complete = 1;
			}
			if (formatCtx->pb && formatCtx->pb->error) {
				player->mVideoState->complete = 0;
				if(!player->mVideoState->errorFlag)
				{
					player->mListener->notify(pthread_self(), MSG_ERROR, 0, "Something Error Happened");
					player->mVideoState->errorFlag = 1;
				}
			}
			usleep(1000);
			continue;
		}
		if(player->mVideoState->errorFlag)
		{
			player->mListener->notify(pthread_self(), MSG_ERROR_FIXED, 0, "Fixed Error");
			player->mVideoState->errorFlag = 0;
		}
		if (pkt->stream_index == player->mVideoState->getAudioIndex()) {
			player->mAudioQueue->PacketQueue_Put(pkt);
		}
		if (pkt->stream_index == player->mVideoState->getVideoIndex()) {
			player->mVideoQueue->PacketQueue_Put(pkt);
		}
	}
	if(player->mAudioProcess.get() != NULL)//if exit the thread ,then sync decode & this thread.
	{
		pthread_join(player->mAudioProcess->getThreadId(), NULL);
	}
	if(player->mVideoProcess.get() != NULL)
	{
		pthread_join(player->mVideoProcess->getThreadId(), NULL);
	}
	LOGI("main thread interrupt!");
	if(player->mAudioProcess.get() != NULL)
	{
		while(!player->mAudioProcess->getThreadExitFlag())
		{
//			LOGI("wait mAudioProcess");
			usleep(1000);
		}
	}
	if(player->mVideoProcess.get() != NULL)
	{
		while(!player->mVideoProcess->getThreadExitFlag())
		{
//			LOGI("wait mVideoProcess");
			usleep(1000);
		}
	}
//	while (!(player->mAudioProcess->getThreadExitFlag() & player->mVideoProcess->getThreadExitFlag())) {
//		usleep(5000);
//	}
	player->mListener->notify(pthread_self(), MSG_STOP, 1, "MSG_STOP");//notify JAVA
	LOGI("main thread exit!");
	LOGI("main end of decode!");
	player->mExitFlag = 1;
	return 0;
}
