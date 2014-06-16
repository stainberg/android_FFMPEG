#!/bin/bash
#PREBUILT=/home/damon/android-ndk-r6b/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86
#PLATFORM=/home/damon/android-ndk-r6b/platforms/android-9/arch-arm

PREBUILT=/home/stainberg/linux/android-ndk-r8/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86
PLATFORM=/home/stainberg/linux/android-ndk-r8/platforms/android-9/arch-arm


./configure \
--enable-static \
--enable-asm \
--cpu=cortex-a8 \
--arch=armv7 \
--enable-neon \
--disable-shared \
--enable-gpl \
--enable-version3 \
--enable-nonfree \
--disable-doc \
--disable-ffmpeg \
--disable-ffplay \
--disable-ffprobe \
--disable-ffserver \
--disable-avdevice \
--disable-swresample \
--disable-postproc \
--disable-avfilter \
--disable-encoders \
--disable-hwaccels \
--disable-muxers \
--disable-indevs \
--disable-outdevs \
--disable-devices \
--disable-filters \
--disable-decoders \
--enable-decoder=aac \
--enable-decoder=aac_latm \
--enable-decoder=ac3 \
--enable-decoder=mp3 \
--enable-decoder=mpeg1video \
--enable-decoder=mpeg2video \
--enable-decoder=mpeg4 \
--enable-decoder=mpegvideo \
--enable-decoder=amrnb \
--enable-decoder=amrwb \
--enable-decoder=h264 \
--disable-parsers \
--enable-parser=aac \
--enable-parser=mpegvideo \
--enable-parser=aac_latm \
--enable-parser=ac3	\
--enable-parser=h264 \
--enable-parser=mpeg4video \
--enable-parser=mpegaudio \
--disable-demuxers \
--enable-demuxer=aac \
--enable-demuxer=ac3 \
--enable-demuxer=amr \
--enable-demuxer=rawvideo \
--enable-demuxer=latm \
--enable-demuxer=rtp \
--enable-demuxer=rtsp \
--enable-demuxer=sdp \
--enable-demuxer=mov \
--enable-demuxer=mp3 \
--enable-demuxer=mpegts \
--enable-demuxer=mpegvideo \
--enable-demuxer=g722 \
--enable-demuxer=g723_1 \
--enable-demuxer=g729 \
--enable-demuxer=h264 \
--enable-demuxer=hls \
--disable-protocols \
--enable-protocol=file \
--enable-protocol=http \
--enable-protocol=tcp \
--enable-protocol=udp \
--enable-protocol=rtp \
--enable-protocol=applehttp \
--enable-protocol=hls \
--disable-debug \
--cross-prefix=$PREBUILT/bin/arm-linux-androideabi- \
--enable-cross-compile \
--target-os=linux \
--extra-cflags='-I$PLATFORM/usr/include -mfpu=neon -mfloat-abi=softfp' \
--extra-ldflags='-L$PLATFORM/usr/lib -nostdlib'  

sed -i 's/HAVE_LRINT 0/HAVE_LRINT 1/g' config.h
sed -i 's/HAVE_LRINTF 0/HAVE_LRINTF 1/g' config.h
sed -i 's/HAVE_ROUND 0/HAVE_ROUND 1/g' config.h
sed -i 's/HAVE_ROUNDF 0/HAVE_ROUNDF 1/g' config.h
sed -i 's/HAVE_TRUNC 0/HAVE_TRUNC 1/g' config.h
sed -i 's/HAVE_TRUNCF 0/HAVE_TRUNCF 1/g' config.h

##--disable-decoders 
#--enable-decoder=aac 
#--enable-decoder=aac_latm 
#--enable-decoder=ac3 
#--enable-decoder=mp3 
#--enable-decoder=mpeg1video 
#--enable-decoder=mpeg2video 
#--enable-decoder=mpeg4 
#--enable-decoder=mpegvideo 
#--enable-decoder=amrnb 
#--enable-decoder=amrwb 
#--enable-decoder=h264 
##--disable-parsers 
#--enable-parser=aac 
#--enable-parser=mpegvideo 
#--enable-parser=aac_latm 
#--enable-parser=ac3	
#--enable-parser=h264 
#--enable-parser=mpeg4video 
#--enable-parser=mpegaudio 
##--disable-demuxers 
#--enable-demuxer=aac 
#--enable-demuxer=ac3 
#--enable-demuxer=amr 
#--enable-demuxer=rawvideo 
#--enable-demuxer=latm 
#--enable-demuxer=rtp 
#--enable-demuxer=rtsp 
#--enable-demuxer=sdp 
#--enable-demuxer=mov 
#--enable-demuxer=mp3 
#--enable-demuxer=mpegts 
#--enable-demuxer=mpegvideo 
#--enable-demuxer=mtv
#--enable-demuxer=wav 
#--enable-demuxer=g722 
#--enable-demuxer=g723_1 
#--enable-demuxer=g729 
#--enable-demuxer=h264 
#--enable-demuxer=hls 
##--disable-protocols 
#--enable-protocol=file 
#--enable-protocol=http 
#--enable-protocol=tcp 
#--enable-protocol=udp 
#--enable-protocol=rtp 
#--enable-protocol=applehttp 
#--enable-protocol=hls 
#--enable-protocol=rtmp 
#--enable-protocol=httpproxy 
