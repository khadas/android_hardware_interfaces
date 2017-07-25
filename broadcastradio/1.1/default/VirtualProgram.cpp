/*
 * Copyright (C) 2017 The Android Open Source Project
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
#include "VirtualProgram.h"

#include <broadcastradio-utils/Utils.h>

#include "resources.h"

namespace android {
namespace hardware {
namespace broadcastradio {
namespace V1_1 {
namespace implementation {

using V1_0::MetaData;
using V1_0::MetadataKey;
using V1_0::MetadataType;

// TODO (b/36864090): inject this data in a more elegant way
static int gHalVersion = 2;  // 1 = 1.0, 2 = 1.1

void setCompatibilityLevel(int halversion) {
    gHalVersion = halversion;
}

static MetaData createDemoBitmap(MetadataKey key) {
    MetaData bmp = {MetadataType::INT, key, resources::demoPngId, {}, {}, {}};
    if (gHalVersion < 2) {
        bmp.type = MetadataType::RAW;
        bmp.intValue = 0;
        bmp.rawValue = hidl_vec<uint8_t>(resources::demoPng, std::end(resources::demoPng));
    }
    return bmp;
}

VirtualProgram::operator ProgramInfo() const {
    ProgramInfo info11 = {};
    auto& info10 = info11.base;

    utils::getLegacyChannel(selector, &info10.channel, &info10.subChannel);
    info11.selector = selector;
    info10.tuned = true;
    info10.stereo = true;
    info10.digital = utils::isDigital(selector);
    info10.signalStrength = info10.digital ? 100 : 80;

    info10.metadata = hidl_vec<MetaData>({
        {MetadataType::TEXT, MetadataKey::RDS_PS, {}, {}, programName, {}},
        {MetadataType::TEXT, MetadataKey::TITLE, {}, {}, songTitle, {}},
        {MetadataType::TEXT, MetadataKey::ARTIST, {}, {}, songArtist, {}},
        createDemoBitmap(MetadataKey::ICON),
        createDemoBitmap(MetadataKey::ART),
    });

    return info11;
}

// Defining order on virtual programs, how they appear on band.
// It's mostly for default implementation purposes, may not be complete or correct.
bool operator<(const VirtualProgram& lhs, const VirtualProgram& rhs) {
    auto& l = lhs.selector;
    auto& r = rhs.selector;

    // Two programs with the same primaryId is considered the same.
    if (l.programType != r.programType) return l.programType < r.programType;
    if (l.primaryId.type != r.primaryId.type) return l.primaryId.type < r.primaryId.type;
    if (l.primaryId.value != r.primaryId.value) return l.primaryId.value < r.primaryId.value;

    // A little exception for HD Radio subchannel - we check secondary ID too.
    if (utils::hasId(l, IdentifierType::HD_SUBCHANNEL) &&
        utils::hasId(r, IdentifierType::HD_SUBCHANNEL)) {
        return utils::getId(l, IdentifierType::HD_SUBCHANNEL) <
               utils::getId(r, IdentifierType::HD_SUBCHANNEL);
    }

    return false;
}

}  // namespace implementation
}  // namespace V1_1
}  // namespace broadcastradio
}  // namespace hardware
}  // namespace android
