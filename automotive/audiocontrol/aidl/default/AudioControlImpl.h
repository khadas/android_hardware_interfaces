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
#ifndef ANDROID_HARDWARE_AUTOMOTIVE_AUDIOCONTROL_AUDIOCONTROLIMPL_H
#define ANDROID_HARDWARE_AUTOMOTIVE_AUDIOCONTROL_AUDIOCONTROLIMPL_H
#include <thread>
#include <aidl/android/hardware/automotive/audiocontrol/AudioFocusChange.h>
#include <aidl/android/hardware/automotive/audiocontrol/AudioGainConfigInfo.h>
#include <aidl/android/hardware/automotive/audiocontrol/BnAudioControl.h>
#include <aidl/android/hardware/automotive/audiocontrol/DuckingInfo.h>
#include <aidl/android/hardware/automotive/audiocontrol/IAudioGainCallback.h>
#include <aidl/android/hardware/automotive/audiocontrol/IModuleChangeCallback.h>
#include <aidl/android/hardware/automotive/audiocontrol/IUpdateDspCallback.h>
#include <aidl/android/hardware/automotive/audiocontrol/MutingInfo.h>
#include <aidl/android/hardware/automotive/audiocontrol/Reasons.h>

#include <aidl/android/hardware/audio/common/PlaybackTrackMetadata.h>

#include <aidl/android/media/audio/common/AudioChannelLayout.h>
#include <aidl/android/media/audio/common/AudioDeviceType.h>
#include <aidl/android/media/audio/common/AudioFormatDescription.h>
#include <aidl/android/media/audio/common/AudioFormatType.h>
#include <aidl/android/media/audio/common/AudioGainMode.h>
#include <aidl/android/media/audio/common/AudioIoFlags.h>
#include <aidl/android/media/audio/common/AudioOutputFlags.h>
#include <audio_control.h>

namespace aidl::android::hardware::automotive::audiocontrol {

namespace audiohalcommon = ::aidl::android::hardware::audio::common;
namespace audiomediacommon = ::aidl::android::media::audio::common;

typedef void (*UpdatePercentCallback)(int32_t state, float progress);
class AudioControlImpl {
  public:
    AudioControlImpl();
    ndk::ScopedAStatus setBalance(float value);
    ndk::ScopedAStatus setVolume(const char* address, int32_t volume);
    ndk::ScopedAStatus setEffectMode(int32_t mode);
    ndk::ScopedAStatus setEffectConfig(int32_t bass, int32_t mid, int32_t treble);
    ndk::ScopedAStatus setBalanceTowardRight(float value);
    ndk::ScopedAStatus setFadeTowardFront(float value);
    ndk::ScopedAStatus updateDsp(int32_t mode);
    std::string getDspVersion();
    static void updataCallback(int32_t state, float progess);
    static void registerUpdateCallback(const std::shared_ptr<IUpdateDspCallback>& in_callback);

  public:
    static audio_control_hw_device *dspDevice;
    static bool dspDefaultLoaded;
    float horizontalRatio;
    float verticalRation;
    static std::shared_ptr<IUpdateDspCallback> mUpdateDspCallback;
};

void loadDspDefault();

}  // namespace aidl::android::hardware::automotive::audiocontrol
#endif  // ANDROID_HARDWARE_AUTOMOTIVE_AUDIOCONTROL_AUDIOCONTROLIMPL_H
