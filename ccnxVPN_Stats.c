/*
 * Copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX OR PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ################################################################################
 * #
 * # PATENT NOTICE
 * #
 * # This software is distributed under the BSD 2-clause License (see LICENSE
 * # file).  This BSD License does not make any patent claims and as such, does
 * # not act as a patent grant.  The purpose of this section is for each contributor
 * # to define their intentions with respect to intellectual property.
 * #
 * # Each contributor to this source code is encouraged to state their patent
 * # claims and licensing mechanisms for any contributions made. At the end of
 * # this section contributors may each make their own statements.  Contributor's
 * # claims and grants only apply to the pieces (source code, programs, text,
 * # media, etc) that they have contributed directly to this software.
 * #
 * # There is no guarantee that this section is complete, up to date or accurate. It
 * # is up to the contributors to maintain their portion of this section and up to
 * # the user of the software to verify any claims herein.
 * #
 * # Do not remove this header notification.  The contents of this section must be
 * # present in all distributions of the software.  You may only modify your own
 * # intellectual property statements.  Please provide contact information.
 *
 * - Palo Alto Research Center, Inc
 * This software distribution does not grant any rights to patents owned by Palo
 * Alto Research Center, Inc (PARC). Rights to these patents are available via
 * various mechanisms. As of January 2016 PARC has committed to FRAND licensing any
 * intellectual property used by its contributions to this software. You may
 * contact PARC at cipo@parc.com for more information or visit http://www.ccnx.org
 */
/**
 * @author Nacho Solis, Christopher A. Wood, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2016, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */
#include <stdio.h>

#include <ccnx/common/ccnx_Name.h>
#include <ccnx/transport/common/transport_MetaMessage.h>

#include <parc/algol/parc_HashMap.h>
#include <parc/algol/parc_Object.h>
#include <parc/algol/parc_DisplayIndented.h>

#include "ccnxVPN_Stats.h"

typedef struct vpn_stats_entry {
    uint64_t sendTimeInUs;
    uint64_t receivedTimeInUs;
    uint64_t rtt;
    size_t size;
    CCNxName *nameSent;
    CCNxMetaMessage *message;
} CCNxVPNStatsEntry;

struct vpn_stats {
    uint64_t totalRtt;
    uint64_t firstTime;
    uint64_t lastTime;
    size_t totalReceived;
    size_t totalSent;
    PARCHashMap *pings;
};

static bool
_ccnxVPNStatsEntry_Destructor(CCNxVPNStatsEntry **statsPtr)
{
    CCNxVPNStatsEntry *entry = *statsPtr;
    ccnxName_Release(&entry->nameSent);
    if (entry->message) {
        ccnxMetaMessage_Release(&entry->message);
    }
    return true;
}

static bool
_ccnxVPNStats_Destructor(CCNxVPNStats **statsPtr)
{
    CCNxVPNStats *stats = *statsPtr;
    parcHashMap_Release(&stats->pings);
    return true;
}

parcObject_Override(CCNxVPNStatsEntry, PARCObject,
                    .destructor = (PARCObjectDestructor *) _ccnxVPNStatsEntry_Destructor);

parcObject_ImplementAcquire(ccnxVPNStatsEntry, CCNxVPNStatsEntry);
parcObject_ImplementRelease(ccnxVPNStatsEntry, CCNxVPNStatsEntry);

CCNxVPNStatsEntry *
ccnxVPNStatsEntry_Create()
{
    return parcObject_CreateInstance(CCNxVPNStatsEntry);
}

parcObject_Override(CCNxVPNStats, PARCObject,
                    .destructor = (PARCObjectDestructor *) _ccnxVPNStats_Destructor);

parcObject_ImplementAcquire(ccnxVPNStats, CCNxVPNStats);
parcObject_ImplementRelease(ccnxVPNStats, CCNxVPNStats);

CCNxVPNStats *
ccnxVPNStats_Create(void)
{
    CCNxVPNStats *stats = parcObject_CreateInstance(CCNxVPNStats);

    stats->pings = parcHashMap_Create();
    stats->totalSent = 0;
    stats->totalReceived = 0;
    stats->totalRtt = 0;
    stats->firstTime = 0;
    stats->lastTime = 0;

    return stats;
}

void
ccnxVPNStats_RecordRequest(CCNxVPNStats *stats, CCNxName *name, uint64_t currentTime)
{
    CCNxVPNStatsEntry *entry = ccnxVPNStatsEntry_Create();

    entry->nameSent = ccnxName_Acquire(name);
    entry->message = NULL;
    entry->sendTimeInUs = currentTime;

    stats->totalSent++;

    if (currentTime < stats->firstTime || stats->firstTime == 0)
    {
        stats->firstTime = currentTime;
    }

    parcHashMap_Put(stats->pings, name, entry);
}

size_t
ccnxVPNStats_RecordResponse(CCNxVPNStats *stats, CCNxName *nameResponse, uint64_t currentTime, CCNxMetaMessage *message)
{
    size_t pingsReceived = stats->totalReceived + 1;
    CCNxVPNStatsEntry *entry = (CCNxVPNStatsEntry *) parcHashMap_Get(stats->pings, nameResponse);

    if (entry != NULL) {
        stats->totalReceived++;

        entry->receivedTimeInUs = currentTime;
        entry->rtt = entry->receivedTimeInUs - entry->sendTimeInUs;
        stats->totalRtt += entry->rtt;

        if (currentTime > stats->lastTime || stats->lastTime == 0)
        {
               stats->lastTime = currentTime;
        }

        CCNxContentObject *contentObject = ccnxMetaMessage_GetContentObject(message);
        PARCBuffer *payload = ccnxContentObject_GetPayload(contentObject);
        entry->size = parcBuffer_Remaining(payload);

        return entry->rtt;
    }

    return 0;
}

bool
ccnxVPNStats_Display(CCNxVPNStats *stats)
{
    FILE* fp = fopen("dropped.csv", "a");
    fprintf(fp, "%d,%.2f\n", stats->totalSent,1.0*(stats->totalSent - stats->totalReceived)/stats->totalSent);
    fclose(fp);

    if (stats->totalReceived > 0) {

        parcDisplayIndented_PrintLine(0, "Sent = %zu : Received = %zu : AvgDelay %llu us",
                                      stats->totalSent, stats->totalReceived, stats->totalRtt / stats->totalReceived);

        return true;
    }

    return false;
}

void
storeThroughput(CCNxVPNStats *stats, long long int payloadSize)
{
    double delay = (stats->totalRtt / stats->totalReceived)/1000000.0; // converting time to seconds
    FILE* fp = fopen("throughput.csv", "a");
    double thgp = (stats->totalReceived * payloadSize * 8) / delay;
    if (delay != 0) {
        fprintf(fp, "%f,%f,%d,%d\n", delay, thgp, stats->lastTime - stats->firstTime, stats->totalReceived * payloadSize * 8);
    }
    fclose(fp);
}
