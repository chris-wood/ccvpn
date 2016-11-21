/*
 * Copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC)
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
 * @author Kevin Fox, Palo Alto Research Center (Xerox PARC)
 * @copyright (c) 2015, Xerox Corporation (Xerox) and Palo Alto Research Center, Inc (PARC).  All rights reserved.
 */

#include "../athena.c"

#include <LongBow/unit-test.h>

#include <parc/algol/parc_SafeMemory.h>

#include <stdio.h>
#include <sodium.h>

LONGBOW_TEST_RUNNER(athena_pair)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);

    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(athena_pair)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(athena_pair)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, athena_pair_ForwardInterest);
    LONGBOW_RUN_TEST_CASE(Global, athena_pair_ForwardContent);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    __attribute__((unused)) int result = sodium_init();
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDOUT_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }
    return LONGBOW_STATUS_SUCCEEDED;
}

static Athena *
_setupForwarder(char *forwarderName)
{
    unsigned char publicKeyABuffer[crypto_box_PUBLICKEYBYTES];
    unsigned char secretKeyABuffer[crypto_box_SECRETKEYBYTES];
    crypto_box_keypair(publicKeyABuffer, secretKeyABuffer);
    PARCBuffer *publicKeyA = parcBuffer_CreateFromArray(publicKeyABuffer, crypto_box_PUBLICKEYBYTES);
    parcBuffer_Flip(publicKeyA);
    PARCBuffer *secretKeyA = parcBuffer_CreateFromArray(secretKeyABuffer, crypto_box_SECRETKEYBYTES);
    parcBuffer_Flip(secretKeyA);

    CCNxName *gatewayAName = ccnxName_CreateFromCString(forwarderName);

    Athena *gatewayA = athena_CreateWithKeyPair(gatewayAName, 100, secretKeyA, publicKeyA);

    parcBuffer_Release(&publicKeyA);
    parcBuffer_Release(&secretKeyA);
    ccnxName_Release(&gatewayAName);

    return gatewayA;
}

LONGBOW_TEST_CASE(Global, athena_pair_ForwardInterest)
{
    Athena *gatewayA = _setupForwarder("ccnx:/gateway/A");
    Athena *gatewayB = _setupForwarder("ccnx:/gateway/B");

    CCNxName *producerName = ccnxName_CreateFromCString("ccnx:/producer");

    PARCBitVector *bitVector = parcBitVector_Create();
    parcBitVector_Set(bitVector, 1);
    athenaFIB_AddTranslationRoute(gatewayA->athenaFIB, producerName, gatewayB->publicName, gatewayB->publicKey, bitVector);

    CCNxName *interestName = ccnxName_ComposeNAME(producerName, "foo");
    CCNxInterest *interest = ccnxInterest_CreateSimple(interestName);

    // Send the interest to gatewayA
    PARCBitVector *ingressVector = parcBitVector_Create();
    parcBitVector_Set(ingressVector, 7);
    CCNxInterest *encapsulatedInterest = athena_ProcessMessage(gatewayA, interest, ingressVector);

    // Send the encrypted interest to gatewayB
    CCNxInterest *originalInterest = athena_ProcessMessage(gatewayB, encapsulatedInterest, ingressVector);

    // Ensure that the original interest matches the unwrapped interest
    assertTrue(ccnxInterest_Equals(interest, originalInterest), "The original input interest did not match the output decapsulated interest");

    ccnxName_Release(&interestName);
    ccnxName_Release(&producerName);

    ccnxInterest_Release(&interest);
    ccnxInterest_Release(&encapsulatedInterest);
    ccnxInterest_Release(&originalInterest);

    parcBitVector_Release(&bitVector);
    parcBitVector_Release(&ingressVector);

    athena_Release(&gatewayA);
    athena_Release(&gatewayB);
}

static CCNxContentObject *
_createMatchingContentObject(CCNxInterest *interest)
{
    CCNxName *name = ccnxInterest_GetName(interest);
    PARCBuffer *payload = parcBuffer_AllocateCString("hello world. dogs rule. cats are lame.");
    CCNxContentObject *content = ccnxContentObject_CreateWithNameAndPayload(name, payload);
    parcBuffer_Release(&payload);
    return content;
}

LONGBOW_TEST_CASE(Global, athena_pair_ForwardContent)
{
    Athena *gatewayA = _setupForwarder("ccnx:/gateway/A");
    Athena *gatewayB = _setupForwarder("ccnx:/gateway/B");

    CCNxName *producerName = ccnxName_CreateFromCString("ccnx:/producer");

    PARCBitVector *bitVector = parcBitVector_Create();
    parcBitVector_Set(bitVector, 1);
    athenaFIB_AddTranslationRoute(gatewayA->athenaFIB, producerName, gatewayB->publicName, gatewayB->publicKey, bitVector);
    athenaFIB_AddRoute(gatewayB->athenaFIB, producerName, bitVector);

    CCNxName *interestName = ccnxName_ComposeNAME(producerName, "foo");
    CCNxInterest *interest = ccnxInterest_CreateSimple(interestName);

    // Send the interest to gatewayA
    PARCBitVector *ingressVector = parcBitVector_Create();
    parcBitVector_Set(ingressVector, 7);
    CCNxInterest *encapsulatedInterest = athena_ProcessMessage(gatewayA, interest, ingressVector);

    // Send the encrypted interest to gatewayB
    CCNxInterest *originalInterest = athena_ProcessMessage(gatewayB, encapsulatedInterest, ingressVector);

    // From the interest, create a matching content object
    CCNxContentObject *content = _createMatchingContentObject(originalInterest);

    // Send the content through gatewayB and get the encrypted/encapsulated content
    CCNxContentObject *encapsulatedContent = athena_ProcessMessage(gatewayB, content, bitVector);

    // Forward the encapsulated content object to gatewayA
    CCNxContentObject *originalContent = athena_ProcessMessage(gatewayA, encapsulatedContent, bitVector);

    // Check that the decapsulated content matches that which was originally sent
    CCNxName *expectedContentName = ccnxContentObject_GetName(content);
    CCNxName *actualContentName = ccnxContentObject_GetName(originalContent);

    printf("%s\n", ccnxName_ToString(expectedContentName));
    printf("%s\n", ccnxName_ToString(actualContentName));
    PARCBuffer *expectedPayload = ccnxContentObject_GetPayload(content);
    PARCBuffer *actualPayload = ccnxContentObject_GetPayload(originalContent);
    printf("%s\n", parcBuffer_ToString(expectedPayload));
    printf("%s\n", parcBuffer_ToString(actualPayload));

    assertTrue(ccnxContentObject_Equals(content, originalContent), "The decapsulated content does not match the original content");

    ccnxName_Release(&producerName);

    ccnxInterest_Release(&interest);
    ccnxInterest_Release(&encapsulatedInterest);
    ccnxInterest_Release(&originalInterest);

    parcBitVector_Release(&bitVector);
    parcBitVector_Release(&ingressVector);

    athena_Release(&gatewayA);
    athena_Release(&gatewayB);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(athena_pair);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}