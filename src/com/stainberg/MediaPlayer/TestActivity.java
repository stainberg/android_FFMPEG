package com.stainberg.MediaPlayer;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.OutputStream;

import android.app.Activity;
import android.content.Context;
import android.net.wifi.WifiManager;
import android.net.wifi.WifiManager.MulticastLock;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;

import com.stainberg.MediaPlayer.TestSurface.onSurfaceListener;
import com.stainberg.MediaPlayer.R;

public class TestActivity extends Activity implements OnClickListener, OnSeekBarChangeListener {
	
	private Context mContext;
	private ScrollView mOut;
	private LinearLayout mOut2;
	private TextView mTextLogcat;
//	private ProgressDialog mDlg;
	private MediaPlayer mTest;
	private Button mBtn, mBtnSetFile;
	private Button mBtnFinalize;
	private Button mBtnStop;
	private Button mBtnPlayPause;
	private TestSurface mView;//http://222.33.67.38:1935/dvr/smil:test.smil/playlist.m3u8?DVR
	private SeekBar mSeekbar1;//"http://222.33.67.38:1935/dvr/20853.stream/playlist.m3u8";
	private int mDuration = 0;//Environment.getExternalStorageDirectory().toString() + File.separator + name;
	MulticastLock multicastLock;//http://222.33.67.38:1935/dvr/30701.stream/playlist.m3u8?DVR
	private int mPosition = 0;//http://192.166.62.10:1935/vod/_definst_/mp4:fast_and_furious/fast_and_furious_5.mp4/playlist.m3u8
	private TextView posision, duration;//http://192.166.62.138/SumaPlayer/video/2.mp4
	private EditText mEdit;//http://192.166.62.23:1935/dvr/100.stream/playlist.m3u8?DVR
	private final int MSG_PROGRESS = 0xAAAA;// http://192.166.62.23:1935/dvr/test1.stream/playlist.m3u8?DVR
	//http://111.8.54.40:1935/dvr/103103.stream/playlist.m3u8?DVR
	private final String name = "[阳光电影www.ygdy8.com].机械战警.BD.720p.中英双字幕.rmvb";
	private String url = "http://10.155.58.163/[阳光电影www.ygdy8.com].机械战警.BD.720p.中英双字幕.rmvb";
	private Handler hander = new Handler() {
	public void handleMessage(Message msg) {
		if(msg.what != MSG_PROGRESS)
			mTextLogcat.setText(mTextLogcat.getText() + "MSG_CODE = " + msg.arg1 + " & MSG = " + (String)msg.obj + "\n");
		switch(msg.what) {
		case MediaPlayer.MSG_TEST:
			Toast.makeText(mContext, msg.arg1 + "<-------" + (String)msg.obj, Toast.LENGTH_SHORT).show();
			break;
		case 9:
			break;
		case 10:
			break;
		case MediaPlayer.MSG_FILE_OPEN_OK:
			mTest.prepare();
			break;
		case MediaPlayer.MSG_MOVIE_PREPARE_OK:
			mDuration = mTest.getDuration();
			posision.setText("0");
			duration.setText(String.valueOf(mTest.getDuration()));
			hander.sendEmptyMessageDelayed(MSG_PROGRESS, 1000);
			mTest.start();
			break;
		case MediaPlayer.MSG_BUFFERING:
//			mDlg.show();
			break;
		case MediaPlayer.MSG_BUFFERED:
//			mDlg.dismiss();
			break;
		case MSG_PROGRESS:
			mPosition++;
			posision.setText(String.valueOf(mPosition));
			mSeekbar1.setProgress((int)((double)((double)mPosition/(double)mDuration)*100));
			hander.sendEmptyMessageDelayed(MSG_PROGRESS, 1000);
			break;
			}
		}
	};
	
	private void allowMulticast() {
        WifiManager wifiManager=(WifiManager)getSystemService(Context.WIFI_SERVICE);
        multicastLock=wifiManager.createMulticastLock("multicast.test");
        multicastLock.acquire();
    }
	
	@Override
	protected void onDestroy() {
		super.onDestroy();
//		multicastLock.release();
	}

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_test);
//		allowMulticast();
		mContext = this;
