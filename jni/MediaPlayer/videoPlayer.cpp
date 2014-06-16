/*
 * videoPlayer.cpp
 *
 *  Created on: 2013-1-3
 *      Author: stainberg
 */
#include  "myLog.h"
#include "videoPlayer.h"

#ifdef TAG
#undef TAG
#endif
#define TAG "videoPlayer.cpp"



VideoPlayer::VideoPlayer()
{
	LOGI("VideoPlayer::VideoPlayer()");
	mVideoProcess = this;
	mListener = NULL;
	video_st_index = -1;
	mRendererProcess = NULL;
	buffer = (YUVBUFFER*)av_mallocz(sizeof(YUVBUFFER));
	frame = avcodec_alloc_frame();
	display_buff = (uint8_t*)av_mallocz(MAX_FRAME_SIZE);
}

VideoPlayer::VideoPlayer(my_sp<VideoState> vs)
{
	LOGI("VideoPlayer::VideoPlayer() with args");
	mVideoProcess = this;
	mListener = NULL;
	video_st_index = -1;
	mRendererProcess = NULL;
	buffer = (YUVBUFFER*)av_mallocz(sizeof(YUVBUFFER));
	frame = avcodec_alloc_frame();
	display_buff = (uint8_t*)av_mallocz(MAX_FRAME_SIZE);
	mVs = vs;
	video_st_index = mVs->getVideoIndex();
}

VideoPlayer::~VideoPlayer()
{
	av_free(frame);
	av_free(buffer);
	av_free(display_buff);
	display_buff = NULL;
	mListener = NULL;
	mRendererProcess = NULL;
	mVideoProcess = NULL;
	mVideoQueue = NULL;
	mVs = NULL;
	LOGI("VideoPlayer::~VideoPlayer()");
}

void VideoPlayer::update_video_pts(double pts, int64_t pos) {//update pts for sync AV
    mVs->video_current_pts = pts;
    mVs->video_current_pts_drift = mVs->video_current_pts - av_gettime() / 1000000.0;
    mVs->video_current_pos = pos;
    mVs->frame_last_pts = pts;
}

double VideoPlayer::compute_target_delay()//compute delay for sync AV
{
    double diff;
	diff = mVs->get_video_clock() - mVs->get_master_clock();
//	LOGW("get_video_clock = %f, get_master_clock = %f", mVs->get_video_clock() , mVs->get_master_clock());
	if(fabs(diff) <= AV_SYNC_THRESHOLD) {
		return 0;
	}
	return diff;
}

