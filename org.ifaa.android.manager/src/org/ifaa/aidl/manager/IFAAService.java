package org.ifaa.aidl.manager;

import android.app.Service;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.SystemProperties;
import android.util.Log;

import org.json.JSONObject;

import java.lang.ref.WeakReference;
import java.util.ArrayList;
import java.util.Arrays;

import vendor.xiaomi.hardware.mlipay.V1_1.IMlipayService;

public class IFAAService extends Service {
    private static final boolean sIsFod = SystemProperties.getBoolean("ro.hardware.fp.fod", false);
    private final IBinder mIFAABinder = new IFAAServiceStub(this);
    private IMlipayService mService;

    public IFAAService() {
        Log.v("IFAAService", "IFAAService");
    }

    public void onCreate() {
        Log.v("IFAAService", "OnCreate");
        super.onCreate();
    }

    public IBinder onBind(Intent intent) {
        Log.v("IFAAService", "OnBind");
        return this.mIFAABinder;
    }

    public void onDestroy() {
        Log.v("IFAAService", "onDestroy");
        this.mService = null;
    }

    private void initService() throws RemoteException {
        if (this.mService == null) {
            this.mService = IMlipayService.getService();
        } else {
            Log.i("IFAAService", "[initService] Service not null, skip to init");
        }
    }

    private Byte[] wrapArray(byte[] bArr) {
        Byte[] bArr2 = new Byte[bArr.length];
        int length = bArr.length;
        int i = 0;
        int i2 = 0;
        while (i < length) {
            bArr2[i2] = Byte.valueOf(bArr[i]);
            i++;
            i2++;
        }
        return bArr2;
    }

    private String initExtString() {
        JSONObject jSONObject = new JSONObject();
        JSONObject jSONObject2 = new JSONObject();
        String str = SystemProperties.get("persist.vendor.sys.fp.fod.location.X_Y", "");
        String str2 = SystemProperties.get("persist.vendor.sys.fp.fod.size.width_height", "");
        try {
            if (!validateVal(str) || !validateVal(str2)) {
                Log.e("IFAAService", "[initExtString] invalidate, xy: " + str + ", wh: " + str2);
                return "";
            }
            String[] split = str.split(",");
            String[] split2 = str2.split(",");
            jSONObject2.put("startX", Integer.parseInt(split[0]));
            jSONObject2.put("startY", Integer.parseInt(split[1]));
            jSONObject2.put("width", Integer.parseInt(split2[0]));
            jSONObject2.put("height", Integer.parseInt(split2[1]));
            jSONObject2.put("navConflict", true);
            jSONObject.put("type", 0);
            jSONObject.put("fullView", jSONObject2);
            return jSONObject.toString();
        } catch (Exception e) {
            Log.e("IFAAService", "[initExtString] Exception, xy: " + str + ", wh: " + str2, e);
            return "";
        }
    }

    private boolean validateVal(String str) {
        return !"".equalsIgnoreCase(str) && str.contains(",");
    }

    private final class IFAAServiceStub extends IFAAManagerService.Stub {
        IFAAServiceStub(IFAAService ifaaService) {
            new WeakReference(ifaaService);
        }

        @Override
        public void setExtInfo(int i, String str, String str2) {
        }

        @Override
        public int getSupportBIOTypes() {
            int i = SystemProperties.getInt("persist.vendor.sys.pay.ifaa", 0);
            String str = SystemProperties.get("persist.vendor.sys.fp.vendor", "");
            int i2 = "none".equalsIgnoreCase(str) ? i & 2 : i & 3;
            if ((i2 & 1) == 1 && IFAAService.sIsFod) {
                i2 |= 16;
            }
            Log.i("IFAAService", "[getSupportBIOTypes] ifaaProp: " + i + ", sIsFod: " + IFAAService.sIsFod + ", fpVendor: " + str + ", res: " + i2);
            return i2;
        }

        @Override
        public int startBIOManager(int i) {
            int i2;
            if (1 == i) {
                Intent intent = new Intent();
                intent.setClassName("com.android.settings", "com.android.settings.NewFingerprintActivity");
                intent.setFlags(268435456);
                IFAAService.this.getApplicationContext().startActivity(intent);
                i2 = 0;
            } else {
                i2 = -1;
            }
            Log.i("IFAAService", "[startBIOManager] authType: " + i + ", res: " + i2);
            return i2;
        }

        @Override
        public String getDeviceModel() {
            String str;
            str = "Xiaomi-" + Build.DEVICE;
            Log.i("IFAAService", "[getDeviceModel] devcieModel: " + str);
            return str;
        }

        @Override
        public byte[] processCmd(byte[] bArr) {
            try {
                IFAAService.this.initService();
                if (IFAAService.this.mService != null) {
                    ArrayList<Byte> arrayList = new ArrayList<>(Arrays.asList(IFAAService.this.wrapArray(bArr)));
                    ArrayList<Byte> invoke_command = IFAAService.this.mService.invoke_command(arrayList, arrayList.size());
                    int size = invoke_command.size();
                    byte[] bArr2 = new byte[size];
                    for (int i = 0; i < size; i++) {
                        bArr2[i] = invoke_command.get(i).byteValue();
                    }
                    return bArr2;
                }
                Log.e("IFAAService", "[processCmd] IMlipayService not found");
                return null;
            } catch (RemoteException e) {
                Log.e("IFAAService", "[processCmd] transact fail: " + e);
                return null;
            }
        }

        @Override
        public int getVersion() {
            Log.i("IFAAService", "[getVersion] SDK: " + Build.VERSION.SDK_INT + ", ifaaVer: " + 4);
            return 4;
        }

        @Override
        public String getExtInfo(int i, String str) {
            Log.i("IFAAService", "[getExtInfo] authType: " + i + ", keyExtInfo: " + str);
            return IFAAService.this.initExtString();
        }

        @Override
        public int getEnabled(int i) {
            int i2 = 1 == i ? 1000 : 1003;
            Log.i("IFAAService", "[getEnabled] bioType: " + i + ", res: " + i2);
            return i2;
        }

        @Override
        public int[] getIDList(int i) {
            try {
                IFAAService.this.initService();
                if (IFAAService.this.mService != null) {
                    ArrayList<Integer> ifaa_get_idlist = IFAAService.this.mService.ifaa_get_idlist(i);
                    int size = ifaa_get_idlist.size();
                    int[] iArr = new int[size];
                    for (int i2 = 0; i2 < size; i2++) {
                        iArr[i2] = ifaa_get_idlist.get(i2).intValue();
                    }
                    return iArr;
                }
                Log.e("IFAAService", "[getIDList] IMlipayService not found");
                return null;
            } catch (RemoteException e) {
                Log.e("IFAAService", "[getIDList] transact fail " + e);
                return null;
            }
        }
    }
}
