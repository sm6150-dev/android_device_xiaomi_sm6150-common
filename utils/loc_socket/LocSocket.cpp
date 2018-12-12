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
#include <LocSocket.h>
#include <log_util.h>
#include <libqrtr.h>

namespace loc_util {

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LocSvc_LocSocket"

#define LOC_MSG_BUF_LEN 8192
#define LOC_MSG_HEAD "$MSGLEN$"
#define LOC_MSG_ABORT "LocSocketMsg::ABORT"

static inline __le32 cpu_to_le32(uint32_t x) { return htole32(x); }
static inline uint32_t le32_to_cpu(__le32 x) { return le32toh(x); }

class LocSocketRunnable : public LocRunnable {
friend LocSocket;
public:
    LocSocketRunnable(LocSocket& locSocket, const std::string& socketName)
            : mLocSocket(locSocket), mSocketName(socketName) {}
    bool run() override {
        if (!mLocSocket.startListeningBlocking(mSocketName)) {
            LOC_LOGe("listen to socket failed");
        }

        return false;
    }
private:
     LocSocket& mLocSocket;
     const std::string mSocketName;
};

bool LocSocket::startListeningNonBlocking(const std::string& name) {
    mRunnable = new LocSocketRunnable(*this, name);
    std::string threadName("LocSocket-");
    threadName.append(name);
    return mThread.start(threadName.c_str(), mRunnable);
}

bool LocSocket::startListeningBlocking(const std::string& name) {
    bool stopRequested = false;

    // name is of format "serviceid.instanceid", and thus
    // should not exceed 65 bytes
    mService = atoi(name.c_str());
    const char* instance_ptr = strchr(name.c_str(), '.');
    if (nullptr != instance_ptr) {
        instance_ptr++;
    }

    mInstance = atoi(instance_ptr);

    // socket
    int fd = socket(AF_QIPCRTR, SOCK_DGRAM, 0);
    if (fd < 0) {
        LOC_LOGe("create socket error. reason:%s", strerror(errno));
        return false;
    }

    // get socket name
    sockaddr_qrtr addr = {0};
    socklen_t sl = sizeof(addr);
    int rc = getsockname(fd, (void*)&addr, &sl);
    if (rc || addr.sq_family != AF_QIPCRTR || sl != sizeof(addr)) {
        LOC_LOGe("error: getsockname rc=%d errno=%d (%s)\n", rc, errno, strerror(errno));
        return false;
    }

    LOC_LOGv("family=%u, node=%d, port=%d", addr.sq_family, addr.sq_node, addr.sq_port);
    // register this server by sending control packet
    struct qrtr_ctrl_pkt pkt = {0};
    pkt.cmd = cpu_to_le32(QRTR_TYPE_NEW_SERVER);
    pkt.server.service = cpu_to_le32(mService);
    pkt.server.instance = cpu_to_le32(mInstance);
    pkt.server.node = cpu_to_le32(addr.sq_node);
    pkt.server.port = cpu_to_le32(addr.sq_port);
    addr.sq_port = QRTR_PORT_CTRL;
    rc = sendto(fd, &pkt, sizeof(pkt), 0, (void*)&addr, sizeof(addr));
    if (rc < 0) {
        LOC_LOGe("send control packet failed");
        return false;
    }

    mFdMe = fd;
    LOC_LOGv("family=%u, node=%d, port=%d", mDestAddr.sq_family, mDestAddr.sq_node, mDestAddr.sq_port);

    // inform that the socket is ready to receive message
    onListenerReady();

    ssize_t nBytes = 0;
    std::string msg = "";
    std::string abort = LOC_MSG_ABORT;
    while (1) {
        struct sockaddr_qrtr tmpAddr = {0};
        socklen_t tmpAddrlen = sizeof(tmpAddr);

        msg.resize(LOC_MSG_BUF_LEN);

        nBytes = recvfrom(fd, (void*)(msg.data()), msg.size(), 0, (void*)&tmpAddr, &tmpAddrlen);
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

        struct sockaddr_qrtr clientAddr = {0};
        clientAddr = tmpAddr;

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
                nBytes = recvfrom(fd, (void*)&(msg[msgLenReceived]),
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

void LocSocket::stopListening() {

    if (mFdMe >= 0) {
        std::string abort = LOC_MSG_ABORT;
        send(mService, mInstance, abort);
        mFdMe = -1;
    }

    if (mRunnable) {
        mRunnable = nullptr;
    }
}

bool LocSocket::findService(int fd, sockaddr_qrtr& addr, int service, int instance) {

    memset(&addr, 0, sizeof(addr));
    socklen_t sl = sizeof(addr);

    // get socket name
    int rc = getsockname(fd, (void*)&addr, &sl);
    if (rc || addr.sq_family != AF_QIPCRTR || sl != sizeof(addr)) {
        LOC_LOGe("error: getsockname rc=%d errno=%d (%s)\n", rc, errno, strerror(errno));
        return false;
    }
    //addr.sq_node = QRTR_NODE_BCAST;
    addr.sq_port = QRTR_PORT_CTRL;

    // send control packet
    struct qrtr_ctrl_pkt pkt = {0};
    pkt.cmd = cpu_to_le32(QRTR_TYPE_NEW_LOOKUP);
    pkt.server.service = cpu_to_le32(service);
    pkt.server.instance = cpu_to_le32(instance);
    rc = sendto(fd, &pkt, sizeof(pkt), 0, (void*)&addr, sizeof(addr));
    if (rc < 0) {
        LOC_LOGe("sendto failed!\n");
        return false;
    }

    // get server addr from ipc router
    int len;
    while ((len = recv(fd, &pkt, sizeof(pkt), 0)) > 0) {
        unsigned int type = le32_to_cpu(pkt.cmd);

        if (len < sizeof(pkt) || type != QRTR_TYPE_NEW_SERVER) {
            LOC_LOGe("invalid/short packet\n");
            continue;
        }

        LOC_LOGd("service=%d\n", le32_to_cpu(pkt.server.service));
        LOC_LOGd("version=%d\n", le32_to_cpu(pkt.server.instance) & 0xff);
        LOC_LOGd("instance=%d\n", le32_to_cpu(pkt.server.instance) >> 8);
        LOC_LOGd("node=%d\n", le32_to_cpu(pkt.server.node));
        LOC_LOGd("port=%d\n", le32_to_cpu(pkt.server.port));

        if (0 != pkt.server.service) {
            break;
        }
    }
    if (len < 0) {
        LOC_LOGe("recv() returned %d bytes\n", len);
        return false;
    }

    addr.sq_node = le32_to_cpu(pkt.server.node);
    addr.sq_port = le32_to_cpu(pkt.server.port);
    return true;
}

// static
bool LocSocket::send(int service, int instance, const std::string& data) {
    return send(service, instance, (const uint8_t*)data.c_str(), data.length());
}

// static
bool LocSocket::send(int service, int instance, const uint8_t data[], uint32_t length) {

    bool result = true;
    int fd = socket(AF_QIPCRTR, SOCK_DGRAM, 0);
    if (fd < 0) {
        LOC_LOGe("create socket error. reason:%s", strerror(errno));
        return false;
    }

    sockaddr_qrtr addr;
    memset(&addr, 0, sizeof(addr));
    int rc = findService(fd, addr, service, instance);
    result = sendData(fd, addr, data, length);

    (void)close(fd);
    return result;
}

// static
bool LocSocket::sendData(int fd, const sockaddr_qrtr& addr, const uint8_t data[], uint32_t length) {

    bool result = true;

    if (length <= LOC_MSG_BUF_LEN) {
        if (sendto(fd, data, length, 0, (void*)&addr, sizeof(addr)) < 0) {
            LOC_LOGe("cannot send to socket. reason:%s", strerror(errno));
            result = false;
        }
    } else {
        std::string head = LOC_MSG_HEAD;
        head.append(std::to_string(length));
        if (sendto(fd, head.c_str(), head.length(), 0, (void*)&addr, sizeof(addr)) < 0) {
            LOC_LOGe("cannot send to socket. reason:%s", strerror(errno));
            result = false;
        } else {
            size_t sentBytes = 0;
            while(sentBytes < length) {
                size_t partLen = length - sentBytes;
                if (partLen > LOC_MSG_BUF_LEN) {
                    partLen = LOC_MSG_BUF_LEN;
                }
                ssize_t rv = sendto(fd, (data + sentBytes), partLen, 0, (void*)&addr, sizeof(addr));
                if (rv < 0) {
                    LOC_LOGe("cannot send to socket. reason:%s", strerror(errno));
                    result = false;
                    break;
                }
                sentBytes += rv;
            }
        }
    }
    return result;
}
}