void* VideoPlayer::decode(void* ptr)//decode frame
{
	LOGI("VideoPlayer::decode in");
	VideoPlayer* videoDec = (VideoPlayer*)ptr;
	AVPacket pkt1, *pkt = &pkt1;
	AVCodecContext *dec = videoDec->mVs->getAVStream(videoDec->video_st_index)->codec;
	int ret = -1, new_packet = 0, ct = 0, got_frame = 0, len = 0;
	double pts = 0, frame_delay = 0;
	double dpts = 0, ptsdiff = 0;
	int64_t pos = -1;
	if(videoDec->mVs->pFormatCtx->width != 0 & videoDec->mVs->pFormatCtx->height != 0)
	{
		mRendererProcess->initialize(videoDec->mVs->pFormatCtx->width, videoDec->mVs->pFormatCtx->height);
	}
	else
	{
		mRendererProcess->initialize(dec->width, dec->height);
	}
	mVs->video_current_pos      = -1;//当前的视频播放位置
	mVs->frame_last_pts         = AV_NOPTS_VALUE;//最后帧的pts
	mVs->frame_last_duration    = 0;//最后帧的显示时长
	mVs->frame_timer            = (double)av_gettime() / 1000000.0;//上一帧的系统时钟
	if(mVideoQueue.get() != NULL)
	{
		LOGI("VideoPlayer::decode start");
		while(!videoDec->getThreadStatusInterrupt())
		{
			if(mVs->mState == MEDIA_PLAYER_PAUSED) {
				usleep(1000*100);
				continue;
			}
//			if(videoDec->mVs->bufferingFlag) {
//				mVs->syncVideoFlag = 0;
//				usleep(1000);
//				continue;
//			}
			while(mVs->seekReq) {
				usleep(1000);
				continue;
			}
			if(videoDec->mVideoQueue->PacketQueue_Get_nb_Pkt() > 0)
			{
				new_packet = mVideoProcess->mVideoQueue->PacketQueue_Get(pkt);
				if (pkt->data == mVideoProcess->mVideoQueue->flush_pkt.data)
				{
					avcodec_flush_buffers(mVs->video_st->codec);
					pkt->size = 0;
					av_free(frame);
					frame = avcodec_alloc_frame();
					mVs->frame_last_pts = AV_NOPTS_VALUE;
					mVs->video_current_pts = AV_NOPTS_VALUE;
					continue;
				}
				avcodec_decode_video2(dec, frame, &got_frame, pkt);//decode frame
				if (got_frame) {
					int ret = 1;
					pts = frame->pkt_pts;
					if (pts == AV_NOPTS_VALUE) {
						pts = 0;
					}
					if (mVs->frame_last_pts != AV_NOPTS_VALUE && pts) {
						dpts = av_q2d(mVs->video_st->time_base) * pts;
						ptsdiff = dpts - mVs->frame_last_pts;
					}
				}
				pos = pkt->pos;
				av_free_packet(pkt);
				pts = pts * av_q2d(mVs->video_st->time_base);
				if (pts != 0) {
					mVs->video_clock = pts;
				} else {
					pts = mVs->video_clock;
				}
				frame_delay = av_q2d(mVs->video_st->codec->time_base);
				frame_delay += frame->repeat_pict * (frame_delay * 0.5);
				mVs->video_clock += frame_delay;
				if(!frame->data[0])
				{
					continue;
				}
				if (ptsdiff > 0) {
					mVs->frame_last_duration = ptsdiff;
				}
#ifndef DEBUG
retry:
				update_video_pts(pts, pos);
				if(videoDec->getThreadStatusInterrupt()) {
					break;
				}
//				if(mVs->seekReq_2) {
//					continue;
//				}
				double syncDiff = compute_target_delay();
				if(syncDiff > AV_NOSYNC_THRESHOLD) {
					continue;
				}
				double current_timer = av_gettime()/1000000.0;
				if (current_timer < mVs->frame_timer + syncDiff) {//视频快于音频，进入等待
					usleep(1000);
					goto retry;
				}
#endif
				//build data for OpenGLES  <YUV420p>
				YV12Image &im = buffer->image;
				YUVPLANES &planes = buffer->planes;
				im.width = dec->width;
				im.height = dec->height;
				im.cshift_x = 1;
				im.cshift_y = 1;
				im.stride[0] = im.width;
				im.stride[1] = im.width >> im.cshift_x;
				im.stride[2] = im.width >> im.cshift_x;
				im.planeSize[0] = im.stride[0] * im.height;
				im.planeSize[1] = im.stride[1] * (im.height >> im.cshift_y);
				im.planeSize[2] = im.stride[2] * (im.height >> im.cshift_y);
				for (int i = 0; i < MAX_PLANES; i++)
				{
					int shift = (i == 0) ? 0 : 1;
					planes[i].texwidth = im.width >> shift;
					planes[i].texheight = im.height >> shift;
				}
				BYTE *s = frame->data[0];
				BYTE *d = im.planeData[0] = display_buff;
				int w = frame->width;
				int h = frame->height;
				if ((w == frame->linesize[0]) && ((unsigned int) frame->linesize[0] == im.stride[0]))
				{
					memcpy(d, s, w*h);
				}
				else
				{
					for (int y = 0; y < h; y++)
					{
						memcpy(d, s, w);
						s += frame->linesize[0];
						d += im.stride[0];
					}
				}
				s = frame->data[1];
				d = im.planeData[1] = display_buff + im.planeSize[0];
				w = frame->width >> 1;
				h = frame->height >> 1;
				if((w == frame->linesize[1]) && ((unsigned int) frame->linesize[1] == im.stride[1]))
				{
					memcpy(d, s, w*h);
				}
				else
				{
					for(int y = 0; y < h; y++)
					{
						memcpy(d, s, w);
						s += frame->linesize[1];
						d += im.stride[1];
					}
				}
				s = frame->data[2];
				d = im.planeData[2] = display_buff + im.planeSize[0] + im.planeSize[1];
				if((w == frame->linesize[2]) && ((unsigned int) frame->linesize[2] == im.stride[2]))
				{
					memcpy(d, s, w*h);
				}
				else
				{
					for (int y = 0; y < h; y++)
					{
						memcpy(d, s, w);
						s += frame->linesize[2];
						d += im.stride[2];
					}
				}
				if(mRendererProcess->surface_change_lock)
				{
					mRendererProcess->destroy();
//					mRendererProcess->initialize(dec->width, dec->height);
					if(videoDec->mVs->pFormatCtx->width != 0 & videoDec->mVs->pFormatCtx->height != 0)
					{
						mRendererProcess->initialize(videoDec->mVs->pFormatCtx->width, videoDec->mVs->pFormatCtx->height);
					}
					else
					{
						mRendererProcess->initialize(dec->width, dec->height);
					}
					mRendererProcess->surface_change_lock = 0;
				}
				else
				{
					mRendererProcess->drawFrame(buffer);//draw Frame
					mVs->syncVideoFlag = 1;
					if(mVs->mState == MEDIA_PLAYER_PREPARED) {
						mVs->mState = MEDIA_PLAYER_STARTED;
						mListener->notify(pthread_self(), MSG_PLAYING, 1, "MSG_PLAYING");
					}
				}
				mVs->frame_timer = (double)av_gettime() / 1000000.0;//get current system time
			}
			else
			{
				usleep(1000*100);
				continue;
			}
		}
		do {
		} while(android_atomic_cmpxchg(videoDec->mThreadExited, !videoDec->mThreadExited, &videoDec->mThreadExited));
		mRendererProcess->destroy();
		videoDec->mVideoQueue->PacketQueue_Abort();
		LOGI("VideoPlayer::decode end");
	}
	LOGI("VideoPlayer::decode out");
	return 0;
}


