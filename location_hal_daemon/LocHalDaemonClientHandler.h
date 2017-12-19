/* Copyright (c) 2018, The Linux Foundation. All rights reserved.
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
 */

#ifndef LOCHALCLIENTHANDLER_H
#define LOCHALCLIENTHANDLER_H

#include <queue>
#include <log_util.h>

#include <LocationAPI.h>
#include <LocHalDaemonIPCSender.h>

/******************************************************************************
LocHalDaemonClientHandler
******************************************************************************/
class LocHalDaemonClientHandler
{
public:
    inline LocHalDaemonClientHandler(
            const std::string& clientname, LocationCallbacks& callbacks) :
            mCapabilityMask(0),
            mSessionId(0),
            mPendingMessages(),
            mSubscriptionMask(0),
            mLocationApi(LocationAPI::createInstance(callbacks)),
            mIpcSender(nullptr) {
        if (clientname != "default") {
            if (!mLocIpcSender) {
                mIpcSender = new LocHalDaemonIPCSender(clientname.c_str());
                //mLocIpcSender = mIpcSender;
            } else {
                //mIpcSender = mLocIpcSender->replicate(clientname.c_str());
            }
        }
    }

    inline ~LocHalDaemonClientHandler() {
        if (mIpcSender) {
            delete mIpcSender;
        }
        if (mLocationApi) {
            mLocationApi->destroy();
        }
    }

    // send ipc message to this client for general use
    template <typename MESSAGE>
    bool sendMessage(const MESSAGE& msg) {
        return sendMessage(reinterpret_cast<const uint8_t*>(&msg), sizeof(msg));
    }

    // send ipc message to this client for serialized payload
    bool sendMessage(const uint8_t* pmsg, size_t msglen) {
        bool ret = false;
        if (mIpcSender) {
            ret = mIpcSender->send(pmsg, msglen);
            if (!ret) {
                LOC_LOGe("Failed to send message. Disconnected client.");
                delete mIpcSender;
                mIpcSender = nullptr;
            }
        }
        return ret;
    }

    LocationCapabilitiesMask mCapabilityMask;
    uint32_t mSessionId;
    std::queue<ELocMsgID> mPendingMessages;
    uint32_t mSubscriptionMask;
    LocationAPI* mLocationApi;

private:
    static LocHalDaemonIPCSender* mLocIpcSender;
    LocHalDaemonIPCSender* mIpcSender;
};

#endif //LOCHALCLIENTHANDLER_H

