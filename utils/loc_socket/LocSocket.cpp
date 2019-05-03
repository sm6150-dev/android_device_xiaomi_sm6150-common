/* Copyright (c) 2019 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of The Linux Foundation, nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <unistd.h>
#include <memory>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#ifdef USE_QSOCKET
#include <sys/ioctl.h>
#include <poll.h>
#include <qsocket.h>
#include <qsocket_ipcr.h>
#else
#include <endian.h>
#include <linux/qrtr.h>
#endif

#include <log_util.h>
#include <LocIpc.h>

namespace loc_util {

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LocSvc_Qrtr"

class ServiceInfo {
    const int32_t mServiceId;
    const int32_t mInstanceId;
    const string mName;
public:
    inline ServiceInfo(int32_t service, int32_t instance) :
            mServiceId(service), mInstanceId(instance),
            mName(to_string(service) + ":" + to_string(instance)) {}
    inline const char* getName() const { return mName.data(); }
    inline int32_t getServiceId() const { return mServiceId; }
    inline int32_t getInstanceId() const { return mInstanceId; }
};

#ifdef USE_QSOCKET

class LocIpcQsockSender : public LocIpcSender {
protected:
    shared_ptr<Sock> mSock;
    const ServiceInfo mServiceInfo;
    qsockaddr_ipcr mAddr;
    mutable bool mLookupPending;
    inline virtual bool isOperable() const override {
        return mSock != nullptr && mSock->mSid != -1;
    }
    inline void serviceLookup() const {
        if (mLookupPending && mSock->isValid()) {
            ipcr_name_t ipcrName = {
                .service = (uint32_t)mServiceInfo.getServiceId(),
                .instance = (uint32_t)mServiceInfo.getInstanceId()
            };
            uint32_t numEntries = 1;
            int rc = ipcr_find_name(mSock->mSid, &ipcrName,
                    (struct qsockaddr_ipcr*)&mAddr, NULL, &numEntries, 0);
            if (rc < 0 || 1 != numEntries) {
                LOC_LOGe("Failed ipcr_find_name, service: %d, reason: %s",
                         mServiceInfo.getServiceId(), strerror(errno));
                mSock->close();
            }
            mLookupPending = false;
        }
    }
    inline virtual ssize_t send(const uint8_t data[], uint32_t length,
                                int32_t /* msgId */) const override {
        serviceLookup();
        return mSock->send(data, length, 0, (struct sockaddr*)&mAddr,
                           sizeof(mAddr));
    }
public:
    inline LocIpcQsockSender(int service, int instance) :
            LocIpcSender(),
            mSock(make_shared<Sock>(::socket(AF_IPC_ROUTER, SOCK_DGRAM, 0))),
            mServiceInfo(service, instance), mAddr({}), mLookupPending(true) {
    }
};

