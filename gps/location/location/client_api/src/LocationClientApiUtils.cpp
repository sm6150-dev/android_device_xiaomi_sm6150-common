/* Copyright (c) 2019-2020 The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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
#include <LocationClientApiImpl.h>

namespace location_client {

void translateDiagGnssLocationPositionDynamics(clientDiagGnssLocationPositionDynamics& out,
        const GnssLocationPositionDynamics& in) {
    out.bodyFrameDataMask = in.bodyFrameDataMask;
    out.longAccel = in.longAccel;
    out.latAccel = in.latAccel;
    out.vertAccel = in.vertAccel;
    out.yawRate = in.yawRate;
    out.pitch = in.pitch;
    out.longAccelUnc = in.longAccelUnc;
    out.latAccelUnc = in.latAccelUnc;
    out.vertAccelUnc = in.vertAccelUnc;
    out.yawRateUnc = in.yawRateUnc;
    out.pitchUnc = in.pitchUnc;
}

clientDiagGnss_LocSvSystemEnumType parseDiagGnssConstellation(
        Gnss_LocSvSystemEnumType gnssConstellation) {
    clientDiagGnss_LocSvSystemEnumType constellation;
    switch(gnssConstellation) {
        case GNSS_LOC_SV_SYSTEM_GPS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GPS;
            break;
        case GNSS_LOC_SV_SYSTEM_GALILEO:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GALILEO;
            break;
        case GNSS_LOC_SV_SYSTEM_SBAS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_SBAS;
            break;
        case GNSS_LOC_SV_SYSTEM_GLONASS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_GLONASS;
            break;
        case GNSS_LOC_SV_SYSTEM_BDS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_BDS;
            break;
        case GNSS_LOC_SV_SYSTEM_QZSS:
            constellation = CLIENT_DIAG_GNSS_LOC_SV_SYSTEM_QZSS;
            break;
        default:
            constellation = (clientDiagGnss_LocSvSystemEnumType)~0;
            break;
    }
    return constellation;
}

clientDiagGnssSystemTimeStructType parseDiagGnssTime(
        const GnssSystemTimeStructType &halGnssTime) {

    clientDiagGnssSystemTimeStructType gnssTime;
    memset(&gnssTime, 0, sizeof(gnssTime));
    uint32_t gnssTimeFlags = 0;

    if (GNSS_SYSTEM_TIME_WEEK_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_VALID;
        gnssTime.systemWeek = halGnssTime.systemWeek;
    }
    if (GNSS_SYSTEM_TIME_WEEK_MS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_TIME_WEEK_MS_VALID;
        gnssTime.systemMsec = halGnssTime.systemMsec;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_VALID;
        gnssTime.systemClkTimeBias = halGnssTime.systemClkTimeBias;
    }
    if (GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_CLK_TIME_BIAS_UNC_VALID;
        gnssTime.systemClkTimeUncMs = halGnssTime.systemClkTimeUncMs;
    }
    if (GNSS_SYSTEM_REF_FCOUNT_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_REF_FCOUNT_VALID;
        gnssTime.refFCount = halGnssTime.refFCount;
    }
    if (GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID & halGnssTime.validityMask) {
        gnssTimeFlags |= GNSS_SYSTEM_NUM_CLOCK_RESETS_VALID;
        gnssTime.numClockResets = halGnssTime.numClockResets;
    }

    gnssTime.validityMask = (clientDiagGnssSystemTimeStructTypeFlags)gnssTimeFlags;

    return gnssTime;
}

clientDiagGnssGloTimeStructType parseDiagGloTime(const GnssGloTimeStructType &halGloTime) {

    clientDiagGnssGloTimeStructType gloTime;
    memset(&gloTime, 0, sizeof(gloTime));
    uint32_t gloTimeFlags = 0;

    if (GNSS_CLO_DAYS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_CLO_DAYS_VALID;
        gloTime.gloDays = halGloTime.gloDays;
    }
    if (GNSS_GLO_MSEC_VALID  & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_MSEC_VALID ;
        gloTime.gloMsec = halGloTime.gloMsec;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_VALID;
        gloTime.gloClkTimeBias = halGloTime.gloClkTimeBias;
    }
    if (GNSS_GLO_CLK_TIME_BIAS_UNC_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_CLK_TIME_BIAS_UNC_VALID;
        gloTime.gloClkTimeUncMs = halGloTime.gloClkTimeUncMs;
    }
    if (GNSS_GLO_REF_FCOUNT_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_REF_FCOUNT_VALID;
        gloTime.refFCount = halGloTime.refFCount;
    }
    if (GNSS_GLO_NUM_CLOCK_RESETS_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_NUM_CLOCK_RESETS_VALID;
        gloTime.numClockResets = halGloTime.numClockResets;
    }
    if (GNSS_GLO_FOUR_YEAR_VALID & halGloTime.validityMask) {
        gloTimeFlags |= GNSS_GLO_FOUR_YEAR_VALID;
        gloTime.gloFourYear = halGloTime.gloFourYear;
    }

    gloTime.validityMask = (clientDiagGnssGloTimeStructTypeFlags)gloTimeFlags;

    return gloTime;
}

void translateDiagSystemTime(clientDiagGnssSystemTime& out,
        const GnssSystemTime& in) {
    out.gnssSystemTimeSrc = (clientDiagGnss_LocSvSystemEnumType)in.gnssSystemTimeSrc;
    switch (in.gnssSystemTimeSrc) {
        case GNSS_LOC_SV_SYSTEM_GPS:
           out.u.gpsSystemTime = parseDiagGnssTime(in.u.gpsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_GALILEO:
           out.u.galSystemTime = parseDiagGnssTime(in.u.galSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_GLONASS:
           out.u.gloSystemTime = parseDiagGloTime(in.u.gloSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_BDS:
           out.u.bdsSystemTime = parseDiagGnssTime(in.u.bdsSystemTime);
           break;
        case GNSS_LOC_SV_SYSTEM_QZSS:
           out.u.qzssSystemTime = parseDiagGnssTime(in.u.qzssSystemTime);
           break;
        default:
           break;
    }
}

clientDiagGnssLocationSvUsedInPosition parseDiagLocationSvUsedInPosition(
        const GnssLocationSvUsedInPosition &halSv) {

    clientDiagGnssLocationSvUsedInPosition clientSv;
    clientSv.gpsSvUsedIdsMask = halSv.gpsSvUsedIdsMask;
    clientSv.gloSvUsedIdsMask = halSv.gloSvUsedIdsMask;
    clientSv.galSvUsedIdsMask = halSv.galSvUsedIdsMask;
    clientSv.bdsSvUsedIdsMask = halSv.bdsSvUsedIdsMask;
    clientSv.qzssSvUsedIdsMask = halSv.qzssSvUsedIdsMask;

    return clientSv;
}

void translateDiagGnssSignalType(clientDiagGnssSignalTypeMask& out, GnssSignalTypeMask in) {
    out = (clientDiagGnssSignalTypeMask)0;
    if (in & GNSS_SIGNAL_GPS_L1CA_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L1CA;
    }
    if (in & GNSS_SIGNAL_GPS_L1C_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L1C;
    }
    if (in & GNSS_SIGNAL_GPS_L2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L2;
    }
    if (in & GNSS_SIGNAL_GPS_L5_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GPS_L5;
    }
    if (in & GNSS_SIGNAL_GLONASS_G1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G1;
    }
    if (in & GNSS_SIGNAL_GLONASS_G2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GLONASS_G2;
    }
    if (in & GNSS_SIGNAL_GALILEO_E1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E1;
    }
    if (in & GNSS_SIGNAL_GALILEO_E5A_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5A;
    }
    if (in & GNSS_SIGNAL_GALILEO_E5B_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_GALILEO_E5B;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B1I_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1I;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B1C_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1C;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2I_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2I;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2AI_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AI;
    }
    if (in & GNSS_SIGNAL_QZSS_L1CA_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1CA;
    }
    if (in & GNSS_SIGNAL_QZSS_L1S_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L1S;
    }
    if (in & GNSS_SIGNAL_QZSS_L2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L2;
    }
    if (in & GNSS_SIGNAL_QZSS_L5_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_QZSS_L5;
    }
    if (in & GNSS_SIGNAL_SBAS_L1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_SBAS_L1;
    }
    if (in & GNSS_SIGNAL_NAVIC_L5_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_NAVIC_L5;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2AQ_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2AQ;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B1_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B1;
    }
    if (in & GNSS_SIGNAL_BEIDOU_B2_BIT) {
        out |= CLIENT_DIAG_GNSS_SIGNAL_BEIDOU_B2;
    }
}

void translateDiagGnssMeasUsageInfo(clientDiagGnssMeasUsageInfo& out,
        const GnssMeasUsageInfo& in) {
    clientDiagGnssSignalTypeMask diagGnssSignalType;
    translateDiagGnssSignalType(diagGnssSignalType, in.gnssSignalType);
    out.gnssSignalType = diagGnssSignalType;
   /** Specifies GNSS Constellation Type */
    out.gnssConstellation = parseDiagGnssConstellation(in.gnssConstellation);
    /**  GNSS SV ID.
     For GPS:      1 to 32
     For GLONASS:  65 to 96. When slot-number to SV ID mapping is unknown, set as 255.
     For SBAS:     120 to 151
     For QZSS-L1CA:193 to 197
     For BDS:      201 to 237
     For GAL:      301 to 336 */
    out.gnssSvId = in.gnssSvId;
}

