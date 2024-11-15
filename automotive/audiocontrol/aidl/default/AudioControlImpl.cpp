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

namespace aidl::android::hardware::automotive::audiocontrol {
using ::android::base::EqualsIgnoreCase;
using ::android::base::ParseBool;
using ::android::base::ParseBoolResult;
using ::android::base::ParseInt;
using ::std::shared_ptr;
using ::std::string;

constexpr uint32_t kMaxVersionLength = 512;
bool AudioControlImpl::dspDefaultLoaded = false;
audio_control_hw_device* AudioControlImpl::dspDevice = nullptr;
std::shared_ptr<IUpdateDspCallback> AudioControlImpl::mUpdateDspCallback = nullptr;

AudioControlImpl::AudioControlImpl() {
    horizontalRatio = 0;
    verticalRation = 0;
}

ndk::ScopedAStatus AudioControlImpl::setBalance(float value) {
    ALOGE("Balance value out of range -1 to 1 at value:%f ", value);
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AudioControlImpl::setVolume(const char* address, int32_t value) {
    ALOGE("address :%s value: %d ", address, value);
    if (dspDefaultLoaded) {
        dspDevice->set_volume(address, value);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AudioControlImpl::setEffectMode(int32_t mode)  {
    ALOGE("set effect mode : %d ", mode);
    if (dspDefaultLoaded) {
        dspDevice->set_effect_mode(mode);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AudioControlImpl::setEffectConfig(int32_t bass, int32_t mid, int32_t treble) {
    ALOGE("set effect config bass: %d mid: %d  treble: %d", bass, mid, treble);
    if (dspDefaultLoaded) {
        dspDevice->set_effect_config(bass, mid, treble);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AudioControlImpl::setBalanceTowardRight(float value) {
    ALOGE("set BalanceTowardRight : %f ", value);
    horizontalRatio = value;
    if (dspDefaultLoaded) {
        dspDevice->set_sound_balance(horizontalRatio, verticalRation);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AudioControlImpl::setFadeTowardFront(float value) {
    ALOGE("set FadeTowardFront : %f ", value);
    verticalRation = value;
    if (dspDefaultLoaded) {
        dspDevice->set_sound_balance(horizontalRatio, verticalRation);
    }
    return ndk::ScopedAStatus::ok();
}

ndk::ScopedAStatus AudioControlImpl::updateDsp(int32_t mode) {
    ALOGE("updateDsp : %d ", mode);
    if (dspDefaultLoaded) {
        dspDevice->update_dsp_firmware(AudioControlImpl::updataCallback);
    }
    return ndk::ScopedAStatus::ok();
}

std::string AudioControlImpl::getDspVersion() {
    char dspVersion[kMaxVersionLength] = "Unknown";
    if (dspDefaultLoaded) {
        dspDevice->get_dsp_version(dspVersion);
        ALOGD("get dsp version : %s ", dspVersion);
    }

    return std::string(dspVersion);
}

void AudioControlImpl::updataCallback(int32_t state, float progess) {
    if (dspDefaultLoaded) {
        ALOGD("state: %d, get update progress : %f ", state, progess);
        if (mUpdateDspCallback) {
            mUpdateDspCallback->onDspUpadateProgessChanged(state, progess);
        }
    }
}

void AudioControlImpl::registerUpdateCallback(const std::shared_ptr<IUpdateDspCallback>& in_callback) {
    mUpdateDspCallback = in_callback;
    if (dspDefaultLoaded) {
        ALOGD("registerUpdateCallback in");
    }
}

void loadDspDefault() {
    const hw_module_t* hwModule = nullptr;

    ALOGD("%s, Loader %s AudioControl HAL\n", __FUNCTION__, AUDIO_CONTROL_HARDWARE_MODULE_ID);

    int ret = hw_get_module(AUDIO_CONTROL_HARDWARE_MODULE_ID, (const struct hw_module_t**)&hwModule);
    if (ret == 0) {
        ret = hwModule->methods->open(hwModule, AUDIO_CONTROL_HARDWARE_MODULE_ID,
            reinterpret_cast<hw_device_t**>(&AudioControlImpl::dspDevice));
        if (ret == 0) {
            ALOGE("open dsp device success. %p ", AudioControlImpl::dspDevice);
            AudioControlImpl::dspDefaultLoaded = true;
        } else {
            ALOGE("dsp_hw_device_open failed, ret: %d", ret);
        }

    } else {
        ALOGE("hw_get_module %s failed, ret: %d", AUDIO_CONTROL_HARDWARE_MODULE_ID, ret);
    }
}
}  // namespace aidl::android::hardware::automotive::audiocontrol
