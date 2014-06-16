/*
 * audioPlayer.cpp
 *
 *  Created on: 2013-1-3
 *      Author: stainberg
 */
#include  "myLog.h"
#include "audioPlayer.h"
#include <assert.h>

#ifdef TAG
#undef TAG
#endif
#define TAG "audioPlayer.cpp"

uint8_t audio_buf[(AVCODEC_MAX_AUDIO_FRAME_SIZE * 3) / 2];

AudioPlayer* mAudioProcess;

AudioPlayer::AudioPlayer()
{
	mAudioProcess = this;
	mListener = NULL;
	audio_st_index = -1;
	engineObject = NULL;
	engineEngine = NULL;
	outputMixObject = NULL;
	bqPlayerObject = NULL;
	bqPlayerPlay = NULL;
	bqPlayerBufferQueue = NULL;
	frame=avcodec_alloc_frame();
	av_init_packet(&packet);
	packet.size = 0;

}

AudioPlayer::AudioPlayer(my_sp<VideoState> vs)
{
	mAudioProcess = this;
	mListener = NULL;
	engineObject = NULL;
	engineEngine = NULL;
	outputMixObject = NULL;
	bqPlayerObject = NULL;
	bqPlayerPlay = NULL;
	bqPlayerBufferQueue = NULL;
	frame=avcodec_alloc_frame();
	av_init_packet(&packet);
	packet.size = 0;
	mVs = vs;
	audio_st_index = mVs->getAudioIndex();
	createEngine();
	createBufferQueueAudioPlayer();
}

AudioPlayer::~AudioPlayer()
{
	SLresult result;
	result = (*bqPlayerBufferQueue)->Clear(bqPlayerBufferQueue);
	assert(SL_RESULT_SUCCESS == result);
	(*bqPlayerObject)->Destroy(bqPlayerObject);
	(*outputMixObject)->Destroy(outputMixObject);
	(*engineObject)->Destroy(engineObject);
	mListener = NULL;
	mAudioProcess = NULL;
	engineObject = NULL;
	engineEngine = NULL;
	outputMixObject = NULL;
	bqPlayerObject = NULL;
	bqPlayerPlay = NULL;
	bqPlayerBufferQueue = NULL;
	mAudioQueue = NULL;
	mVs = NULL;
	av_free(frame);
	LOGI("AudioPlayer::~AudioPlayer()");
}

void AudioPlayer::createEngine()//create
{
      SLresult result;
      result = slCreateEngine(&engineObject, 0, NULL, 0, NULL, NULL);
      assert(SL_RESULT_SUCCESS == result);
      result = (*engineObject)->Realize(engineObject, SL_BOOLEAN_FALSE);
      assert(SL_RESULT_SUCCESS == result);
      result = (*engineObject)->GetInterface(engineObject, SL_IID_ENGINE, &engineEngine);
      assert(SL_RESULT_SUCCESS == result);
      const SLInterfaceID ids[1] = {SL_IID_PLAYBACKRATE};
      const SLboolean req[1] = {SL_BOOLEAN_FALSE};
      result = (*engineEngine)->CreateOutputMix(engineEngine, &outputMixObject, 1, ids, req);
      assert(SL_RESULT_SUCCESS == result);
      result = (*outputMixObject)->Realize(outputMixObject, SL_BOOLEAN_FALSE);
      assert(SL_RESULT_SUCCESS == result);
      LOGI("createEngine done");
}

void AudioPlayer::createBufferQueueAudioPlayer()//create
{
    SLresult result;
    AVCodecContext *decCtx = mVs->pFormatCtx->streams[mVs->getAudioIndex()]->codec;
    SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, 2};
    SLDataFormat_PCM pcm;
    pcm.formatType = SL_DATAFORMAT_PCM;
    if(decCtx->channels == 1)
    {
    	pcm.channelMask = SL_SPEAKER_FRONT_LEFT;
    	pcm.numChannels = decCtx->channels;
		pcm.samplesPerSec = decCtx->sample_rate*1000;
		pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
		pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    }
    else if(decCtx->channels == 2)
    {
    	pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    	pcm.numChannels = decCtx->channels;
		pcm.samplesPerSec = decCtx->sample_rate*1000;
		pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
		pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    }
    else if(decCtx->channels == 0)
    {
    	pcm.channelMask = SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT;
    	pcm.numChannels = 2;
		pcm.samplesPerSec = SL_SAMPLINGRATE_44_1;
		pcm.bitsPerSample = SL_PCMSAMPLEFORMAT_FIXED_16;
		pcm.containerSize = SL_PCMSAMPLEFORMAT_FIXED_16;
    }
    pcm.endianness = SL_BYTEORDER_LITTLEENDIAN;
    SLDataSource audioSrc = {&loc_bufq, &pcm};
    SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputMixObject};
    SLDataSink audioSnk = {&loc_outmix, NULL};
    const SLInterfaceID ids[1] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
    const SLboolean req[1] = {SL_BOOLEAN_TRUE};
    result = (*engineEngine)->CreateAudioPlayer(engineEngine, &bqPlayerObject, &audioSrc, &audioSnk, 1, ids, req);
    assert(SL_RESULT_SUCCESS == result);
    result = (*bqPlayerObject)->Realize(bqPlayerObject, SL_BOOLEAN_FALSE);
    assert(SL_RESULT_SUCCESS == result);
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_PLAY, &bqPlayerPlay);
    assert(SL_RESULT_SUCCESS == result);
    result = (*bqPlayerObject)->GetInterface(bqPlayerObject, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &bqPlayerBufferQueue);
    assert(SL_RESULT_SUCCESS == result);
    result = (*bqPlayerBufferQueue)->RegisterCallback(bqPlayerBufferQueue, AudioPlayer::playerCallback, NULL);//set callback
    assert(SL_RESULT_SUCCESS == result);
    result = (*bqPlayerPlay)->SetPlayState(bqPlayerPlay, SL_PLAYSTATE_PLAYING);
    assert(SL_RESULT_SUCCESS == result);
    LOGI("createBufferQueueAudioPlayer done");
}