void populateClientDiagLocation(clientDiagGnssLocationStructType* diagGnssLocPtr,
        const GnssLocation& gnssLocation) {
    diagGnssLocPtr->timestamp = gnssLocation.timestamp;
    diagGnssLocPtr->latitude = gnssLocation.latitude;
    diagGnssLocPtr->longitude = gnssLocation.longitude;
    diagGnssLocPtr->altitude = gnssLocation.altitude;
    diagGnssLocPtr->speed = gnssLocation.speed;
    diagGnssLocPtr->bearing = gnssLocation.bearing;
    diagGnssLocPtr->horizontalAccuracy = gnssLocation.horizontalAccuracy;
    diagGnssLocPtr->verticalAccuracy = gnssLocation.verticalAccuracy;
    diagGnssLocPtr->speedAccuracy = gnssLocation.speedAccuracy;
    diagGnssLocPtr->bearingAccuracy = gnssLocation.bearingAccuracy;
    diagGnssLocPtr->flags = gnssLocation.flags;
    diagGnssLocPtr->techMask = gnssLocation.techMask;

    diagGnssLocPtr->gnssInfoFlags = gnssLocation.gnssInfoFlags;
    diagGnssLocPtr->altitudeMeanSeaLevel = gnssLocation.altitudeMeanSeaLevel;
    diagGnssLocPtr->pdop = gnssLocation.pdop;
    diagGnssLocPtr->hdop = gnssLocation.hdop;
    diagGnssLocPtr->vdop = gnssLocation.vdop;
    diagGnssLocPtr->gdop = gnssLocation.gdop;
    diagGnssLocPtr->tdop = gnssLocation.tdop;
    diagGnssLocPtr->magneticDeviation = gnssLocation.magneticDeviation;
    diagGnssLocPtr->horReliability = (clientDiagLocationReliability)gnssLocation.horReliability;
    diagGnssLocPtr->verReliability = (clientDiagLocationReliability)gnssLocation.verReliability;
    diagGnssLocPtr->horUncEllipseSemiMajor = gnssLocation.horUncEllipseSemiMajor;
    diagGnssLocPtr->horUncEllipseSemiMinor = gnssLocation.horUncEllipseSemiMinor;
    diagGnssLocPtr->horUncEllipseOrientAzimuth = gnssLocation.horUncEllipseOrientAzimuth;
    diagGnssLocPtr->northStdDeviation = gnssLocation.northStdDeviation;
    diagGnssLocPtr->eastStdDeviation = gnssLocation.eastStdDeviation;
    diagGnssLocPtr->northVelocity = gnssLocation.northVelocity;
    diagGnssLocPtr->eastVelocity = gnssLocation.eastVelocity;
    diagGnssLocPtr->upVelocity = gnssLocation.upVelocity;
    diagGnssLocPtr->northVelocityStdDeviation = gnssLocation.northVelocityStdDeviation;
    diagGnssLocPtr->eastVelocityStdDeviation = gnssLocation.eastVelocityStdDeviation;
    diagGnssLocPtr->upVelocityStdDeviation = gnssLocation.upVelocityStdDeviation;
    diagGnssLocPtr->svUsedInPosition =
            parseDiagLocationSvUsedInPosition(gnssLocation.svUsedInPosition);
    diagGnssLocPtr->navSolutionMask = gnssLocation.navSolutionMask;
    diagGnssLocPtr->posTechMask = gnssLocation.posTechMask;
    translateDiagGnssLocationPositionDynamics(diagGnssLocPtr->bodyFrameData,
            gnssLocation.bodyFrameData);
    translateDiagSystemTime(diagGnssLocPtr->gnssSystemTime, gnssLocation.gnssSystemTime);
    diagGnssLocPtr->numOfMeasReceived = (uint8_t)gnssLocation.measUsageInfo.size();
    clientDiagGnssMeasUsageInfo measUsage;
    memset(diagGnssLocPtr->measUsageInfo, 0, sizeof(diagGnssLocPtr->measUsageInfo));
    for (int idx = 0; idx < gnssLocation.measUsageInfo.size(); idx++) {
        translateDiagGnssMeasUsageInfo(measUsage, gnssLocation.measUsageInfo[idx]);
        diagGnssLocPtr->measUsageInfo[idx] = measUsage;
    }
    diagGnssLocPtr->leapSeconds = gnssLocation.leapSeconds;
    diagGnssLocPtr->timeUncMs = gnssLocation.timeUncMs;
    diagGnssLocPtr->numSvUsedInPosition = gnssLocation.numSvUsedInPosition;
    diagGnssLocPtr->calibrationConfidencePercent = gnssLocation.calibrationConfidencePercent;
    diagGnssLocPtr->calibrationStatus = gnssLocation.calibrationStatus;
    diagGnssLocPtr->conformityIndex = gnssLocation.conformityIndex;

    struct timespec ts;
    clock_gettime(CLOCK_BOOTTIME, &ts);
    diagGnssLocPtr->bootTimestampNs =
            (ts.tv_sec * 1000000000ULL + ts.tv_nsec);

    diagGnssLocPtr->locOutputEngType =
            (clientDiagLocOutputEngineType) gnssLocation.locOutputEngType;
    diagGnssLocPtr->locOutputEngMask =
            (clientDiagPositioningEngineMask) gnssLocation.locOutputEngMask;
}