class LocIpcQsockRecver : public LocIpcQsockSender, public LocIpcRecver {
protected:
    inline virtual ssize_t recv() const override {
        socklen_t size = sizeof(mAddr);
        return mSock->recv(mDataCb, 0, (struct sockaddr*)&mAddr, &size);
    }
public:
    inline LocIpcQsockRecver(const shared_ptr<ILocIpcListener>& listener,
                             int service, int instance) :
            LocIpcQsockSender(service, instance),
            LocIpcRecver(listener, *this) {
        qsockaddr_ipcr addr = {
            AF_IPC_ROUTER,
            {
                IPCR_ADDR_NAME,
                {
                    (uint32_t)service,
                    (uint32_t)instance
                }
            },
            0
        };
        if (mSock->isValid() &&
            ::bind(mSock->mSid, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            LOC_LOGe("bind socket error. sock fd: %d,reason: %s",
                     mSock->mSid, strerror(errno));
            mSock->close();
        }
    }
    inline virtual const char* getName() const override {
        return mServiceInfo.getName();
    }
    inline virtual void abort() const override {
        if (isSendable()) {
            serviceLookup();
            mSock->sendAbort(0, (struct sockaddr*)&mAddr, sizeof(mAddr));
        }
    }
};

shared_ptr<LocIpcSender> createLocIpcQsockSender(int service, int instance) {
    return make_shared<LocIpcQsockSender>(service, instance);
}
unique_ptr<LocIpcRecver> createLocIpcQsockRecver(const shared_ptr<ILocIpcListener>& listener,
                                                 int service, int instance) {
    return make_unique<LocIpcQsockRecver>(listener, service, instance);
}

#else

static inline __le32 cpu_to_le32(uint32_t x) { return htole32(x); }
static inline uint32_t le32_to_cpu(__le32 x) { return le32toh(x); }

class LocIpcQrtrSender : public LocIpcSender {
protected:
    const ServiceInfo mServiceInfo;
    shared_ptr<Sock> mSock;
    mutable sockaddr_qrtr mAddr;
    mutable struct qrtr_ctrl_pkt mCtrlPkt;
    mutable bool mLookupPending;
    bool ctrlCmdAndResponse(enum qrtr_pkt_type cmd) const {
        if (mSock->isValid()) {
            int rc = 0;
            sockaddr_qrtr addr = mAddr;
            addr.sq_port = QRTR_PORT_CTRL;
            mCtrlPkt.cmd = cpu_to_le32(cmd);
            if ((rc = ::sendto(mSock->mSid, &mCtrlPkt, sizeof(mCtrlPkt), 0,
                               (const struct sockaddr *)&addr, sizeof(addr))) < 0) {
                LOC_LOGe("failed: sendto rc=%d reason=(%s)\n", rc, strerror(errno));
                mSock->close();
            } else if (QRTR_TYPE_NEW_LOOKUP == cmd) {
                int len;
                struct qrtr_ctrl_pkt pkt;
                while ((len = ::recv(mSock->mSid, &pkt, sizeof(pkt), 0)) > 0 &&
                       (len < (decltype(len))sizeof(pkt) ||
                        (cpu_to_le32(QRTR_TYPE_NEW_SERVER) != pkt.cmd) ||
                        (0 == pkt.server.service))) {
                }
                if (len < 0) {
                    LOC_LOGe("failed: recv len=%d reason=(%s)\n", len, strerror(errno));
                    mSock->close();
                } else {
                    mAddr.sq_node = le32_to_cpu(pkt.server.node);
                    mAddr.sq_port = le32_to_cpu(pkt.server.port);
                }
            }
        }
        return mSock->isValid();
    }
    inline virtual bool isOperable() const override {
        return mSock != nullptr && mSock->isValid();
    }
    inline virtual ssize_t send(const uint8_t data[], uint32_t length,
                                int32_t /* msgId */) const override {
        if (mLookupPending) {
            mLookupPending = false;
            ctrlCmdAndResponse(QRTR_TYPE_NEW_LOOKUP);
        }
        return mSock->send(data, length, 0, (struct sockaddr*)&mAddr, sizeof(mAddr));
    }
public:
    inline LocIpcQrtrSender(int service, int instance) : LocIpcSender(),
            mServiceInfo(service, instance),
            mSock(make_shared<Sock>(::socket(AF_QIPCRTR, SOCK_DGRAM, 0))),
            mAddr({AF_QIPCRTR, 0, 0}),
            mCtrlPkt({}),
            mLookupPending(true) {
        socklen_t sl = sizeof(mAddr);
        int rc = 0;
        if ((rc = getsockname(mSock->mSid, (struct sockaddr*)&mAddr, &sl)) ||
            mAddr.sq_family != AF_QIPCRTR || sl != sizeof(mAddr)) {
            LOC_LOGe("failed: getsockname rc=%d reason=(%s), mAddr.sq_family=%d",
                     rc, strerror(errno), mAddr.sq_family);
            mSock->close();
        } else {
            mCtrlPkt.server.service = cpu_to_le32(service);
            mCtrlPkt.server.instance = cpu_to_le32(instance);
            mCtrlPkt.server.node = cpu_to_le32(mAddr.sq_node);
            mCtrlPkt.server.port = cpu_to_le32(mAddr.sq_port);
        }
    }
};

class LocIpcQrtrRecver : public LocIpcQrtrSender, public LocIpcRecver {
protected:
    inline virtual ssize_t recv() const override {
        socklen_t size = sizeof(mAddr);
        return mSock->recv(mDataCb, 0, (struct sockaddr*)&mAddr, &size);
    }
public:
    inline LocIpcQrtrRecver(const shared_ptr<ILocIpcListener>& listener,
                            int service, int instance) :
            LocIpcQrtrSender(service, instance), LocIpcRecver(listener, *this) {
        ctrlCmdAndResponse(QRTR_TYPE_NEW_SERVER);
    }
    inline ~LocIpcQrtrRecver() { ctrlCmdAndResponse(QRTR_TYPE_DEL_SERVER); }
    inline virtual const char* getName() const override {
        return mServiceInfo.getName();
    }
    inline virtual void abort() const override {
        if (isSendable()) {
            mSock->sendAbort(0, (struct sockaddr*)&mAddr, sizeof(mAddr));
        }
    }
};

shared_ptr<LocIpcSender> createLocIpcQrtrSender(int service, int instance) {
    return make_shared<LocIpcQrtrSender>(service, instance);
}
unique_ptr<LocIpcRecver> createLocIpcQrtrRecver(const shared_ptr<ILocIpcListener>& listener,
                                                int service, int instance) {
    return make_unique<LocIpcQrtrRecver>(listener, service, instance);
}

#endif

}