void AudioPlayer::playerCallback(SLAndroidSimpleBufferQueueItf bq, void *context)//callback to play audio
{
//	LOGI("LINE = %d",__LINE__);
	int seekReq = 0;
	if(mAudioProcess->getThreadStatusInterrupt())
	{
//		LOGI("AudioPlayer::Interrupt");
		mAudioProcess->mAudioQueue->PacketQueue_Abort();
		do {
//			LOGI("1 AudioPlayer::android_atomic_cmpxchg");
		} while(android_atomic_cmpxchg(mAudioProcess->mThreadExited, !mAudioProcess->mThreadExited, &mAudioProcess->mThreadExited));
		LOGI("AudioPlayer::decode end");
		return;
	}
	int len = mAudioProcess->decode_frame(audio_buf);
	while(!len) {
//		LOGI("LINE = %d",__LINE__);
		if(mAudioProcess->getThreadStatusInterrupt())
		{
			mAudioProcess->mAudioQueue->PacketQueue_Abort();
			do {
//				LOGI("2 AudioPlayer::android_atomic_cmpxchg");
			} while(android_atomic_cmpxchg(mAudioProcess->mThreadExited, !mAudioProcess->mThreadExited, &mAudioProcess->mThreadExited));
			LOGI("AudioPlayer::decode end");
			return;
		}
		usleep(1000);
		continue;
	}
	(*mAudioProcess->bqPlayerBufferQueue)->Enqueue(mAudioProcess->bqPlayerBufferQueue, audio_buf, len);
}

int AudioPlayer::decode_frame(uint8_t *stream)//decode packet for callback play
{
	AVPacket pkt1, *pkt = &pkt1;
	AVPacket *tmp_pkt = &packet;
	int bytes_per_sec;
	AVCodecContext *dec = mVs->getAVStream(audio_st_index)->codec;
	int ret = -1, flush_complete = 0, new_packet = 0, got_frame = 0, len = 0, data_size = 0;
	double audio_callback_time = av_gettime()/1000000.0;
	if(mAudioQueue.get() != NULL)
	{
		while(!getThreadStatusInterrupt())
		{
//			LOGI("LINE = %d",__LINE__);
			if(mVs->mState == MEDIA_PLAYER_PAUSED) {
				usleep(1000*100);
				continue;
			}
			while(mVs->seekReq) {
				usleep(1000);
				continue;
			}
//			if(!mVs->syncVideoFlag) {
//				usleep(1000);
//				continue;
//			}
			if(mAudioQueue->PacketQueue_Get_nb_Pkt() > 0)
			{
				if(tmp_pkt->size <= 0 || !tmp_pkt->data)
				{
					new_packet = mAudioProcess->mAudioQueue->PacketQueue_Get(pkt);
					if (pkt->data == mAudioProcess->mAudioQueue->flush_pkt.data)
					{
						avcodec_flush_buffers(mVs->audio_st->codec);
						tmp_pkt->size = 0;
						continue;
					}
					*tmp_pkt = *pkt;
					if(tmp_pkt->pts != AV_NOPTS_VALUE)
					{
						mVs->audio_clock = av_q2d(mVs->audio_st->time_base)*pkt->pts;
					}
					mVs->position = mVs->audio_clock;
				}
				while(new_packet)
				{
//					LOGI("LINE = %d",__LINE__);
					if(flush_complete)
					{
						data_size = av_samples_get_buffer_size(NULL, dec->channels, frame->nb_samples, dec->sample_fmt, 1);
						memcpy(stream, frame->data[0], data_size);
						mVs->audio_clock += (double)data_size / (dec->channels * dec->sample_rate * av_get_bytes_per_sample(dec->sample_fmt));
						bytes_per_sec = dec->sample_rate * dec->channels * av_get_bytes_per_sample(dec->sample_fmt);
						mVs->audio_current_pts = mVs->audio_clock;
						mVs->audio_current_pts_drift = mVs->audio_current_pts - audio_callback_time;
						av_free_packet(pkt);

						if(mVs->get_video_clock() - mVs->get_audio_clock() > 0.5) {
							memset(stream, 0, 4096);
							return 4096;
						}
						if(mVs->seekReq_2 == 1) {
							mVs->seekReq_2 = 0;
							mListener->notify(pthread_self(), MSG_SEEK_COMPLITION, 1, "MSG_SEEK_COMPLITION");
						}
						return data_size;
					}
					len = avcodec_decode_audio4(dec, frame, &got_frame, tmp_pkt);

					if(len < 0)
					{
						tmp_pkt->size = 0;
						av_free_packet(pkt);
						break;
					}
					tmp_pkt->size -= len;
					tmp_pkt->data += len;
					if(got_frame)
					{
						if (tmp_pkt->size <= 0)
						{
							flush_complete = 1;
							continue;
						}
					}
					else
					{
						if (tmp_pkt->size <= 0)
						{
							flush_complete = 1;
							continue;
						}
					}
				}
				flush_complete = 0;
			}
			else
			{
				LOGI("continue");
				usleep(1000*1000);
				continue;
			}
		}
	}
	memset(stream, 0, 4096);
	return 4096;
}

void* AudioPlayer::decode(void* ptr)
{
	LOGI("AudioPlayer::decode start");
	mAudioProcess->playerCallback(NULL, NULL);
	return 0;
}