void populateClientDiagMeasurements(clientDiagGnssMeasurementsStructType* diagGnssMeasPtr,
        const GnssMeasurements& gnssMeasurements) {
    diagGnssMeasPtr->count = gnssMeasurements.measurements.size();

    diagGnssMeasPtr->clock.flags =
            (clientDiagGnssMeasurementsClockFlagsMask)gnssMeasurements.clock.flags;
    diagGnssMeasPtr->clock.leapSecond = gnssMeasurements.clock.leapSecond;
    diagGnssMeasPtr->clock.timeNs = gnssMeasurements.clock.timeNs;
    diagGnssMeasPtr->clock.timeUncertaintyNs = gnssMeasurements.clock.timeUncertaintyNs;
    diagGnssMeasPtr->clock.fullBiasNs = gnssMeasurements.clock.fullBiasNs;
    diagGnssMeasPtr->clock.biasNs = gnssMeasurements.clock.biasNs;
    diagGnssMeasPtr->clock.biasUncertaintyNs = gnssMeasurements.clock.biasUncertaintyNs;
    diagGnssMeasPtr->clock.driftNsps = gnssMeasurements.clock.driftNsps;
    diagGnssMeasPtr->clock.driftUncertaintyNsps = gnssMeasurements.clock.driftUncertaintyNsps;
    diagGnssMeasPtr->clock.hwClockDiscontinuityCount =
            gnssMeasurements.clock.hwClockDiscontinuityCount;

    for (uint32_t idx = 0; idx < diagGnssMeasPtr->count; ++idx) {
        diagGnssMeasPtr->measurements[idx].flags =
                (clientDiagGnssMeasurementsDataFlagsMask)gnssMeasurements.measurements[idx].flags;
        diagGnssMeasPtr->measurements[idx].svId = gnssMeasurements.measurements[idx].svId;
        diagGnssMeasPtr->measurements[idx].svType =
                (clientDiagGnssSvType)gnssMeasurements.measurements[idx].svType;
        diagGnssMeasPtr->measurements[idx].timeOffsetNs =
                gnssMeasurements.measurements[idx].timeOffsetNs;
        diagGnssMeasPtr->measurements[idx].stateMask =
                (clientDiagGnssMeasurementsStateMask)gnssMeasurements.measurements[idx].stateMask;
        diagGnssMeasPtr->measurements[idx].receivedSvTimeNs =
                gnssMeasurements.measurements[idx].receivedSvTimeNs;
        diagGnssMeasPtr->measurements[idx].receivedSvTimeUncertaintyNs =
                gnssMeasurements.measurements[idx].receivedSvTimeUncertaintyNs;
        diagGnssMeasPtr->measurements[idx].carrierToNoiseDbHz =
                gnssMeasurements.measurements[idx].carrierToNoiseDbHz;
        diagGnssMeasPtr->measurements[idx].pseudorangeRateMps =
                gnssMeasurements.measurements[idx].pseudorangeRateMps;
        diagGnssMeasPtr->measurements[idx].pseudorangeRateUncertaintyMps =
                gnssMeasurements.measurements[idx].pseudorangeRateUncertaintyMps;
        diagGnssMeasPtr->measurements[idx].adrStateMask =
                (clientDiagGnssMeasurementsAdrStateMask)
                        gnssMeasurements.measurements[idx].adrStateMask;
        diagGnssMeasPtr->measurements[idx].adrMeters =
                gnssMeasurements.measurements[idx].adrMeters;
        diagGnssMeasPtr->measurements[idx].adrUncertaintyMeters =
                gnssMeasurements.measurements[idx].adrUncertaintyMeters;
        diagGnssMeasPtr->measurements[idx].carrierFrequencyHz =
                gnssMeasurements.measurements[idx].carrierFrequencyHz;
        diagGnssMeasPtr->measurements[idx].carrierCycles =
                gnssMeasurements.measurements[idx].carrierCycles;
        diagGnssMeasPtr->measurements[idx].carrierPhase =
                gnssMeasurements.measurements[idx].carrierPhase;
        diagGnssMeasPtr->measurements[idx].carrierPhaseUncertainty =
                gnssMeasurements.measurements[idx].carrierPhaseUncertainty;
        diagGnssMeasPtr->measurements[idx].multipathIndicator =
                (clientDiagGnssMeasurementsMultipathIndicator)
                        gnssMeasurements.measurements[idx].multipathIndicator;
        diagGnssMeasPtr->measurements[idx].signalToNoiseRatioDb =
                gnssMeasurements.measurements[idx].signalToNoiseRatioDb;
        diagGnssMeasPtr->measurements[idx].agcLevelDb =
                gnssMeasurements.measurements[idx].agcLevelDb;
    }
}

