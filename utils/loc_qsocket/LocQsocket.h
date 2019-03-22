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

#ifndef __LOC_QSOCKET__
#define __LOC_QSOCKET__

#include <string>
#include <memory>
#include <unistd.h>
#include <poll.h> // qxocket.h
#include <sys/ioctl.h> // qsocket_ipcr.h

#include <qsocket.h>
#include <qsocket_ipcr.h>
#include <LocIpc.h>
#include <log_util.h>

#include <LocThread.h>

namespace loc_util {

class LocQsocketSender;

class LocQsocket : LocIpc {
friend LocQsocketSender;
public:
    inline LocQsocket() :
            mService(0), mInstance(0),
            mFdMe(-1), mRunnable(nullptr) {}
    inline virtual ~LocQsocket() { stopListening(); }

    // Listen for new messages in current thread. The socket to listen on
    // is identified by the argument name, which contains the service and
    // instance id of the socket.
    // Calling this funciton will block current thread. The listening can
    // be stopped by calling stopListening().
    bool startListeningBlocking(const std::string& name);

    // Create a new LocThread and listen for new messages in it.
    // The socket to listen on is identified by the argument name,
    // which contains the service and instance id of the socket.
    // Calling this function will return immediately and won't block current thread.
    // The listening can be stopped by calling stopListening().
    bool startListeningNonBlocking(const std::string& name);

    // Stop listening to new messages.
    void stopListening();

    // Send out a message.
    // Call this function to send a message in argument data to socket in argument name.
    //
    // Argument name contains the name of the target unix socket. data contains the
    // message to be sent out. Convert your message to a string before calling this function.
    // The function will return true on success, and false on failure.
    static bool send(int service, int instance, const std::string& data);
    static bool send(int service, int instance, const uint8_t data[], uint32_t length);

protected:
    // Callback function for receiving incoming messages.
    // Override this function in your derived class to process incoming messages.
    // For each received message, this callback function will be called once.
    // This callback function will be called in the calling thread of startListeningBlocking
    // or in the new LocThread created by startListeningNonBlocking.
    //
    // Argument data contains the received message. You need to parse it.
    inline virtual void onReceive(const std::string& /*data*/) {}

    // LocQsocket client can overwrite this function to get notification
    // when the socket for LocQsocket is ready to receive messages.
    inline virtual void onListenerReady() {}

private:
    static bool sendData(int fd, const qsockaddr_ipcr& addr,
            const uint8_t data[], uint32_t length);

    static bool findServiceWithRetry(int fd, qsockaddr_ipcr& addr,
                                     int service, int instance);

    static bool findService(int fd, qsockaddr_ipcr& addr,
                            int service, int instance);

    int mService;
    int mInstance;
    int mFdMe;
    struct qsockaddr_ipcr mDestAddr;
    LocThread mThread;
    LocRunnable *mRunnable;
};

static void printaddr(const struct qsockaddr_ipcr* p)
{
    LOC_LOGd("family=%u", p->family); // AF_MAM_IPC = 27
    LOC_LOGd("addr type=%u", p->address.addrtype); // 1= name, 2= id
    LOC_LOGd("port addr node=0x%x", p->address.addr.port_addr.node_id);
    LOC_LOGd("port addr port=0x%x", p->address.addr.port_addr.port_id);
    LOC_LOGd("port name serv=%d", p->address.addr.port_name.service);
    LOC_LOGd("port name inst=%d", p->address.addr.port_name.instance);
}

class LocQsocketSender {
public:
    // Constructor of LocQsocketSender class
    //
    // Argument service/instance contain destination port.
    // This class hides generated fd and destination address object from user.
    inline LocQsocketSender(int service, int instance) {

        LOC_LOGd("service %d, instance %d", service, instance); // AF_MAM_IPC = 27
        mSocket = qsocket(AF_IPC_ROUTER, QSOCK_DGRAM, 0);
        if (mSocket < 0) {
            return;
        }

        mService = service;
        mInstance = instance;

        LocQsocket::findServiceWithRetry(mSocket, mDestAddr, mService, mInstance);
        printaddr(&mDestAddr);
    }

    inline ~LocQsocketSender() {
        if (mSocket >= 0) {
            qclose(mSocket);
            mSocket = -1;
        }
    }

    // Send out a message.
    // Call this function to send a message
    //
    // Argument data and length contains the message to be sent out.
    // Return true when succeeded
    inline bool send(const uint8_t data[], uint32_t length) {
        bool rtv = true;

        if (nullptr != data) {
            rtv = LocQsocket::sendData(mSocket, mDestAddr, data, length);
        }
        return rtv;
    }

private:
    int mSocket;
    int mService;
    int mInstance;
    qsockaddr_ipcr mDestAddr;
};

}

#endif //__LOC_QSOCKET__
