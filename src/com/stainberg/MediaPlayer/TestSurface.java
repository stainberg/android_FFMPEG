package com.stainberg.MediaPlayer;

import android.content.Context;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class TestSurface extends SurfaceView {

	public TestSurface(Context context, AttributeSet attrs, int defStyle) {
		super(context, attrs, defStyle);
		SurfaceHolder surfHolder = getHolder();
		surfHolder.addCallback(mSHCallback);
		// TODO Auto-generated constructor stub
	}

	public TestSurface(Context context, AttributeSet attrs) {
		super(context, attrs);
		SurfaceHolder surfHolder = getHolder();
		surfHolder.addCallback(mSHCallback);
		// TODO Auto-generated constructor stub
	}

	public TestSurface(Context context) {
		super(context);
		SurfaceHolder surfHolder = getHolder();
		surfHolder.addCallback(mSHCallback);
		// TODO Auto-generated constructor stub
	}
	
	public void setListener(onSurfaceListener l) {
		mListener = l;
	}
	
    SurfaceHolder.Callback mSHCallback = new SurfaceHolder.Callback() {
        public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
        	if(mListener != null)
        		mListener.onSurfaceChangedCompletionListener(holder, w, h);
        }

        public void surfaceCreated(SurfaceHolder holder) {
        	if(mListener != null)
        		mListener.onSurfaceInitCompletionListener(holder);
        }

        public void surfaceDestroyed(SurfaceHolder holder) {
        	
        }
    };
    
    private onSurfaceListener mListener;
    
    public interface onSurfaceListener {
    	void onSurfaceInitCompletionListener(SurfaceHolder holder);
    	void onSurfaceChangedCompletionListener(SurfaceHolder holder, int width, int height);
    }

}
