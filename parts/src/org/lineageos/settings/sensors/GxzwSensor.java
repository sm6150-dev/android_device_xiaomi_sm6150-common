/*
 * Copyright (C) 2020 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package org.lineageos.settings.sensors;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Looper;

import org.lineageos.settings.doze.DozeUtils;
import org.lineageos.settings.sensors.SensorsUtils;

public class GxzwSensor {
    private static final String TAG = "GxzwSensor";
    private final Context mContext;
    private final Handler mHandler = new Handler(Looper.getMainLooper());
    private final SensorManager mSensorManager;
    public GxzwSensorListener mGxzwSensorListener;
    private final SensorEventListener mFodDetectorSensorListener = new SensorEventListener() {
        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            /* Empty */
        }

        @Override
        public void onSensorChanged(SensorEvent event) {
            if (event != null && mGxzwSensorListener != null) {
                if (event.values[0] == 1 || event.values[0] == 3) {
                    mGxzwSensorListener.onDeviceFodDetector();
                }
            }
        }
    };
    private final SensorEventListener mNonUIListener = new SensorEventListener() {
        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {
            /* Empty */
        }

        @Override
        public void onSensorChanged(SensorEvent event) {
            if (event != null && mGxzwSensorListener != null) {
                if (event.values[0] == 0) {
                    mGxzwSensorListener.onNonUI((0));
                } else if (event.values[0] == 1 || event.values[0] == 2) {
                    mGxzwSensorListener.onNonUI((1));
                }
            }
        }
    };
    private int mIsInNonUIMode = 0;
    private Sensor mNonUiSensor;
    private Sensor mFodSensor;

    public GxzwSensor(Context context) {
        mContext = context;
        mSensorManager = mContext.getSystemService(SensorManager.class);
        mFodSensor = SensorsUtils.getSensor(mSensorManager, "xiaomi.sensor.fod_detector");
        mNonUiSensor = SensorsUtils.getSensor(mSensorManager, "xiaomi.sensor.nonui");
    }

    private void onDeviceFodDetector() {
        if (mIsInNonUIMode == 0) {
        DozeUtils.wakeupScreen(mContext);
        }
    }

    private void setNonUIMode(int enable) {
        if (mIsInNonUIMode != enable) {
            mIsInNonUIMode = enable;
            AsyncTask.THREAD_POOL_EXECUTOR.execute(new Runnable() {
                public final void run() {
                    SensorsUtils.setTouchMode(Constants.TOUCH_NONUI_MODE, enable);
                }
            });
        }
    }

    private void onNonUI(boolean enable) {
        setNonUIMode(enable ? 1:0);
    }

    public void enable() {
        if (mFodSensor == null || mNonUiSensor == null) {
            return;
        }
        AsyncTask.THREAD_POOL_EXECUTOR.execute(new Runnable() {
            public final void run() {
                if (mFodSensor != null) {
                    mSensorManager.registerListener(mFodDetectorSensorListener, mFodSensor, SensorManager.SENSOR_DELAY_NORMAL, mHandler);
                }
                if (mNonUiSensor != null) {
                    mSensorManager.registerListener(mNonUIListener, mNonUiSensor, SensorManager.SENSOR_DELAY_NORMAL, mHandler);
                }
            }
        });
    }

    public void disable() {
        mGxzwSensorListener = null;
        if (mFodSensor == null || mNonUiSensor == null) {
            // do nothing
        } else {
            AsyncTask.THREAD_POOL_EXECUTOR.execute(new Runnable() {
                public final void run() {
                    mSensorManager.unregisterListener(mFodDetectorSensorListener);
                    mSensorManager.unregisterListener(mNonUIListener);
                }
            });
        }
    }

    private interface GxzwSensorListener {
        void onDeviceFodDetector();

        void onNonUI(int enable);
    }
}
