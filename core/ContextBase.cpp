/* Copyright (c) 2011-2014, The Linux Foundation. All rights reserved.
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
#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_CtxBase"

#include <dlfcn.h>
#include <cutils/sched_policy.h>
#include <unistd.h>
#include <ContextBase.h>
#include <msg_q.h>
#include <loc_target.h>
#include <platform_lib_includes.h>
#include <loc_log.h>

namespace loc_core {

LBSProxyBase* ContextBase::getLBSProxy(const char* libName)
{
    LBSProxyBase* proxy = NULL;
    LOC_LOGD("%s:%d]: getLBSProxy libname: %s\n", __func__, __LINE__, libName);
    void* lib = dlopen(libName, RTLD_NOW);

    if ((void*)NULL != lib) {
        getLBSProxy_t* getter = (getLBSProxy_t*)dlsym(lib, "getLBSProxy");
        if (NULL != getter) {
            proxy = (*getter)();
        }
    }
    else
    {
        LOC_LOGW("%s:%d]: FAILED TO LOAD libname: %s\n", __func__, __LINE__, libName);
    }
    if (NULL == proxy) {
        proxy = new LBSProxyBase();
    }
    LOC_LOGD("%s:%d]: Exiting\n", __func__, __LINE__);
    return proxy;
}

LocApiBase* ContextBase::createLocApi(LOC_API_ADAPTER_EVENT_MASK_T exMask)
{
    LocApiBase* locApi = NULL;

    // first if can not be MPQ
    if (TARGET_MPQ != loc_get_target()) {
        if (NULL == (locApi = mLBSProxy->getLocApi(mMsgTask, exMask))) {
            // only RPC is the option now
            void* handle = dlopen("libloc_api-rpc-qc.so", RTLD_NOW);
            if (NULL != handle) {
                getLocApi_t* getter = (getLocApi_t*)dlsym(handle, "getLocApi");
                if (NULL != getter) {
                    locApi = (*getter)(mMsgTask, exMask);
                }
            }
        }
    }

    // locApi could still be NULL at this time
    // we would then create a dummy one
    if (NULL == locApi) {
        locApi = new LocApiBase(mMsgTask, exMask);
    }

    return locApi;
}

ContextBase::ContextBase(const MsgTask* msgTask,
                         LOC_API_ADAPTER_EVENT_MASK_T exMask,
                         const char* libName) :
    mLBSProxy(getLBSProxy(libName)),
    mMsgTask(msgTask),
    mLocApi(createLocApi(exMask)),
    mLocApiProxy(mLocApi->getLocApiProxy())
{
}

}