//		copyFile();
//		mDlg = new ProgressDialog(this);
//		mDlg.setCanceledOnTouchOutside(false);
		
		mOut = (ScrollView)findViewById(R.id.scrollView1);
		mOut2 = (LinearLayout)findViewById(R.id.layout2);
		mEdit = (EditText)findViewById(R.id.editText1);
		mTextLogcat = (TextView)findViewById(R.id.textView1);
		mTextLogcat.setText("");
		mEdit.setText(url);
		mBtn = (Button)findViewById(R.id.button1);
		mBtn.setId(0xE001);
		mBtn.setOnClickListener(this);
		mBtnSetFile = (Button)findViewById(R.id.button2);
		mBtnSetFile.setId(0xE002);
		mBtnSetFile.setOnClickListener(this);
		mBtnFinalize = (Button)findViewById(R.id.button3);
		mBtnFinalize.setId(0xE003);
		mBtnFinalize.setOnClickListener(this);
		mBtnStop = (Button)findViewById(R.id.button5);
		mBtnStop.setId(0xE005);
		mBtnStop.setOnClickListener(this);
		mBtnPlayPause = (Button)findViewById(R.id.button6);
		mBtnPlayPause.setId(0xE006);
		mBtnPlayPause.setOnClickListener(this);
		posision = (TextView)findViewById(R.id.textView2);
		duration = (TextView)findViewById(R.id.textView3);
		mSeekbar1 = (SeekBar)findViewById(R.id.seekBar1);
		mSeekbar1.setMax(100);
		mSeekbar1.setOnSeekBarChangeListener(this);
		mTest = new MediaPlayer();
		mTest.init();
		mTest.setLogout(hander);
		
		mView = (TestSurface)findViewById(R.id.testSurface1);
		mView.setListener(new onSurfaceListener() {

			@Override
			public void onSurfaceInitCompletionListener(SurfaceHolder holder) {

			}

			@Override
			public void onSurfaceChangedCompletionListener(SurfaceHolder holder, int width, int height) {
				mTest.surface_init(mView.getHolder());
			}
			
		});
		
	}

	private boolean copyFile() {
		try {
			int byteread = 0;
			String destPath = Environment.getExternalStorageDirectory().toString() + File.separator + name;
			InputStream inStream = this.getResources().getAssets().open(name);
			OutputStream fs = new BufferedOutputStream(new FileOutputStream(destPath));
			byte[] buffer = new byte[8192];
			while ((byteread = inStream.read(buffer)) != -1) {
				fs.write(buffer, 0, byteread);
			}
			inStream.close();
			fs.close();
			return true;
		} catch (Exception e) {
			return false;
		}
	}
	
	@Override
	public void onClick(View v) {
		if(v.getId() == mBtn.getId()) {
			mTextLogcat.setText("");
			//mnt/sdcard/234.mp4
			//http://192.166.62.10:1935/vod/_definst_/mp4:THM_OVTA0000000000004379/mp4_2/OVTA0000000000004379.mp4/playlist.m3u8
			mTest.setSource(mEdit.getText().toString());//http://192.166.62.46:1935/dvr/102.stream/playlist.m3u8?DVR
		}
		if(v.getId() == mBtnFinalize.getId()) {
			mTest.releae();
		}
		if(v.getId() == mBtnStop.getId()) {
			mTest.stop();
		}
		if(v.getId() == mBtnPlayPause.getId()) {
			mTest.pause();
			Toast.makeText(this, mTest.Version(), Toast.LENGTH_SHORT).show();
		}
		if(mBtnSetFile == v) {
			if(mEdit.getVisibility() == View.VISIBLE) {
				mEdit.setVisibility(View.GONE);
			} else {
				mEdit.setVisibility(View.VISIBLE);
			}
		}
		if(mOut == v) {
			if(mOut2.getVisibility() == View.VISIBLE) {
				mOut2.setVisibility(View.INVISIBLE);
			} else {
				mOut2.setVisibility(View.VISIBLE);
			}
		}
	}

	@Override
	public void onProgressChanged(SeekBar arg0, int arg1, boolean arg2) {
		
	}

	@Override
	public void onStartTrackingTouch(SeekBar arg0) {
		
	}

	@Override
	public void onStopTrackingTouch(SeekBar arg0) {
		double tmp = (double)((double)arg0.getProgress()/100);
		int result = (int)(tmp*mDuration);
		mTest.seekTo(result);
	}
}
