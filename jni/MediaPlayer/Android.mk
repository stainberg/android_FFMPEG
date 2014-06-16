LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

#LOCAL_MODULE    := SumaPlayer_no_neon
LOCAL_MODULE    := SumaPlayer

LOCAL_CFLAGS := -D__STDC_CONSTANT_MACROS
LOCAL_CFLAGS += -fexceptions

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include \
											$(LOCAL_PATH)/../libffmpeg
											
LOCAL_SRC_FILES := listener.cpp \
										decodec.cpp \
										mediaPlayer.cpp \
										audioPlayer.cpp \
										videoPlayer.cpp \
										onLoad.cpp \
										../libffmpeg/cmdutils.c \
										renderer.cpp \
										com_stainberg_MediaPlayer_MediaPlayer.cpp
	
#LOCAL_SHARED_LIBRARIES := libffmpeg_no_neon
LOCAL_SHARED_LIBRARIES := libffmpeg

LOCAL_LDLIBS := -llog -lOpenSLES -lEGL -lGLESv1_CM -lGLESv2 -lcutils -lutils -landroid -ldl
#LOCAL_STATIC_LIBRARIES
#LOCAL_SHARED_LIBRARIES
include $(BUILD_SHARED_LIBRARY)
