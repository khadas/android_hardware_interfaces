/*
 * Copyright (C) 2020 The Android Open Source Project
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

#define LOG_TAG "AudioControlImpl"
// #define LOG_NDEBUG 0

#include "AudioControlImpl.h"
#include <android-base/logging.h>
#include <android-base/parsebool.h>
#include <android-base/parseint.h>
#include <android-base/strings.h>
#include <android/binder_ibinder.h>
#include <android/binder_manager.h>
#include <android/binder_status.h>
#include <android_audio_policy_configuration_V7_0-enums.h>
#include <private/android_filesystem_config.h>
#include <utils/Log.h>
#include <android-base/chrono_utils.h>
#include <android-base/logging.h>
#include <android/binder_manager.h>
#include <utils/SystemClock.h>
#include <numeric>
#include <stdio.h>
#include <hardware/hardware.h>


using ::android::base::EqualsIgnoreCase;
using ::android::base::ParseBool;
using ::android::base::ParseBoolResult;
using ::android::base::ParseInt;
using ::std::shared_ptr;
using ::std::string;

AudioControlImpl::AudioControlImpl(): dspDefaultLoaded(false) {
    init();
}

ndk::ScopedAStatus AudioControlImpl::setBalance(float value) {
    ALOGE("Balance value out of range -1 to 1 at value:%f ", value);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AudioControlImpl::setVolume(const char* address, int value) {
    ALOGE("address :%s value: %d ", address, value);
    if (dspDefaultLoaded) {
        dspDevice->setVolume(address, value);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AudioControlImpl::init() {
    loadDspDefault();
    return ndk::ScopedAStatus::ok();
}

bool AudioControlImpl::loadDspDefault() {
    std::lock_guard<std::mutex> lk(mLock);

    ALOGD("initDspDevice: open dsp device.");
    const hw_module_t* hw_module = nullptr;
    if (0 == hw_get_module(DSP_HARDWARE_MODULE_ID, &hw_module)) {
        if (0 == dsp_hw_device_open(hw_module, &dspDevice)) {
            dspDefaultLoaded = true;
        } else {
            ALOGE("dsp_hw_device_open failed");
        }
    } else {
        ALOGE("hw_get_module %s failed", DSP_HARDWARE_MODULE_ID);
    }
    return dspDefaultLoaded;
}