void translateDiagGnssSv(clientDiagGnssSv& out, const GnssSv& in) {

    /** Unique Identifier */
    out.svId = in.svId;
    /** type of SV (GPS, SBAS, GLONASS, QZSS, BEIDOU, GALILEO) */
    out.type = (clientDiagGnssSvType)in.type;
    /** signal strength */
    out.cN0Dbhz = in.cN0Dbhz;
    /** elevation of SV (in degrees) */
    out.elevation = in.elevation;
    /** azimuth of SV (in degrees) */
    out.azimuth = in.azimuth;
    /** Bitwise OR of GnssSvOptionsBits */
    out.gnssSvOptionsMask = in.gnssSvOptionsMask;
    /** carrier frequency of SV (in Hz) */
    out.carrierFrequencyHz = in.carrierFrequencyHz;
    /** Bitwise OR of clientDiagGnssSignalTypeBits */
    clientDiagGnssSignalTypeMask diagGnssSignalType;
    translateDiagGnssSignalType(diagGnssSignalType, in.gnssSignalTypeMask);
    out.gnssSignalTypeMask = diagGnssSignalType;
}

void populateClientDiagGnssSv(clientDiagGnssSvStructType* diagGnssSvPtr,
        std::vector<GnssSv>& gnssSvs) {
    clientDiagGnssSv diagGnssSv;
    diagGnssSvPtr->count = gnssSvs.size();
    for (int idx = 0; idx < gnssSvs.size(); ++idx) {
        translateDiagGnssSv(diagGnssSv, gnssSvs[idx]);
        diagGnssSvPtr->gnssSvs[idx] = diagGnssSv;
    }
}

void populateClientDiagNmea(clientDiagGnssNmeaStructType *diagGnssNmeaPtr,
        const LocAPINmeaSerializedPayload &nmeaSerializedPayload) {
    diagGnssNmeaPtr->timestamp = nmeaSerializedPayload.timestamp;
    diagGnssNmeaPtr->nmeaLength = nmeaSerializedPayload.length;
    memcpy(&diagGnssNmeaPtr->nmea, nmeaSerializedPayload.nmea, nmeaSerializedPayload.length);
}

}
