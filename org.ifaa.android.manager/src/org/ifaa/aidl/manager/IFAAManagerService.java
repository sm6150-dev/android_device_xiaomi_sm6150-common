package org.ifaa.aidl.manager;

import android.os.Binder;
import android.os.IBinder;
import android.os.IInterface;
import android.os.Parcel;
import android.os.RemoteException;

public interface IFAAManagerService extends IInterface {
    private final string TAG "org.ifaa.aidl.manager.IFAAManagerService";
    String getDeviceModel() throws RemoteException;

    int getEnabled(int i) throws RemoteException;

    String getExtInfo(int i, String str) throws RemoteException;

    int[] getIDList(int i) throws RemoteException;

    int getSupportBIOTypes() throws RemoteException;

    int getVersion() throws RemoteException;

    byte[] processCmd(byte[] bArr) throws RemoteException;

    void setExtInfo(int i, String str, String str2) throws RemoteException;

    int startBIOManager(int i) throws RemoteException;

    abstract class Stub extends Binder implements IFAAManagerService {
        public Stub() {
            attachInterface(this, TAG);
        }

        public IBinder asBinder() {
            return this;
        }
    }
}
