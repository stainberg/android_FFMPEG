/*
 * packetQueue.h
 *
 *  Created on: 2013-1-16
 *      Author: stainberg
 */

#ifndef PACKETQUEUE_H_
#define PACKETQUEUE_H_

#define MAX_PACKET_QUEUE 2048
#define MAX_SIZE 16777216 //16M data
#include "myLog.h"
#include "mutex.h"
#include "refBase.h"
#include <unistd.h>

#ifdef TAG
#undef TAG
#endif
#define TAG "packetQueue.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "libavformat/avformat.h"

class PacketQueue: public My_RefBase<PacketQueue> {
public:
	PacketQueue();
	inline virtual ~PacketQueue();
	int PacketQueue_Put(AVPacket* pkt);
	int PacketQueue_Get(AVPacket* pkt);
	void PacketQueue_Flush();
	inline void PacketQueue_Abort(){abort_request = 1; PacketQueue_Flush();};
	inline int PacketQueue_Get_nb_Pkt(){return nb_packets;};
	inline int PacketQueue_Get_Data_Size(){return size;};
private:
	AVPacketList* first_pkt;
	AVPacketList* last_pkt;
	int32_t nb_packets;
	int32_t size;
    uint32_t abort_request;
    pthread_mutex_t mutex;
public:
    AVPacket flush_pkt;

    int test ;
};

inline PacketQueue::PacketQueue() {
	test = 0;

	first_pkt = last_pkt = NULL;
	nb_packets = 0;
	size = 0;
	abort_request = 0;
	av_init_packet(&flush_pkt);
	flush_pkt.data = (uint8_t *)&flush_pkt;
	flush_pkt.size = 1;
	pthread_mutex_init(&mutex, NULL);
}

inline PacketQueue::~PacketQueue() {
	PacketQueue_Flush();
	first_pkt = last_pkt = NULL;
	nb_packets = 0;
	size = 0;
	abort_request = 0;
	pthread_mutex_destroy(&mutex);
	LOGI("PacketQueue::~PacketQueue()");
}

inline int PacketQueue::PacketQueue_Put(AVPacket* pkt) {
	pthread_mutex_lock(&mutex);
	int ret = -1;
	AVPacketList *pkt1;
	if(abort_request) {
		ret = -1;
		goto end;
	}
	if(pkt != &flush_pkt && av_dup_packet(pkt) < 0) {
		ret = -1;
		goto end;
	}
	pkt1 = (AVPacketList *)av_mallocz(sizeof(AVPacketList));
	if(!pkt1) {ret = -1; goto end;}
	pkt1->pkt = *pkt;
	pkt1->next = NULL;
	if(first_pkt == NULL) {
		first_pkt = pkt1;
	} else {
		last_pkt->next = pkt1;
	}
	last_pkt = pkt1;
	size += pkt->size;
	nb_packets++;
	ret = 1;
end:
	pthread_mutex_unlock(&mutex);
	return ret;
}

inline int PacketQueue::PacketQueue_Get(AVPacket* pkt) {
	pthread_mutex_lock(&mutex);
	int ret = -1;
	AVPacketList *pkt1;
//	if(test) {
//		if(pkt != &flush_pkt) {
//			LOGE("time = %f", pkt->pts/AV_TIME_BASE);
//			test = 0;
//		}
//	}
	if(abort_request) {
		ret = -1;
		goto end;
	}
	pkt1 = first_pkt;
	if(pkt1) {
		first_pkt = pkt1->next;
		if(first_pkt == NULL) {
			last_pkt = NULL;
		}
		*pkt = pkt1->pkt;
		av_free(pkt1);
		size -= pkt->size;
		nb_packets--;
	} else {
		ret = 0;
		goto end;
	}
	ret = 1;
end:
	pthread_mutex_unlock(&mutex);
	return ret;
}

inline void PacketQueue::PacketQueue_Flush() {
	pthread_mutex_lock(&mutex);
	AVPacketList *pkt, *pkt1;
//	test = 1;
	nb_packets = 0;
	size = 0;
    for (pkt = first_pkt; pkt != NULL; pkt = pkt1) {
        pkt1 = pkt->next;
        av_free_packet(&pkt->pkt);
        av_freep(&pkt);
    }
    last_pkt = NULL;
    first_pkt = NULL;
    pthread_mutex_unlock(&mutex);
}

#ifdef __cplusplus
}
#endif

#endif /* PACKETQUEUE_H_ */
