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

#include <sys/types.h>
#include <sys/stat.h>
#include <LocQsocket.h>
#include <log_util.h>

namespace loc_util {

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LocSvc_LocQSocket"

#define LOC_MSG_BUF_LEN 8192
#define LOC_MSG_HEAD "$MSGLEN$"
#define LOC_MSG_ABORT "LocQsocketMsg::ABORT"
#define RETRY_FINDSERVICE_MAX_COUNT 10
#define RETRY_FINDSERVICE_SLEEP_MS  5

class LocQsocketRunnable : public LocRunnable {
friend LocQsocket;
public:
    LocQsocketRunnable(LocQsocket& locQsocket, const std::string& qsocketName)
            : mLocQsocket(locQsocket), mQsocketName(qsocketName) {}
    bool run() override {
        if (!mLocQsocket.startListeningBlocking(mQsocketName)) {
            LOC_LOGe("listen to socket failed");
        }

        return false;
    }
private:
     LocQsocket& mLocQsocket;
     const std::string mQsocketName;
};

bool LocQsocket::startListeningNonBlocking(const std::string& name) {
    mRunnable = new LocQsocketRunnable(*this, name);
    std::string threadName("LocQsocket-");
    threadName.append(name);
    return mThread.start(threadName.c_str(), mRunnable);
}

bool LocQsocket::startListeningBlocking(const std::string& name) {
    bool stopRequested = false;

    // socket
    int fd  = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
    if (fd < 0) {
        LOC_LOGe("create socket error. reason:%s", strerror(errno));
        return false;
    }

    // name is of format "serviceid.instanceid", and thus
    // should not exceed 65 bytes
    mService = atoi(name.c_str());
    const char* instance_ptr = strchr(name.c_str(), '.');
    if (nullptr == instance_ptr) {
        return false;
    }

    mInstance = atoi(++instance_ptr);

    // bind
    memset(&mDestAddr, 0, sizeof(mDestAddr));
    mDestAddr.family = AF_IPC_ROUTER;
    mDestAddr.address.addrtype = IPCR_ADDR_NAME;
    mDestAddr.address.addr.port_name.service = mService;
    mDestAddr.address.addr.port_name.instance = mInstance;
    if (qbind(fd, (struct qsockaddr*)&mDestAddr, sizeof(mDestAddr)) < 0) {
        LOC_LOGe("bind socket error. reason:%s", strerror(errno));
        qclose(fd);
        return false;
    }

    mFdMe = fd;
    LOC_LOGd("startListeningBlocking mFd=%d", mFdMe);
    printaddr(&mDestAddr);

    // inform that the socket is ready to receive message
    onListenerReady();

    ssize_t nBytes = 0;
    std::string msg = "";
    std::string abort = LOC_MSG_ABORT;
    while (1) {
        struct qsockaddr_ipcr tmpAddr = {0};
        qsocklen_t tmpAddrlen = sizeof(tmpAddr);

        msg.resize(LOC_MSG_BUF_LEN);

        nBytes = qrecvfrom(fd, (void*)(msg.data()), msg.size(), 0, (struct qsockaddr *)&tmpAddr, &tmpAddrlen);
        LOC_LOGd("recvfrom done=%d", nBytes);
        if (nBytes < 0) {
            break;
        } else if (nBytes == 0) {
            continue;
        }

        if (strncmp(msg.data(), abort.c_str(), abort.length()) == 0) {
            stopRequested = true;
            LOC_LOGi("recvd abort msg.data %s", msg.data());
            break;
        }

        if (strncmp(msg.data(), LOC_MSG_HEAD, sizeof(LOC_MSG_HEAD) - 1)) {
            // short message
            msg.resize(nBytes);
            onReceive(msg);
        } else {
            // long message
            size_t msgLen = 0;
            sscanf(msg.data(), LOC_MSG_HEAD"%zu", &msgLen);
            msg.resize(msgLen);
            size_t msgLenReceived = 0;
            while ((msgLenReceived < msgLen) && (nBytes > 0)) {
                nBytes = qrecvfrom(fd, (void*)&(msg[msgLenReceived]),
                        msg.size() - msgLenReceived, 0, NULL, NULL);
                msgLenReceived += nBytes;
            }
            if (nBytes > 0) {
                onReceive(msg);
            } else {
                break;
            }
        }
    }

    if (::close(fd)) {
        LOC_LOGe("cannot close socket:%s", strerror(errno));
    }

    return stopRequested;
}

void LocQsocket::stopListening() {

    if (mFdMe >= 0) {
        std::string abort = LOC_MSG_ABORT;
        send(mService, mInstance, abort);
        mFdMe = -1;
    }

    if (mRunnable) {
        mRunnable = nullptr;
    }
}

bool LocQsocket::findService(int fd, qsockaddr_ipcr& addr,
                             int service, int instance) {
    bool serviceFound = false;
    if (0 <= fd) {
        memset(&addr, 0, sizeof(addr));
        ipcr_name_t ipcr_name = { service, instance };

        unsigned int num_entries = 1;
        int rc = ipcr_find_name(fd, &ipcr_name, &addr, NULL, &num_entries, 0);
        if ((rc > 0) && (1 == num_entries)) {
            serviceFound = true;
            LOC_LOGd("for service %d, instance %d, number of services successfully found %d",
                      service, instance, num_entries);
            printaddr(&addr);
        } else {
            LOC_LOGe("service found error code %d, service num %d", rc, num_entries);
        }
    }
    return serviceFound;
}

bool LocQsocket::findServiceWithRetry(int fd, qsockaddr_ipcr& addr,
                                      int service, int instance) {
    int retryCount = 0;

    bool result = false;
    do {
        result = LocQsocket::findService(fd, addr, service, instance);
        if (true == result) {
            break;
        }
        usleep(RETRY_FINDSERVICE_SLEEP_MS * 1000);
    } while (retryCount++ <= RETRY_FINDSERVICE_MAX_COUNT);

    LOC_LOGd("find service returned %d, retryCount %d", result, retryCount);
    return result;
}

// static
bool LocQsocket::send(int service, int instance, const std::string& data) {
    return send(service, instance, (const uint8_t*)data.c_str(), data.length());
}

// static
bool LocQsocket::send(int service, int instance, const uint8_t data[], uint32_t length) {

    bool result = true;
    int fd = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
    if (fd < 0) {
        LOC_LOGe("create socket error. reason:%s", strerror(errno));
        return false;
    }

    qsockaddr_ipcr addr;
    result = findServiceWithRetry(fd, addr, service, instance);
    if (result == true) {
        result = sendData(fd, addr, data, length);
    }

    (void)qclose(fd);
    return result;
}

// static
bool LocQsocket::sendData(int fd, const qsockaddr_ipcr& addr, const uint8_t data[], uint32_t length) {

    bool result = true;

    if (length <= LOC_MSG_BUF_LEN) {
        if (qsendto(fd, data, length, 0,
                (struct qsockaddr*)&addr, sizeof(addr)) < 0) {
            LOC_LOGe("cannot send to socket. reason:%s", strerror(errno));
            result = false;
        }
    } else {
        std::string head = LOC_MSG_HEAD;
        head.append(std::to_string(length));
        if (qsendto(fd, head.c_str(), head.length(), 0,
                (struct qsockaddr*)&addr, sizeof(addr)) < 0) {
            LOC_LOGe("cannot send to socket. reason:%s", strerror(errno));
            result = false;
        } else {
            size_t sentBytes = 0;
            while(sentBytes < length) {
                size_t partLen = length - sentBytes;
                if (partLen > LOC_MSG_BUF_LEN) {
                    partLen = LOC_MSG_BUF_LEN;
                }
                ssize_t rv = qsendto(fd, (data + sentBytes), partLen, 0,
                        (struct qsockaddr*)&addr, sizeof(addr));
                if (rv < 0) {
                    LOC_LOGe("cannot send to socket. reason:%s", strerror(errno));
                    result = false;
                    break;
                }
                sentBytes += rv;
            }
        }
    }
    LOC_LOGd("send finished len=%u result=%d", length, result);
    return result;
}

}
