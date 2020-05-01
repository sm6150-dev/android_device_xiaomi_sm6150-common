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

package org.lineageos.settings.doze;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.AsyncTask;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

class GxzwSensor {
    private static int TYPE_NONUI_SENSOR = 33171027;
    private static int TYPE_FOD_SENSOR = 33171030;
    private final Context mContext;
    private final Handler mHandler = new Handler(Looper.getMainLooper());
    private static final String TAG = "GxzwSensor";
    public GxzwSensorListener mGxzwSensorListener;
    private final SensorManager mSensorManager;

    interface GxzwSensorListener {
        void onDeviceMove();
        void onDeviceFodDetector();
        void onDeviceStable();
        void onExitNonUI(int i);
        void onEnterNonUI(int i);
    }

    private final SensorEventListener mNonUIListener = new SensorEventListener() {
        public void onAccuracyChanged(Sensor sensor, int i) {
        }

        public void onSensorChanged(SensorEvent sensorEvent) {
            if (sensorEvent != null && mGxzwSensorListener != null) {
                float[] fArr = sensorEvent.values;
                if (fArr[0] == 0.0f) {
                    mGxzwSensorListener.onExitNonUI((int) sensorEvent.values[0]);
                } else if (fArr[0] == 1.0f || fArr[0] == 2.0f) {
                    mGxzwSensorListener.onEnterNonUI((int) sensorEvent.values[0]);
                } else {
                    Log.w(TAG, "event.values[0] = " + sensorEvent.values[0]);
                }
            }
        }
    };

    private final SensorEventListener mFodDetectorSensorListener = new SensorEventListener() {
        public void onAccuracyChanged(Sensor sensor, int i) {
        }

        public void onSensorChanged(SensorEvent sensorEvent) {
            if (sensorEvent != null && mGxzwSensorListener != null) {
                float[] fArr = sensorEvent.values;
                if (fArr[0] == 1.0f) {
                    mGxzwSensorListener.onDeviceMove();
                } else if (fArr[0] == 2.0f) {
                    mGxzwSensorListener.onDeviceStable();
                } else if (fArr[0] == 3.0f) {
                    mGxzwSensorListener.onDeviceFodDetector();
                } else {
                    Log.w(TAG, "event.values[0] = " + sensorEvent.values[0]);
                }
            }
        }
    };

    GxzwSensor(Context context) {
        mContext = context;
        mSensorManager = (SensorManager) context.getSystemService("sensor");
    }

    public void registerDozeSensor() {
        if (mSensorManager == null) {
            Log.e(TAG, "sensor not supported");
            return;
        }
        if (DozeUtils.isAlwaysOnEnabled(mContext)) {
            AsyncTask.THREAD_POOL_EXECUTOR.execute(new Runnable() {
                public final void run() {
                    doRegisterDozeSensor();
                }
            });
        }
    }

    public void unregisterDozeSensor() {
        mGxzwSensorListener = null;
        if (mSensorManager == null) {
            Log.e(TAG, "sensor not supported");
        } else {
            AsyncTask.THREAD_POOL_EXECUTOR.execute(new Runnable() {
                public final void run() {
                    doUnregisterSensor();
                }
            });
        }
    }

    private void doRegisterDozeSensor() {
        Sensor foddetectorSensor = mSensorManager.getDefaultSensor(TYPE_FOD_SENSOR, true);
        if (foddetectorSensor != null) {
            mSensorManager.registerListener(mFodDetectorSensorListener, foddetectorSensor, 3, mHandler);
        } else {
            Log.e(TAG, "no fod sensor");
        }
        Sensor nonuiSensor = mSensorManager.getDefaultSensor(TYPE_NONUI_SENSOR, true);
        if (nonuiSensor != null) {
            mSensorManager.registerListener(mNonUIListener, nonuiSensor, 3, mHandler);
        } else {
            Log.e(TAG, "no nonui sensor");
        }
    }

    private void doUnregisterSensor() {
        mSensorManager.unregisterListener(mFodDetectorSensorListener);
        mSensorManager.unregisterListener(mNonUIListener);
    }
}
