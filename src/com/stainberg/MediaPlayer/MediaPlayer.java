package com.stainberg.MediaPlayer;

import java.lang.ref.WeakReference;

import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;

public class MediaPlayer {

	private native void native_init();
	private native void native_finalize();
	private native void native_setup(Object weak_thiz);
	private native void setDataSource(String sourcePath);
	private native void _prepare();
	private native void _start();
	private native void _stop();
	private native void _pause();
	private native void _seekTo(double pos);
	private native double _getDuration();
	private native double _getCurrentPosition();
	private native boolean _isPlaying();
	private native void _setSurface(Object surface);
	private native String getVersion();
	private Surface mNativeSurface;
	private static Handler mHandler;
	public final static int MSG_TEST = 0;
	public final static int MSG_FILE_OPEN_OK = 1;
	public final static int MSG_MOVIE_PREPARE_OK = 2;
	public final static int MSG_PLAYING = 3;
	public final static int MSG_PAUSE = 4;
	public final static int MSG_BUFFERING = 5;
	public final static int MSG_BUFFERED = 6;
	public final static int MSG_STOP = 7;
	public final static int MSG_RELEASE_MEDIAPLAYER = 8;
	public final static int MSG_POSISION = 9;
	public final static int MSG_DURATION = 10;
	public final static int MSG_PLAY_COMPLETE = 11;
	public final static int MSG_SEEK_ERROR = 12;
	public final static int MSG_SEEK_START = 13;
	public final static int MSG_SEEK_COMPLITION = 14;
	public final static int MSG_ERROR = 15;
	public final static int MSG_ERROR_FIXED = 16;
	
	public void surface_init(SurfaceHolder sh) {
		mNativeSurface = sh.getSurface();
		_setSurface(mNativeSurface);
	}
	
	public MediaPlayer() {
		native_setup(new WeakReference<MediaPlayer>(this));
	}
	
	public void releae() {
		native_finalize();
	}
	
	public void stop() {
		_stop();
	}
	
	public void pause() {
		_pause();
	}
	
	public int getDuration() {
		return (int)_getDuration();
	}
	
	public int getCurrentPosition() {
		return (int)_getCurrentPosition();
	}
	
	public void init() {
		native_init();
	}
	
	public void setSource(String sourcePath) {
		setDataSource(sourcePath);
	}
	
	public void prepare() {
		_prepare();
	}
	
	public void start() {
		_start();
	}
	
	public void seekTo(int position) {
		_seekTo((double)position);
	}
	
	public String Version() {
		return getVersion();
	}
	
	static {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("SumaPlayer");
	}
	
	public void setLogout(Handler handler) {
		mHandler = handler;
	}
	
	private static void postEventFromNative(Object weak_thiz, int msg, int arg0, String arg1) {
		Log.v("123", "msg = " + msg + "arg0 = " + arg0 + "arg1 = " + arg1);
		Message msg0 = new Message();
		msg0.what = msg;
		msg0.arg1 = arg0;
		msg0.obj = arg1;
		mHandler.sendMessage(msg0);
		if(mSurfaceListener == null) {
			return;
		}
	}
	
	public interface onSurfaceInitListener {
		public void onSurfaceInitCompletionListener();
	}
	
	public void setSurfaceListener(onSurfaceInitListener l) {
		mSurfaceListener = l;
	}
	static onSurfaceInitListener mSurfaceListener;
}
