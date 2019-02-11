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
#ifndef LOCHALDAEMON_IPCSENDER_H
#define LOCHALDAEMON_IPCSENDER_H

#include <gps_extended_c.h>
#include <LocIpc.h>
#include <LocQsocket.h>
#include <LocationApiMsg.h>

using loc_util::LocIpcSender;
using loc_util::LocQsocketSender;

class LocHalDaemonIPCSender
{
public:
    inline LocHalDaemonIPCSender(const char* destSocket) :
            mIpcSender(nullptr),
            mQsockSender(nullptr) {

        if (strncmp(destSocket, SOCKET_DIR_TO_CLIENT,
                    sizeof(SOCKET_DIR_TO_CLIENT)-1) == 0 ) {
            mIpcSender = new LocIpcSender(destSocket);
        }
        else {
            uint32_t serviceId = atoi(destSocket);
            const char* instance_ptr = strchr(destSocket, '.');
            if (nullptr != instance_ptr) {
                uint32_t instanceId = atoi(++instance_ptr);
                mQsockSender = new LocQsocketSender(serviceId, instanceId);
            }
        }
    }

    bool send(const uint8_t data[], uint32_t length);

private:
    LocIpcSender* mIpcSender;
    LocQsocketSender* mQsockSender;
};

#endif //LOCHALDAEMON_IPCSENDER_H

