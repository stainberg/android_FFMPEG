#!/bin/bash
#PREBUILT=/home/damon/android-ndk-r6b/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86
#PLATFORM=/home/damon/android-ndk-r6b/platforms/android-9/arch-arm

PREBUILT=/home/stainberg/linux/android-ndk-r8/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86
PLATFORM=/home/stainberg/linux/android-ndk-r8/platforms/android-9/arch-arm

./configure \
--enable-static \
--enable-asm \
--arch=arm \
--disable-shared \
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
--enable-decoder=libstagefright-h264 \
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
--enable-parser=libstagefright-h264 \
--enable-parser=aac \
--enable-parser=mpegvideo \
--enable-parser=aac_latm \
--enable-parser=ac3	\
--enable-parser=h264 \
--enable-parser=mpeg4video \
--enable-parser=mpegaudio \
--disable-demuxers \
--enable-demuxer=libstagefright-h264 \
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
--disable-neon \
--disable-debug \
--cross-prefix=$PREBUILT/bin/arm-linux-androideabi- \
--enable-cross-compile \
--target-os=linux \
--extra-cflags='-I$PLATFORM/usr/include -mfloat-abi=softfp' \
--extra-ldflags='-L$PLATFORM/usr/lib -nostdlib' \
--prefix=/mnt/hgfs/havlenapetr-FFMpeg0-8-1/bin

sed -i 's/HAVE_LRINT 0/HAVE_LRINT 1/g' config.h
sed -i 's/HAVE_LRINTF 0/HAVE_LRINTF 1/g' config.h
sed -i 's/HAVE_ROUND 0/HAVE_ROUND 1/g' config.h
sed -i 's/HAVE_ROUNDF 0/HAVE_ROUNDF 1/g' config.h
sed -i 's/HAVE_TRUNC 0/HAVE_TRUNC 1/g' config.h
sed -i 's/HAVE_TRUNCF 0/HAVE_TRUNCF 1/g' config.h

#--enable-hardcoded-tables 
#--disable-symver 
#--disable-debug 
#--disable-stripping
#--enable-small

#--enable-gpl 
#--enable-version3 
#--enable-nonfree 

#--cpu=cortex-a8 
#--arch=armv7 
#--enable-neon\
#--disable-armv5te 
#--disable-armv6 
#--disable-armv6t2

#./configure --target-os=linux \
#			--arch=arm \
#			--cpu=cortex-a8 \
#			--enable-gpl \
#			--enable-version3 \
#			--enable-nonfree \
#			--enable-sram \
#			--disable-optimizations \
#			--enable-hwaccel=h264_dxva2 \
#			--enable-hwaccel=h264_vaapi \
#			--disable-doc \
#			--disable-ffmpeg \
#			--disable-ffplay \
#			--disable-ffserver \
#			--disable-ffprobe \
#			--disable-encoders \
#   		--disable-muxers \
#	    	--disable-devices \
#			--disable-postproc \
#			--disable-network \
#			--disable-avdevice \
#			--disable-avfilter \
#			--disable-protocols \
#			--enable-protocol=file \
#			--enable-cross-compile \
#			--cc=$PREBUILT/bin/arm-linux-androideabi-gcc \
#			--cross-prefix=$PREBUILT/bin/arm-linux-androideabi- \
#			--nm=$PREBUILT/bin/arm-linux-androideabi-nm \
#		    --extra-cflags="-fPIC -DANDROID" \
#			--enable-static \
#			--disable-shared \
#			--prefix=/mnt/hgfs/havlenapetr-FFMpeg0-8-1/bin \
#			--extra-ldflags="-Wl,-T,$PREBUILT/arm-linux-androideabi/lib/ldscripts/armelf_linux_eabi.x -Wl,-rpath-link=$PLATFORM/usr/lib -L$PLATFORM/usr/lib -nostdlib $PREBUILT/lib/gcc/arm-linux-androideabi/4.4.3/crtbegin.o $PREBUILT/lib/gcc/arm-linux-androideabi/4.4.3/crtend.o -lc -lm -ldl"

